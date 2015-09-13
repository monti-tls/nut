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

//!
//! sem_operators.inc
//!

//! This file defines the valid operators in expressions.
//! It specifies the name, associativity and precedence fo the operator,
//!   as well as the token that represents it.
//! It is used by :
//!   - sem_tokens.h:   to define the enumeration constants
//!   - sem_tokens.cpp: to define the operator structure instances

DECL_OP_BINARY(MUL,
               LEFT, 3,
               TOKEN_STAR)

DECL_OP_BINARY(DIV,
               LEFT, 3,
               TOKEN_SLASH)

DECL_OP_BINARY(MOD,
               LEFT, 3,
               TOKEN_PERCENT)

DECL_OP_BINARY(ADD,
               LEFT, 4,
               TOKEN_PLUS)

DECL_OP_BINARY(SUB,
               LEFT, 4,
               TOKEN_MINUS)

DECL_OP_BINARY(LESS,
               LEFT, 6,
               TOKEN_LESS)

DECL_OP_BINARY(LESS_EQUAL,
               LEFT, 6,
               TOKEN_LESS_EQUAL)

DECL_OP_BINARY(GREATER,
               LEFT, 6,
               TOKEN_GREATER)

DECL_OP_BINARY(GREATER_EQUAL,
               LEFT, 6,
               TOKEN_GREATER_EQUAL)

DECL_OP_BINARY(EQUAL,
               LEFT, 7,
               TOKEN_EQUAL)

DECL_OP_BINARY(NOT_EQUAL,
               LEFT, 7,
               TOKEN_NOT_EQUAL)

