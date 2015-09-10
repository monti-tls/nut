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

#include "nut/pr_lexer.h"
#include <string>
#include <iostream>
#include <sstream>

int main()
{
    using namespace pr;
    
    std::string str =
    ",(,} if else;\n"
    "   # ffdsg;dsg;; if\n"
    "for do < <= / * */";
    
    std::istringstream ss;
    ss.str(str);
    
    lexer lex = lexer_create(ss);
    
    for (;;)
    {
        token tok = lexer_get(lex);
        token_pretty_print(tok, std::cout);
        std::cout << std::endl;
        
        if (tok.type == TOKEN_BAD || tok.type == TOKEN_EOF)
            break;
    }
    
    lexer_free(lex);
    
    return 0;
}
