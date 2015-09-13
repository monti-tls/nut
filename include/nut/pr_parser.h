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
#include "nut/sem_ast_node.h"

namespace pr
{
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
    sem::ast_node* parser_parse_program(parser& par);
}

#endif // NUT_PR_PARSER_H
