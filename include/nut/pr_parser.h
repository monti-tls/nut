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

#ifndef NUT_PR_PARSER_H
#define NUT_PR_PARSER_H

#include "nut/pr_lexer.h"
#include "nut/pr_context.h"
#include "nut/pr_ast.h"

//!
//! pr_parser
//!

//! The Nut parser, that builds an AST out of a sequence of tokens from a lexer.
//! This file defines the parser bare bones, including error managing and generation,
//!   but does only implement statements and control-flow constructs.
//! Expressions are implemented by a separate Pratt (top-down precedence parser) parser,
//!   located in pr_pratt.h & .cpp.
//!
//! This parser keeps a scope in his context only
//!   to check some basic things. The scope is also needed to distinguish
//!   declarations and expression (as user may define type names).
//! It checks :
//!   - for multiple declarations of types, variables and functions
//!   - for use of undeclared identifiers (see in pr_pratt.cpp), regardless of their kind.
//! It does NOT check :
//!   - for invalid function calls, for example (1+3)(...) or invalid call arity, see pass_call_check.
//!   - for type errors, see pass_type_check.
//!
//! It also does NOT build a complete scope for the program, and local scopes are
//!   destroyed during parsing. The complete scope tree is created by pass_create_declarators.
//!
//! No semantic information is extracted during parsing, and all checking are done in later
//!   passes (see sem_pass.h).

namespace pr
{
    //! The parser structure, holding a lexer and a context reference.
    struct parser
    {
        parser(lexer& lex, context& ctx) : lex(lex), ctx(ctx) {};
        
        lexer& lex;
        context& ctx;
    };
    
    //! Create a parser entity based on a lexer and attached to a context.
    parser parser_create(lexer& lex, context& ctx);
    
    //! Delete a parser.
    void parser_free(parser& par);
    
    //! Parse a program module.
    ast_node* parser_parse_program(parser& par);
    
    //!
    //! The functions below are not meant to be used by a regular user,
    //!   but by other parsing modules like pr_pratt.cpp.
    //!
    
    //! Throw out an error, printing the message msg and location of the token tok.
    void parser_parse_error(parser& par, token const& tok, std::string const& msg);
    
    //! Get an information string about the token location, in the format :
    //! "line %line, col %column: "
    std::string parser_token_information(token const& tok);
    
    //! Generate an line error information from the position in tok.
    //! Format (abab... is the corresponding line of the input stream) :
    //! abababababababababab
    //! ~~~~~~~~~~~~~^
    //! The caret is at tok.info.column.
    std::string parser_error_line(parser& par, token const& tok);
    
    //! Check that the next token is of the given type.
    //! Throw an error if it is not the case.
    //! If err_msg is empty, it outputs the default error message (automatic), otherwise
    //!   it outputs err_msg.
    //! If eat == true, consume the token and return it.
    //! If eat == false, the return value is uninitialized.
    token parser_expect(parser& par, int type, std::string const& err_msg = "", bool eat = true);
    //! Check an identifier token against multiple declarations.
    void parser_check_declaration(parser& par, token const& tok);
    //! Check if an identifier token is a type name.
    bool parser_is_type_name(parser& par, token const& tok);
    //! Check if an identifier token is a variable name.
    bool parser_is_variable_name(parser& par, token const& tok);
    //! Check if an identifier token is a function name.
    bool parser_is_function_name(parser& par, token const& tok);
}

#endif // NUT_PR_PARSER_H
