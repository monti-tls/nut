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

#ifndef NUT_SEM_OPERATOR_H
#define NUT_SEM_OPERATOR_H

namespace sem
{
    //! Declare the enumeration constants OP_*_*
    #define DECL_OP_BINARY(name, associativity, precedence, token) OP_BIN_ ## name,
    #define DECL_OP_UNARY(name, precedence, token) OP_UN_ ## name,
    enum
    {
        #include "nut/sem_operators.inc"
    };
    #undef DECL_OP_UNARY
    #undef DECL_OP_BINARY
    
    //! Operator associativity enumeration.
    enum
    {
        OP_ASSOC_LEFT,
        OP_ASSOC_RIGHT
    };
    
    //! The operator structure.
    struct op
    {
        int type;
        int assoc;
        int precedence;
        int token;
    };
    
    //! Find an operator entry by token type.
    //! Returns 0 if not found.
    op* op_find_by_token(int token);
}

#endif // NUT_SEM_OPERATOR_H
