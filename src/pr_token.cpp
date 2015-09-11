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

#include "nut/pr_token.h"

namespace pr
{
    struct named_token
    {
        int type;
        std::string name;
    };
    
    #define DECL_TOKEN(name)             { TOKEN_ ## name, #name },
    #define DECL_TOKEN_CHAR(name, char)  DECL_TOKEN(name)
    #define DECL_TOKEN_OP(name, str)     DECL_TOKEN(name)
    #define DECL_TOKEN_KW(name, str)     DECL_TOKEN(name)
    #define DECL_TOKEN_SCOPE(name, flag) DECL_TOKEN(name)
    
    static named_token named_tokens[] =
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int named_tokens_size = sizeof(named_tokens) / sizeof(named_token);
    
    #undef DECL_TOKEN_SCOPE
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    #undef DECL_TOKEN
    
    //! Find a named token by type (e.g. TOKEN_* constants).
    static named_token* find_named_token(int type)
    {
        for (int i = 0; i < named_tokens_size; ++i)
            if (named_tokens[i].type == type)
                return named_tokens + i;
        
        return 0;
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    void token_pretty_print(token const& tok, std::ostream& os)
    {
        named_token* ntk = find_named_token(tok.type);
        if (!ntk)
            os << "invalid !";
        else
        {
            os << ntk->name;
            if (tok.value.size())
                os << "=" << tok.value;
        }
    }
}
