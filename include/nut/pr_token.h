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

namespace pr
{
    #define DECL_TOKEN_CHAR(name, char)   TOKEN_ ## name,
    #define DECL_TOKEN_OP(name, str)      TOKEN_ ## name,
    #define DECL_TOKEN_KW(name, str)      TOKEN_ ## name,
    
    enum
    {
        TOKEN_EOF,
        TOKEN_BAD,
        
        #include "nut/pr_tokens.inc"
    };
    
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    
    struct token_info
    {
        int line, column;
    };
    
    struct token
    {
        int type;
        std::string value;
        token_info info;
    };
    
    void token_pretty_print(token const& tok, std::ostream& os = std::cout);
}

#endif // NUT_PR_TOKEN_H
