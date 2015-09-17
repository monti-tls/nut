/* This file is part of nut.
 * 
 * Copyright (c) 2015, Alexandre Monti
 * 
 * nut is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * nut is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with nut.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nut/sem_passman.h"
#include "nut/sem_declarator.h"
#include <sstream>
#include <stdexcept>
#include <iostream> // for std::cerr

namespace sem
{
    using namespace pr;
    
    /*************************************/
    /*** Private module implementation ***/
    /*************************************/
    
    passman::passman(pr::parser& par) : par(par)
    { }
    
    //! Generate an (empty) table for the built-in types.
    #define DECL_BUILTIN_TYPE(name, flags) { },
    
    type builtin_types[] = {
        #include "nut/sem_builtins.inc"
    };
    
    static int builtin_types_size = sizeof(builtin_types) / sizeof(type);
    
    #undef DECL_BUILTIN_TYPE
    
    //! Find a built-in type by name.
    //! Returns 0 if not found.
    static type* find_builtin_type(std::string const& name)
    {
        static bool inited = false;
        if (!inited)
        {
            int i = 0;
            type* tp;
            
            #define DECL_BUILTIN_TYPE(nm, fl) \
                tp = builtin_types + i++; \
                tp->tag = TYPE_DECLARATOR; \
                tp->name = #nm; \
                tp->self = tp; \
                tp->flags = fl;
            
            #include "nut/sem_builtins.inc"
            
            #undef DECL_BUILTIN_TYPE
            
            inited = true;
        }
        
        for (int i = 0; i < builtin_types_size; ++i)
            if (builtin_types[i].name == name)
                return builtin_types + i;
        
        return 0;
    }
    
    //! Emit a semantic error about a node.
    //! This throws an exception with the associated line and column.
    static void pass_error(passman& pman, ast_node* node, std::string const& msg)
    {
        std::ostringstream ss;
        ss << "semantic error: " << parser_token_information(node->saved_tok) << msg << std::endl;
        ss << parser_error_line(pman.par, node->saved_tok);
        throw std::logic_error(ss.str());
    }
    
    //! Emit a semantic warning about a node.
    //! This prints to stderr with the associated line and column.
    static void pass_warning(passman& pman, ast_node* node, std::string const& msg)
    {
        std::ostringstream ss;
        ss << "warning: " << parser_token_information(node->saved_tok) << msg << std::endl;
        ss << parser_error_line(pman.par, node->saved_tok);
        
        std::cerr << ss.str() << std::endl;
    }
    
    //! Resolve a declarator in the node's subtree.
    //! Returns 0 if not found.
    static declarator* resolve_inner_declarator(std::string const& name, ast_node* node)
    {
        if (node->decl && node->decl->name == name)
            return node->decl;
        
        for (unsigned int i = 0; i < node->children.size(); ++i)
        {
            declarator* decl = resolve_inner_declarator(name, node->children[i]);
            if (decl)
                return decl;
        }
        
        return 0;
    }
    
    //! Resolve a declarator by name in the AST.
    //! Returns the first declarator whose name is matching
    //!   regardless of its type.
    //! Returns 0 if not found.
    //WARNING: this has exponential run time in AST depth
    //         because it calls resolve_inner_declarator on each node, then on node->parent
    //         so each tree is examined multiple times :/
    static declarator* resolve_declarator(std::string const& name, ast_node* node)
    {
        type* builtin = find_builtin_type(name);
        if (builtin)
            return builtin;
        
        if (!node)
            return 0;
        
        // Search in previous nodes (including this one)
        for (ast_node* it = node; it; it = it->prev)
        {
            declarator* decl = resolve_inner_declarator(name, it);
            if (decl)
                return decl;
        }
        
        // If it is a function, also search in its arguments
        if (node->tag == FUNCTION_DECL)
        {
            if (!node->decl) throw std::runtime_error("sem::resolve_declarator: internal error: null declarator");
            function* fun = node->decl->as_function;
            
            for (unsigned int i = 0; i < fun->arguments.size(); ++i)
                if (fun->arguments[i]->name == name)
                    return fun->arguments[i];
        }
        
        // Search in the node's parent, if null returns 0
        return resolve_declarator(name, node->parent);
    }
    
    //! Resolve the current function declarator.
    static function* resolve_function_declarator(ast_node* node)
    {
        if (!node)
            return 0;
        
        if (node->decl && node->decl->tag == FUNCTION_DECLARATOR)
            return node->decl->as_function;
        
        return resolve_function_declarator(node->parent);
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    passman passman_create(pr::parser& par)
    {
        passman pman(par);
        return pman;
    }

    void passman_free(passman&)
    { }
    
    void passman_run_all(passman& pman, pr::ast_node* node)
    {
        pass_fix_ast(pman, node);
        pass_create_declarators(pman, node);
        pass_check_calls(pman, node);
        pass_resolve_result_types(pman, node);
        pass_type_check(pman, node);
        pass_unused_expression_results(pman, node);
        pass_unreachable_code(pman, node);
    }
    
    void pass_fix_ast(passman& pman, ast_node* node)
    {
        int n = (int) node->children.size();
        
        for (int i = 0; i < n; ++i)
        {
            ast_node* child = node->children[i];
            
            // Patch parent pointer
            child->parent = node;
            // Patch previous pointer if not first
            if (i != 0)
                child->prev = node->children[i-1];
            // Patch next pointer if not last
            if (i != n-1)
                child->next = node->children[i+1];
            
            // Fix this node's subtree
            pass_fix_ast(pman, child);
        }
    }
    
    void pass_create_declarators(passman& pman, ast_node* node)
    {
        switch (node->tag)
        {
            case DECLARATION_STMT:
            {
                declaration_stmt_node* stmt = node->as_declaration_stmt;
                
                // Create a declarator with the appropriate name and type
                variable* var = variable_create(stmt->name);
                var->tp = resolve_declarator(stmt->children[0]->as_type_specifier->name, stmt)->as_type;
                
                node->decl = var;
                break;
            }
            
            case ARGUMENT:
            {
                argument_node* arg = node->as_argument;
                
                variable* var = variable_create(arg->name);
                var->tp = resolve_declarator(arg->children[0]->as_type_specifier->name, arg)->as_type;
                
                node->decl = var;
                break;
            }
            
            case FUNCTION_DECL:
            {
                // Some casted helper pointers
                function_decl_node* stmt = node->as_function_decl;
                type_specifier_node* stmt_ret_tp = stmt->children[0]->as_type_specifier;
                argument_list_node* stmt_args = stmt->children[1]->as_argument_list;
                
                // Create a declarator with the appropriate name and type
                function* fun = function_create(stmt->name);
                fun->ret_tp = resolve_declarator(stmt_ret_tp->name, stmt)->as_type;
                
                // Create arguments specifications
                for (unsigned int i = 0; i < stmt_args->children.size(); ++i)
                {
                    argument_node* stmt_arg = stmt_args->children[i]->as_argument;
                    
                    variable* arg = variable_create(stmt_arg->name);
                    arg->tp = resolve_declarator(stmt_arg->children[0]->as_type_specifier->name, stmt_arg)->as_type;
                    fun->arguments.push_back(arg);
                }
                
                node->decl = fun;
                break;
            }
        }
        
        for (unsigned int i = 0; i < node->children.size(); ++i)
            pass_create_declarators(pman, node->children[i]);
    }

    void pass_check_calls(passman& pman, ast_node* node)
    {
        if (node->tag == FUNCTION_CALL_EXPR)
        {
            // Check if the called object is an identifier
            ast_node* id = node->children[0];
            if (id->tag != IDENTIFIER_EXPR)
                pass_error(pman, node, "function calls are only supported on identifiers");
            std::string name = id->as_identifier_expr->name;
            
            // Get the associated declarator
            declarator* fun = resolve_declarator(name, node);
            
            // This is an internal error, because the parser already checks for
            //   uses of undeclared identifiers
            if (!fun) throw std::runtime_error("sem::pass_check_calls: internal error: declarator not found");
            
            // Check if the resolved object is a function
            if (fun->tag != FUNCTION_DECLARATOR)
                pass_error(pman, node, "'" + name + "' is not a function");
            
            // Number of arguments that the function expects
            int arity = fun->as_function->arguments.size();
            
            // Count the number of given arguments
            int call_arity = 0;
            if (node->children.size() > 1)
            {
                call_arity = 1;
                
                //! There is as much arguments as list nodes plus one.
                ast_node* lst = node->children[1];
                while (lst->tag == LIST_EXPR)
                {
                    ++call_arity;
                    lst = lst->as_list_expr->children[0];
                }
            }
            
            // Check the arity of the call
            if (call_arity != arity)
            {
                std::ostringstream ss;
                
                ss << "'" << name << "' expects " << arity << " arguments ";
                ss << "(" << call_arity << " given)";
                
                pass_error(pman, node, ss.str());
            }
        }
        
        for (unsigned int i = 0; i < node->children.size(); ++i)
            pass_check_calls(pman, node->children[i]);
    }
    
    void pass_resolve_result_types(passman& pman, pr::ast_node* node)
    {
        switch (node->tag)
        {
            //! The expression node is just a wrapper.
            case EXPRESSION:
                pass_resolve_result_types(pman, node->children[0]);
                node->res_tp = node->children[0]->res_tp;
                break;
            
            //! Trivial for literals.
            case INTEGER_LITERAL_EXPR:
                node->res_tp = find_builtin_type("int");
                break;
                
            //! For identifiers, find the declarator and
            //!   take the declared type.
            case IDENTIFIER_EXPR:
            {
                declarator* decl = resolve_declarator(node->as_identifier_expr->name, node);
                if (!decl) throw std::runtime_error("sem::pass_resolve_result_types: internal error: null declarator");
                
                if (decl->tag != VARIABLE_DECLARATOR)
                    pass_error(pman, node, "invalid use of identifier '" + node->as_identifier_expr->name + "'");
                
                node->res_tp = decl->as_variable->tp;
                break;
            }
                
            // For function calls, find the function declarator
            //   and take its return type
            case FUNCTION_CALL_EXPR:
            {
                // The declarator is guaranteed to be a function
                declarator* decl = resolve_declarator(node->children[0]->as_identifier_expr->name, node);
                if (!decl || decl->tag != FUNCTION_DECLARATOR)
                    throw std::runtime_error("sem::pass_resolve_result_types: internal error: invalid call declarator");
                
                // Write out expression result's type
                node->res_tp = decl->as_function->ret_tp;
                
                // Don't forget to generate type information for call expressions
                pass_resolve_result_types(pman, node->children[1]);
                break;
            }
            
            //! Unary operators that results in the same type than
            //!   their sub-expression.
            case INC_EXPR:
            case DEC_EXPR:
            case NEG_EXPR:
            case NOT_EXPR:
            {
                ast_node* sub_expr = node->children[0];
                
                pass_resolve_result_types(pman, sub_expr);
                
                node->res_tp = sub_expr->res_tp;
                break;
            }
            
            //! Binary operators that results in the same type than
            //!   their sub-expression.
            //! They require that the two operands be of the same type.
            case ADD_EXPR:
            case SUB_EXPR:
            case MUL_EXPR:
            case DIV_EXPR:
            case ASSIGNMENT_EXPR:
            {
                // Resolve the left hand side sub-expression's result type
                ast_node* sub_expr = node->children[0];
                pass_resolve_result_types(pman, sub_expr);
                type* lhs_res_tp = sub_expr->res_tp;
                
                // Resolve the right hand side sub-expression's result type
                sub_expr = node->children[1];
                pass_resolve_result_types(pman, sub_expr);
                type* rhs_res_tp = sub_expr->res_tp;
                
                // Check for compatibility
                if (lhs_res_tp->name != rhs_res_tp->name)
                {
                    std::ostringstream ss;
                    ss << "operation between incompatible types '";
                    ss << lhs_res_tp->name << "' and '";
                    ss << rhs_res_tp->name << "'";
                    pass_error(pman, node, ss.str());
                }
                
                node->res_tp = lhs_res_tp;
            }
            
            default:
                for (unsigned int i = 0; i < node->children.size(); ++i)
                    pass_resolve_result_types(pman, node->children[i]);
        }
    }
    
    void pass_type_check(passman& pman, pr::ast_node* node)
    {
        switch (node->tag)
        {
            case DECLARATION_STMT:
            case ARGUMENT:
            {
                type* decl_tp = node->decl->as_variable->tp;
                
                // Check for void variable declarations
                if (decl_tp->flags & TYPE_FLAG_NONCOPYABLE)
                    pass_error(pman, node, "variable '" + node->as_declaration_stmt->name + "' declared void");
                
                // If there is an initialization, check for type incompatibility
                if (node->children.size() > 1)
                {
                    type* init_tp = node->children[1]->res_tp;
                    if (decl_tp->name != init_tp->name)
                        pass_error(pman, node, "initializing variable with incompatible type '" +  init_tp->name + "'");
                    
                    // Recurse the call in the expression
                    pass_type_check(pman, node->children[1]);
                }
                break;
            }
            
            case FUNCTION_CALL_EXPR:
            {
                // The declarator is guaranteed to be a function
                //   because other passes checked this up (as well for children[0]
                //   being an identifier_expr_node)
                std::string name = node->children[0]->as_identifier_expr->name;
                function* fun = resolve_declarator(name, node)->as_function;
                
                // It is guaranteed that the argument count matches the function declarator
                for (int i = 0; i < (int) fun->arguments.size(); ++i)
                {
                    ast_node* arg = node->children[1]->children[i];
                    type* decl_tp = fun->arguments[i]->tp;
                    type* res_tp = arg->res_tp;
                    
                    if (decl_tp->name != res_tp->name)
                        pass_error(pman, arg, "initializing parameter with incompatible type '" + res_tp->name + "'");
                }
                
                break;
            }
            
            case RETURN_STMT:
            {
                function* fun = resolve_function_declarator(node);
                if (!fun) throw std::runtime_error("sem::pass_type_check: internal error: null function declarator");
                
                type* tp;
                if (!node->children.size())
                    tp = find_builtin_type("void");
                else
                    tp = node->children[0]->res_tp;
                
                if (tp->name != fun->ret_tp->name)
                {
                    if (tp->flags & TYPE_FLAG_NONCOPYABLE)
                        pass_error(pman, node, "this function expects a return value");
                    else if (fun->ret_tp->flags & TYPE_FLAG_NONCOPYABLE)
                        pass_error(pman, node, "this function does not expects a return value");
                    else
                        pass_error(pman, node, "returning with incompatible type '" + tp->name + "'");
                }
                break;
            }
            
            default:
                for (unsigned int i = 0; i < node->children.size(); ++i)
                    pass_type_check(pman, node->children[i]);
        }
    }
    
    void pass_unused_expression_results(passman& pman, pr::ast_node* node)
    {
        //! Here we search for simple expression statements (including function
        //!   calls).
        if (node->tag == STATEMENT)
        {
            ast_node* expr = node->children[0];
            
            if (expr->tag == EXPRESSION)
            {
                bool warn = false;
                
                if (expr->children[0]->tag == FUNCTION_CALL_EXPR)
                {
                    identifier_expr_node* id = expr->children[0]->children[0]->as_identifier_expr;
                    // This is guaranteed to success
                    function* fun = resolve_declarator(id->name, node)->as_function;
                    
                    // If the function returns a void result
                    if (fun->ret_tp->flags & TYPE_FLAG_NONCOPYABLE)
                        warn = true;
                }
                
                if (!warn)
                    pass_warning(pman, expr, "unused expression result");
            }
        }
        
        for (unsigned int i = 0; i < node->children.size(); ++i)
            pass_unused_expression_results(pman, node->children[i]);
    }
    
    void pass_unreachable_code(passman& pman, pr::ast_node* node)
    {
        // node->parent is a STATEMENT node wrapper,
        //   so if node->parent->next != 0, there is another statement after this one
        if (node->tag == RETURN_STMT && node->parent->next)
            pass_warning(pman, node, "code is unreachable after this return statement");
        
        for (unsigned int i = 0; i < node->children.size(); ++i)
            pass_unreachable_code(pman, node->children[i]);
    }
}
