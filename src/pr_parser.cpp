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

#include "nut/pr_parser.h"
#include "nut/pr_pratt.h"
#include <string>
#include <sstream>
#include <stdexcept>

namespace pr
{
    /**************************************/
    /*** Private implementation section ***/
    /**************************************/
    
    struct error_message
    {
        int token_type;
        std::string message;
    };
    
    #define DECL_TOKEN_CHAR(name, char) { TOKEN_ ## name, "expected " #char },
    #define DECL_TOKEN_OP(name, str)    { TOKEN_ ## name, "expected " str },
    #define DECL_TOKEN_KW(name, str)    { TOKEN_ ## name, "expected " str },
    #define DECL_TOKEN(name)            { TOKEN_ ## name, "expected " #name },
    
    static error_message error_messages[] =
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int error_messages_size = sizeof(error_messages) / sizeof(error_message);
    
    #undef DECL_TOKEN
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    
    //! Find a default error message by token type.
    //! Returns 0 if not found (bogus bogus !).
    static error_message* parser_find_default_error_message(int token_type)
    {
        for (int i = 0; i < error_messages_size; ++i)
            if (error_messages[i].token_type == token_type)
                return error_messages + i;
        
        return 0;
    }
    
    //! The rules below are not prefixed with parser_rule_
    //!   for the sake of readability.
    //! The equivalent EBNF grammar is shown in the comments before each rule.
    //! Rule names are in lowercase letters, while tokens in UPPERCASE.
    //! The grammar is written in pr_grammar.bnf.
    //! The expression parser is defined in pr_pratt.cpp.
    
    //! Helper rules.
    static ast_node* type_specifier(parser&);
    static ast_node* argument_list(parser&);
    //! Statements.
    static ast_node* declaration_stmt(parser&);
    static ast_node* statement(parser&);
    static ast_node* statement_block(parser&);
    //! Top-level declarators.
    static ast_node* function_decl(parser&);
    
    //! A type specifier.
    //!
    //! type_specifier := IDENTIFIER?type
    static ast_node* type_specifier(parser& par)
    {
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        
        if (!parser_is_type_name(par, tok))
            parser_parse_error(tok, "\"" + tok.value + "\" does not name a type");
        
        // Create the AST node
        type_specifier_node* node = new type_specifier_node(tok);
        node->name = tok.value;
        return node;
    }
    
    //! An argument list.
    //! 
    //! argument_list := LEFT_PAREN (type_specifier IDENTIFIER (type_specifier IDENTIFIER)*)? RIGHT_PAREN
    static ast_node* argument_list(parser& par)
    {
        token tok = parser_expect(par, TOKEN_LEFT_PAREN);
        
        argument_list_node* node = new argument_list_node(tok);
        
        while (lexer_peekt(par.lex) != TOKEN_RIGHT_PAREN)
        {
            // Create the argument AST node
            argument_node* arg_node = new argument_node(lexer_peek(par.lex));
            
            // Get the argument's type
            ast_add_child(arg_node, type_specifier(par));
            
            // Get its name & check the declaration
            token tok = parser_expect(par, TOKEN_IDENTIFIER);
            parser_check_declaration(par, tok);
            arg_node->name = tok.value;
            
            // Add it to the current scope
            symbol sym;
            sym.name = tok.value;
            sym.flags = SYM_FLAG_VARIABLE;
            sym.info = tok.info;
            scope_add(par.ctx.scp, sym);
            
            // Eat comma, if needed
            if (lexer_peekt(par.lex) != TOKEN_RIGHT_PAREN)
                parser_expect(par, TOKEN_COMMA);
            
            // Append the argument node the the list
            ast_add_child(node, arg_node);
        }
        
        parser_expect(par, TOKEN_RIGHT_PAREN);
        
        return node;
    }
    
    //! A variable declaration statement.
    //!
    //! declaration_stmt := type_specifier IDENTIFIER (EQUALS pratt_expression)? SEMICOLON
    static ast_node* declaration_stmt(parser& par)
    {
        declaration_stmt_node* node = new declaration_stmt_node(lexer_peek(par.lex));
        
        // Type of the variable
        ast_add_child(node, type_specifier(par));
        
        // Get its name and check for multiple declarations
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        parser_check_declaration(par, tok);
        node->name = tok.value;
        
        // Add it to the current scope as soon as possible
        symbol sym;
        sym.name = tok.value;
        sym.flags = SYM_FLAG_VARIABLE;
        sym.info = tok.info;
        scope_add(par.ctx.scp, sym);
        
        // If we have an initializer
        if (lexer_peekt(par.lex) == TOKEN_EQUALS)
        {
            lexer_get(par.lex);
            ast_add_child(node, pratt_expression(par));
        }
        
        parser_expect(par, TOKEN_SEMICOLON);
        
        return node;
    }
    
