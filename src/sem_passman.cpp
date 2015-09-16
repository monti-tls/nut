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

namespace sem
{
    using namespace pr;
    
    /*************************************/
    /*** Private module implementation ***/
    /*************************************/
    
    passman::passman(pr::parser& par) : par(par)
    { }
    
    //! Emit a semantic error about a node.
    //! This throws an exception with the associated line and column.
    static void pass_error(passman& pman, ast_node* node, std::string const& msg)
    {
        std::ostringstream ss;
        ss << "semantic error: " << parser_token_information(node->saved_tok) << msg << std::endl;
        ss << parser_error_line(pman.par, node->saved_tok);
        throw std::logic_error(ss.str());
    }
    
    //! Resolve a declarator by name in the AST.
    //! Returns the first declarator whose name is matching
    //!   regardless of its type.
    //! Returns 0 if not found.
    static declarator* resolve_declarator(std::string const& name, ast_node* node)
    {
        if (!node)
            return 0;
        
        // Search in previous nodes (including this one)
        for (ast_node* it = node; it; it = it->prev)
        {
            if (it->decl && it->decl->name == name)
                return it->decl;
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
                var->tp = type_create(stmt->children[0]->as_type_specifier->name);
                
                node->decl = var;
                break;
            }
            
            case FUNCTION_DECL:
            {
                // Some casted helped pointers
                function_decl_node* stmt = node->as_function_decl;
                type_specifier_node* stmt_ret_tp = stmt->children[0]->as_type_specifier;
                argument_list_node* stmt_args = stmt->children[1]->as_argument_list;
                
                // Create a declarator with the appropriate name and type
                function* fun = function_create(stmt->name);
                fun->ret_tp = type_create(stmt_ret_tp->name);
                
                // Create arguments specifications
                for (unsigned int i = 0; i < stmt_args->children.size(); ++i)
                {
                    argument_node* stmt_arg = stmt_args->children[i]->as_argument;
                    
                    variable* arg = variable_create(stmt_arg->name);
                    arg->tp = type_create(stmt_arg->children[0]->as_type_specifier->name);
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
            
            //! This is an internal error, because the parser already checks for
            //!   uses of undeclared identifiers.
            if (!fun) throw std::runtime_error("sem::pass_check_calls: internal error: declarator not found");
            
            //! Check if the resolved object is a function.
            if (fun->tag != FUNCTION_DECLARATOR)
                pass_error(pman, node, "'" + name + "' is not a function");
            
            //! Check the arity of the call.
            int arity = fun->as_function->arguments.size();
            
            //! Count the number of given arguments.
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
}
