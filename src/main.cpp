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
#include "nut/pr_ast.h"
#include "nut/sem_passman.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <map>

int main()
{
    using namespace pr;
    using namespace sem;
    
    try
    {
        std::ifstream fs("scratch/test.nut");
        
        context ctx = context_create();
        lexer lex = lexer_create(fs, ctx);
        parser par = parser_create(lex, ctx);
        passman pman = passman_create(par);
        
        ast_node* ast = parser_parse_program(par);
        
        passman_run_all(pman, ast);
        
        ast_pretty_print(ast);
        
        ast_free(ast);
        
        passman_free(pman);
        parser_free(par);
        lexer_free(lex);
        context_free(ctx);
    }
    catch (std::exception const& exc)
    {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    
    return 0;
}
