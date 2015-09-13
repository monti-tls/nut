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

#include "nut/sem_operator.h"
#include "nut/pr_token.h"

namespace sem
{
    /**************************************/
    /*** Private implementation section ***/
    /**************************************/
    
    #define DECL_OP_BINARY(name, associativity, precedence, token) \
        { OP_BIN_ ## name, OP_ASSOC_ ## associativity, precedence, pr::token },
    #define DECL_OP_UNARY(name, precedence, token) \
        { OP_UN_ ## name, OP_ASSOC_LEFT, precedence, pr::token },
    
    static op ops[] =
    {
        #include "nut/sem_operators.inc"
    };
    
    static int ops_size = sizeof(ops) / sizeof(op);
    
    #undef DECL_OP_UNARY
    #undef DECL_OP_BINARY
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    op* op_find_by_token(int token)
    {
        for (int i = 0; i < ops_size; ++i)
            if (ops[i].token == token)
                return ops + i;
        
        return 0;
    }
}
