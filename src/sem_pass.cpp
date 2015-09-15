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

#include "nut/sem_pass.h"
#include "nut/pr_parser.h" // for error handling
#include <sstream>
#include <stdexcept>

namespace sem
{
    using namespace pr;
    
    /*************************************/
    /*** Private module implementation ***/
    /*************************************/
    
    static void sem_error(ast_node* node, std::string const& msg)
    {
        //FIXME define the "token location-to-string" method in the parser
        //      and use it from here.
        
        std::ostringstream ss;
        ss << "semantic error: line " << node->saved_tok.info.line;
        ss << ", col " << node->saved_tok.info.column << ": " << msg;
        throw std::logic_error(ss.str());
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/

    void call_check_pass(ast_node* node)
    {
        if (!node)
            return;
        
        if (node->tag == FUNCTION_CALL_EXPR)
        {
            ast_node* function = node->children[0];
            if (function->tag != IDENTIFIER_EXPR)
                sem_error(node, "function calls are only supported on identifiers");
        }
        else
        {
            for (unsigned int i = 0; i < node->children.size(); ++i)
                call_check_pass(node->children[i]);
        }
    }
}
