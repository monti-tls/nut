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
    
    //! Throw out an error, printing the message msg and location of the token tok.
    static void parser_parse_error(token const& tok, std::string const& msg)
    {
        std::ostringstream ss;
        ss << "pr::parser_parse_error: line " << tok.info.line;
        ss << ", col " << tok.info.column << ": " << msg;
        throw std::logic_error(ss.str());
    }
    
    //! Check that the next token is of the given type.
    //! Throw an error if it is not the case.
    //! If err_msg is empty, it outputs the default error message (automatic), otherwise
    //!   it outputs err_msg.
    //! If eat == true, consume the token and return it.
    //! If eat == false, the return value is uninitialized.
    static token parser_expect(parser& par, int type, std::string const& err_msg = "", bool eat = true)
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
    
    //! Check an identifier token against multiple declarations.
    static void parser_check_declaration(parser& par, token const& tok)
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
    
    //! Check if an identifier token is a type name.
    static bool parser_is_type_name(parser& par, token const& tok)
    {
        symbol* sym = scope_find(par.ctx.scp, tok.value);
        if (!sym)
            return false;
        
        return sym->flags & SYM_FLAG_TYPE;
    }
    
    //! Check if an identifier token is a variable name.
    static bool parser_is_variable_name(parser& par, token const& tok)
    {
        symbol* sym = scope_find(par.ctx.scp, tok.value);
        if (!sym)
            return false;
        
        return sym->flags & SYM_FLAG_VARIABLE;
    }
    
    //! Check if an identifier token is a function name.
    static bool parser_is_function_name(parser& par, token const& tok)
    {
        symbol* sym = scope_find(par.ctx.scp, tok.value);
        if (!sym)
            return false;
        
        return sym->flags & SYM_FLAG_FUNCTION;
    }
    
    //! The rules below are not prefixed with parser_rule_
    //!   for the sake of readability.
    //! The equivalent EBNF grammar is shown in the comments before each rule.
    //! Rule names are in lowercase letters, while tokens in UPPERCASE.
    
    static void type_specifier(parser&);
    static void argument_list(parser&);
    
    static void function_call_expr(parser&);
    static void expression(parser&);
    static void expression_list(parser&);
    
    static void declaration_stmt(parser&);
    static void assignment_stmt(parser&);
    static void function_call_stmt(parser&);
    static void statement(parser&);
    static void statement_block(parser&);
    
    static void function_decl(parser&);
    
    //! A type specifier.
    //!
    //! type_specifier := IDENTIFIER?type
    static void type_specifier(parser& par)
    {
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        
        if (!parser_is_type_name(par, tok))
            parser_parse_error(tok, "\"" + tok.value + "\" does not name a type");
    }
    
    //! An argument list.
    //! 
    //! argument_list := LEFT_PAREN (type_specifier IDENTIFIER (type_specifier IDENTIFIER)*)? RIGHT_PAREN
    static void argument_list(parser& par)
    {
        parser_expect(par, TOKEN_LEFT_PAREN);
        
        while (lexer_peekt(par.lex) != TOKEN_RIGHT_PAREN)
        {
            // Get the argument's type
            type_specifier(par);
            
            // Get its name & check the declaration
            token tok = parser_expect(par, TOKEN_IDENTIFIER);
            parser_check_declaration(par, tok);
            
            // Add it to the current scope
            symbol sym;
            sym.name = tok.value;
            sym.flags = SYM_FLAG_VARIABLE;
            sym.info = tok.info;
            scope_add(par.ctx.scp, sym);
            
            // Eat comma, if needed
            if (lexer_peekt(par.lex) != TOKEN_RIGHT_PAREN)
                parser_expect(par, TOKEN_COMMA);
        }
        
        parser_expect(par, TOKEN_RIGHT_PAREN);
    }
    
    //! A function call expression.
    //!
    //! function_call_expr := IDENTIFIER?function expression_list
    static void function_call_expr(parser& par)
    {
        // Get the function name
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        
        if (!parser_is_function_name(par, tok))
            parser_parse_error(tok, "\"" + tok.value + "\" does not name a function");
        
        // Get the calling expression list
        expression_list(par);
    }
    
    //TODO: would be cool if not done the classic recursive descent ugly way...
    //      iterative implementations at http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
    //
    //! An expression.
    //!
    //! expression := 
    static void expression(parser&)
    {}
    
    //! An expression list.
    //!
    //! expression_list := LEFT_PAREN (expression (COMMA expression)*)? RIGHT_PAREN
    static void expression_list(parser& par)
    {
        parser_expect(par, TOKEN_LEFT_PAREN);
        
        while (lexer_peekt(par.lex) != TOKEN_RIGHT_PAREN)
        {
            // Read in the expression
            expression(par);
            
            // Eat the comma, if needed
            if (lexer_peekt(par.lex) != TOKEN_RIGHT_PAREN)
                parser_expect(par, TOKEN_COMMA);
        }
        
        parser_expect(par, TOKEN_RIGHT_PAREN);
    }
    
    //! A variable declaration statement.
    //!
    //! declaration_stmt := type_specifier IDENTIFIER (EQUALS expression)? SEMICOLON
    static void declaration_stmt(parser& par)
    {
        // Type of the variable
        type_specifier(par);
        
        // Get its name and check for multiple declarations
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        parser_check_declaration(par, tok);
        
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
            expression(par);
        }
        
        parser_expect(par, TOKEN_SEMICOLON);
    }
    
    //! An assignment statement.
    //!
    //! assignment_stmt := IDENTIFIER?variable EQUALS expression SEMICOLON
    static void assignment_stmt(parser& par)
    {
        // Get the variable's name
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        
        if (!parser_is_variable_name(par, tok))
            parser_parse_error(tok, "\"" + tok.value + "\" does not name a variable");
        
        // Parse the equal sign
        parser_expect(par, TOKEN_EQUALS);
        
        // Get the expression
        expression(par);
        
        parser_expect(par, TOKEN_SEMICOLON);
    }
    
    //! Function call statement.
    //!
    //! function_call_stmt := function_call_expr SEMICOLON
    static void function_call_stmt(parser& par)
    {
        function_call_expr(par);
        parser_expect(par, TOKEN_SEMICOLON);
    }
    
    //! A single statement, terminated with a semicolon.
    //!
    //! statement := declaration_stmt
    //!            | assignment_stmt
    //!            | function_call_stmt
    static void statement(parser& par)
    {
        token tok = lexer_peek(par.lex);
        
        if (tok.type != TOKEN_IDENTIFIER)
            parser_parse_error(tok, "unexpected token");
        
        if (parser_is_type_name(par, tok))
            declaration_stmt(par);
        else if (parser_is_variable_name(par, tok))
            assignment_stmt(par);
        else if (parser_is_function_name(par, tok))
            function_call_stmt(par);
        else
            parser_parse_error(tok, "use of undeclared identifier `" + tok.value + "'");
    }
    
    //! A statement block, enclosed in curly braces.
    //!
    //! statement_block := LEFT_CURLY statement* RIGHT_CURLY
    static void statement_block(parser& par)
    {
        parser_expect(par, TOKEN_LEFT_CURLY);
        
        while (lexer_peekt(par.lex) != TOKEN_RIGHT_CURLY)
        {
            statement(par);
        }
        
        parser_expect(par, TOKEN_RIGHT_CURLY);
    }
    
    //! A function declaration.
    //!
    //! function_decl := type_specifier IDENTIFIER
    //!                  LEFT_PAREN argument_list RIGHT_PAREN
    //!                  statement_block
    static void function_decl(parser& par)
    {
        // Return type
        type_specifier(par);
        
        // Get its name and check for multiple definitions
        token tok = parser_expect(par, TOKEN_IDENTIFIER);
        parser_check_declaration(par, tok);
        
        // Add the function to the current scope
        symbol sym;
        sym.name = tok.value;
        sym.flags = SYM_FLAG_FUNCTION;
        sym.info = tok.info;
        scope_add(par.ctx.scp, sym);
        
        // Push a new scope
        scope_push(par.ctx.scp);
        
        // Arguments specification
        argument_list(par);
        
        // Function body
        statement_block(par);
        
        // Pop the function scope
        //FIXME: save the layer in the function representation in the AST
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
        function_decl(par);
    }
}