    //! A single statement, terminated with a semicolon.
    //!
    //! statement := declaration_stmt
    //!            | pratt_expression
    static ast_node* statement(parser& par)
    {
        statement_node* node = new statement_node(lexer_peek(par.lex));
        
        // If the next token is a type name identifier, this
        //   is a declaration
        token tok = lexer_peek(par.lex);
        if (tok.type == TOKEN_IDENTIFIER)
        {
            if (parser_is_type_name(par, tok))
                ast_add_child(node, declaration_stmt(par));
        }
        // Otherwise we expect an expression
        else
        {
            ast_add_child(node, pratt_expression(par));
        }
        
        return node;
    }
    
    //! A statement block, enclosed in curly braces.
    //!
    //! statement_block := LEFT_CURLY statement* RIGHT_CURLY
    static ast_node* statement_block(parser& par)
    {
        token tok = parser_expect(par, TOKEN_LEFT_CURLY);
        
        statement_block_node* node = new statement_block_node(tok);
        
        while (lexer_peekt(par.lex) != TOKEN_RIGHT_CURLY)
            ast_add_child(node, statement(par));
        
        parser_expect(par, TOKEN_RIGHT_CURLY);
        
        return node;
    }
    
    //! A function declaration.
    //!
    //! function_decl := type_specifier IDENTIFIER
    //!                  LEFT_PAREN argument_list RIGHT_PAREN
    //!                  statement_block
    static ast_node* function_decl(parser& par)
    {   
        // Return type
        ast_node* ret_type = type_specifier(par);
        
        // Create node
        function_decl_node* node = new function_decl_node(lexer_peek(par.lex));
        ast_add_child(node, ret_type);
        
        // Get its name and check for multiple definitions
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        parser_check_declaration(par, tok);
        node->name = tok.value;
        
        // Add the function to the current scope
        symbol sym;
        sym.name = tok.value;
        sym.flags = SYM_FLAG_FUNCTION;
        sym.info = tok.info;
        scope_add(par.ctx.scp, sym);
        
        // Push a new scope
        scope_push(par.ctx.scp);
        
        // Arguments specification
        ast_add_child(node, argument_list(par));
        
        // Function body
        ast_add_child(node, statement_block(par));
        
        // Pop the function scope
        //FIXME: save the layer in the function representation in the AST
        scope_pop(par.ctx.scp);
        
        return node;
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    parser parser_create(lexer& lex, context& ctx)
    {
        parser par(lex, ctx);
        return par;
    }
    
    void parser_free(parser&)
    {}
    
    ast_node* parser_parse_program(parser& par)
    {
        return function_decl(par);
    }
    
    void parser_parse_error(token const& tok, std::string const& msg)
    {
        std::ostringstream ss;
        ss << "parse error: line " << tok.info.line;
        ss << ", col " << tok.info.column << ": " << msg;
        throw std::logic_error(ss.str());
    }
    
    token parser_expect(parser& par, int type, std::string const& err_msg, bool eat)
    {
        std::string msg = err_msg;
        
        if (!msg.size())
        {
            error_message* emsg = parser_find_default_error_message(type);
            if (!emsg)
                msg = "bogus bogus ! can't find a default error message for this token :(";
            else
                msg = emsg->message;
        }
        
        if (lexer_peekt(par.lex) != type)
            parser_parse_error(lexer_peek(par.lex), msg);
        
        return eat ? lexer_get(par.lex) : token();
    }
    
    void parser_check_declaration(parser& par, token const& tok)
    {
        symbol* sym = scope_find_innermost(par.ctx.scp, tok.value);
        symbol* glob_sym = scope_find(par.ctx.scp, tok.value);
        //! Issue an error either if :
        //!   - the symbol is already declared in the current scope layer
        //!   - the symbol is a builtin
        if (sym || (glob_sym && glob_sym->flags & SYM_FLAG_BUILTIN))
        {
            std::ostringstream ss;
            ss << "symbol `" << tok.value << "' is already declared ";
            
            if (sym && !(sym->flags & SYM_FLAG_BUILTIN))
                ss << "(previously declared at line " << sym->info.line << ", col " << sym->info.column << ")";
            else if (sym || glob_sym)
                ss << "(`" << tok.value << "' is a builtin symbol)";
            
            parser_parse_error(tok, ss.str());
        }
    }
    
    bool parser_is_type_name(parser& par, token const& tok)
    {
        symbol* sym = scope_find(par.ctx.scp, tok.value);
        if (!sym)
            return false;
        
        return sym->flags & SYM_FLAG_TYPE;
    }
    
    bool parser_is_variable_name(parser& par, token const& tok)
    {
        symbol* sym = scope_find(par.ctx.scp, tok.value);
        if (!sym)
            return false;
        
        return sym->flags & SYM_FLAG_VARIABLE;
    }
    
    bool parser_is_function_name(parser& par, token const& tok)
    {
        symbol* sym = scope_find(par.ctx.scp, tok.value);
        if (!sym)
            return false;
        
        return sym->flags & SYM_FLAG_FUNCTION;
    }
}
