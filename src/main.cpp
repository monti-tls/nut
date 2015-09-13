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
#include "nut/pr_context.h"
#include "nut/pr_parser.h"
#include "nut/sem_ast_node.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

int main()
{
    using namespace pr;
    
    try
    {
        std::ifstream fs("scratch/test.nut");
    
        context ctx = context_create();
        lexer lex = lexer_create(fs, ctx);
        parser par = parser_create(lex, ctx);
        
        /*for (;;)
        {
            token tok = lexer_get(lex);
            token_pretty_print(tok, std::cout);
            std::cout << std::endl;
            
            if (tok.type == TOKEN_BAD || tok.type == TOKEN_EOF)
                break;
        }*/
        
        sem::ast_node* ast = parser_parse_program(par);
        
        parser_free(par);
        lexer_free(lex);
        context_free(ctx);
        
        sem::ast_pretty_print(ast);
        
        sem::ast_free(ast);
    }
    catch (std::exception const& exc)
    {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    
    return 0;
}
