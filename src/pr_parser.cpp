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
#include <string>
#include <sstream>
#include <stdexcept>

namespace pr
{
    /**************************************/
    /*** Private implementation section ***/
    /**************************************/
    
    //! Throw out an error, printing the message msg and location of the token tok.
    static void parser_parse_error(token const& tok, std::string const& msg)
    {
        std::ostringstream ss;
        ss << "pr::parser_parse_error: line " << tok.info.line;
        ss << ", col " << tok.info.column << ": " << msg;
        throw std::logic_error(ss.str());
    }
    
    //! Check that the next token is of the given type.
    //! Throw an error if it is not the case, with err_msg.
    //! If eat == true, consume the token and return it.
    //! If eat == false, the return value is uninitialized.
    static token parser_expect(parser& par, int type, std::string const& err_msg, bool eat = true)
    {
        if (lexer_peekt(par.lex) != type)
            parser_parse_error(lexer_peek(par.lex), err_msg);
        
        return eat ? lexer_get(par.lex) : token();
    }
    
    //! The rules below are not prefixed with parser_rule_
    //!   for readability purposed, and are declared static.
    //! The equivalent EBNF grammar is shown in the comments before.
    //! Rule names are in lowercase letters, while tokens in UPPERCASE.
    
    //! A type specifier.
    //!
    //! type_specifier := TYPE_NAME
    static void type_specifier(parser& par)
    {
        parser_expect(par, TOKEN_TYPE_NAME, "expected type specifier", false);
        
        lexer_get(par.lex);
    }
    
    //! An expression.
    //!
    //! expression := 
    static void expression(parser&)
    {}
    
    //! A variable declaration statement.
    //!
    //! declaration_stmt := type_specifier IDENTIFIER (EQUALS expression)? SEMICOLON
    static void declaration_stmt(parser& par)
    {
        // Type of the variable
        type_specifier(par);
        
        // Get its name and check for multiple declarations
        token tok = parser_expect(par, TOKEN_IDENTIFIER, "expected identifier");
        if (scope_find_innermost(par.ctx.scp, tok.value))
            parser_parse_error(tok, "variable `" + tok.value + "' redeclared");
        
        // Add it to the current scope as soon as possible
        symbol sym;
        sym.name = tok.value;
        sym.flags = SYM_FLAG_VARIABLE;
        scope_add(par.ctx.scp, sym);
        
        // If we have an initializer
        if (lexer_peekt(par.lex) == TOKEN_EQUALS)
        {
            lexer_get(par.lex);
            expression(par);
        }
        
        parser_expect(par, TOKEN_SEMICOLON, "expected `;'");
    }
    
    //! An assignment statement.
    //!
    //! assignment_stmt := VARIABLE_NAME EQUALS expression SEMICOLON
    static void assignment_stmt(parser& par)
    {
        // Get the variable's name
        std::string name = parser_expect(par, TOKEN_VARIABLE_NAME, "expected a variable name").value;
        // Parse the equal sign
        parser_expect(par, TOKEN_EQUALS, "expected `='");
        
        // Get the expression
        expression(par);
        
        parser_expect(par, TOKEN_SEMICOLON, "expected `;'");
    }
    
    static void function_call_stmt(parser& par)
    {
        // function_call(par);
        parser_expect(par, TOKEN_SEMICOLON, "expected `;'");
    }
    
    //! A single statement, terminated with a semicolon.
    //!
    //! statement := declaration_stmt
    //!            | assignment_stmt
    //!            | function_call_stmt
    static void statement(parser& par)
    {
        token tok = lexer_peek(par.lex);
        
        switch (tok.type)
        {
            //FIXME: this stmt begins with type_specifier, that may not start with TOKEN_TYPE_NAME !!!
            case TOKEN_TYPE_NAME:
                declaration_stmt(par);
                break;
                
            case TOKEN_VARIABLE_NAME:
                assignment_stmt(par);
                break;
                
            case TOKEN_FUNCTION_NAME:
                function_call_stmt(par);
                break;
                
            case TOKEN_IDENTIFIER:
                parser_parse_error(tok, "use of undeclared identifier `" + tok.value + "'");
                
            default:
                parser_parse_error(tok, "unexpected token");
        }
    }
    
    //! A statement block, enclosed in curly braces.
    //!
    //! statement_block := LEFT_CURLY statement* RIGHT_CURLY
    static void statement_block(parser& par)
    {
        parser_expect(par, TOKEN_LEFT_CURLY, "expected `{'");
        
        while (lexer_peekt(par.lex) != TOKEN_RIGHT_CURLY)
        {
            statement(par);
        }
        
        parser_expect(par, TOKEN_RIGHT_CURLY, "expected `}'");
    }
    
    //! A function declaration.
    //!
    //! function_declaration := type_specifier IDENTIFIER
    //!                         LEFT_PAREN argument_list RIGHT_PAREN
    //!                         statement_block
    static void function_declaration(parser& par)
    {
        // Return type
        type_specifier(par);
        
        // Get its name and check for multiple definitions
        token tok = parser_expect(par, TOKEN_IDENTIFIER, "expected identifier");
        if (scope_find_innermost(par.ctx.scp, tok.value))
            parser_parse_error(tok, "function `" + tok.value + "' is already defined");
        
        // Add the function to the current scope
        symbol sym;
        sym.name = tok.value;
        sym.flags = SYM_FLAG_FUNCTION;
        scope_add(par.ctx.scp, sym);
        
        // Push a new scope
        scope_push(par.ctx.scp);
        
        // Arguments specification
        parser_expect(par, TOKEN_LEFT_PAREN, "expected `('");
        // argument_list();
        parser_expect(par, TOKEN_RIGHT_PAREN, "expected `)'");
        
        // Function body
        statement_block(par);
        
        // Pop the function scope
        //FIXME: probably save the layer in the function description
        scope_pop(par.ctx.scp);
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
    
    void parser_parse_program(parser& par)
    {
        function_declaration(par);
    }
}
