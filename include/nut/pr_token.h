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

#ifndef NUT_PR_TOKEN_H
#define NUT_PR_TOKEN_H

#include <string>
#include <iostream>

//!
//! pr_token
//!

//! This file defines the lexed tokens structure.
//! All tokens are defined in pr_tokens.inc, using the DECL_TOKEN* macros,
//!   this file is then included by this header, pr_tokens.cpp for pretty-print
//!   name mapping, and by the lexer.

namespace pr
{
    #define DECL_TOKEN(name)             TOKEN_ ## name,
    #define DECL_TOKEN_CHAR(name, char)  DECL_TOKEN(name)
    #define DECL_TOKEN_OP(name, str)     DECL_TOKEN(name)
    #define DECL_TOKEN_KW(name, str)     DECL_TOKEN(name)
    
    enum
    {
        #include "nut/pr_tokens.inc"
    };
    
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    #undef DECL_TOKEN
    
    //! Information about a token's location in the input stream.
    struct token_info
    {
        int line, column;
    };
    
    //! An (eventually) valued token.
    struct token
    {
        int type;
        std::string value;
        token_info info;
    };
    
    //! Print out a token (w/ its value, if any) to an output stream
    //!   in the human-readable format TYPE(='value')?
    void token_pretty_print(token const& tok, std::ostream& os = std::cout);
}

#endif // NUT_PR_TOKEN_H
