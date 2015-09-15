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
//! pr_pratt_elements.inc
//!

//! This file defines the Pratt parser's expression elements.
//! This does not contains all the expression elements, as
//!   literals are hard-coded in the parser (they need special care).
//!
//! This is used by pratt_parser.cpp to define :
//!   - the (hidden) element structures
//!   - the token to element switch cases

//! Here is the operator table (inspired from C operators) :
//! Left binding power is proportionally inverse to precedence.
//!
//! +------------+--------+----------+---------------+
//! | Precedence | LBP    | Operator | Associativity |
//! +------------+--------+----------+---------------+
//! | 1          | 100    |  x()     | left          | *
//! |            |        |  x.y     | left          | *
//! +------------+--------+----------+---------------+
//! |            |        |  ++x --x | N/A           | * *
//! | 2          | 90     |  -x +x   | N/A           | * *
//! |            |        |  !x ~x   | N/A           | ? ?
//! +------------+--------+----------+---------------+
//! | 3          | 80     |  * / %   | left          | * * ?
//! +------------+--------+----------+---------------+
//! | 4          | 70     |  + -     | left          | * *
//! +------------+--------+----------+---------------+
//! | 6          | 60     | < <=     | left          | ? ?
//! |            |        | > >=     | left          | ? ?
//! +------------+--------+----------+---------------+
//! | 7          | 50     | == !=    | left          | ? ?
//! +------------+--------+----------+---------------+
//! | 14         | 10     | =        | right         | *
//! +------------+--------+----------+---------------+
//! | 15         | 5      | ,        | left          | *
//! +------------+--------+----------+---------------+

//! Elements declarations starts with ELEMENT_BEGIN(token, binary_lbp)
//!   where the element maps to the given token.
//!   For binary operators,, binary_lbp is their binding power.
//! They are ended w/ ELEMENT_END().
//!
//! The same token can map to either an unary operator, a binary operator or both.
//! Use ELEMENT_UNARY*() to declare an unary operator mapping,
//!   and ELEMENT_BINARY*() to declare a binary operator mapping.
//!
//! ELEMENT_UNARY(node_type, unary_lbp)
//!   declares a unary operator w/ binding power "unary_lbp" that creates a node node_type.
//!
//! ELEMENT_UNARY_SHELL(unary_lbp)
//!   same as above, but does not create a new node, just return the sub-expression node.
//!
//! ELEMENT_UNARY_SHELL_CONSUME(unary_lbp, token)
//!   same as above, but consumes the given token after reading sub-expression. Useful for matched tokens (-), [-]
//!
//! ELEMENT_BINARY(node_type, associativity)
//!   declared a binary operator (lbp is found in ELEMENT_BEGIN), with the given associativity.
//!   associativity must be LEFT, RIGHT or CUSTOM(lbp) (in uppercase).
//!   a node w/ type node_type is created with the two sub-expressions.
//!
//! ELEMENT_BINARY_CONSUME(node_type, associativity, token)
//!   same as above, but consumes the given token after reading sub-expressions.

//! Grouping and function call.
//!   (x) -> RBP = 0
//!   x() -> LBP = 100, RBP = 0
ELEMENT_BEGIN              (TOKEN_LEFT_PAREN, 100)
ELEMENT_UNARY_SHELL_CONSUME(0, TOKEN_RIGHT_PAREN)
ELEMENT_BINARY_CONSUME     (function_call_expr_node, CUSTOM(0), TOKEN_RIGHT_PAREN)
ELEMENT_END                ()

//! Member access
//!   x.y -> LBP = RBP = 100
ELEMENT_BEGIN              (TOKEN_DOT, 100)
ELEMENT_BINARY             (member_access_expr_node, LEFT)
ELEMENT_END                ()

//! Incrementation
//!   ++x -> RBP = 90
ELEMENT_BEGIN              (TOKEN_PLUSPLUS, 90)
ELEMENT_UNARY              (inc_expr_node, 90)
ELEMENT_END                ()

//! Decrementation
//!   --x -> RBP = 90
ELEMENT_BEGIN              (TOKEN_MINUSMINUS, 90)
ELEMENT_UNARY              (dec_expr_node, 90)
ELEMENT_END                ()

//! Unary and binary plus
//!   +x  -> RBP = 90
//!   x+y -> LBP = RBP = 70
ELEMENT_BEGIN              (TOKEN_PLUS, 70)
ELEMENT_UNARY_SHELL        (90)
ELEMENT_BINARY             (add_expr_node, LEFT)
ELEMENT_END                ()

//! Unary and binary minus
//!   -x  -> RBP = 90
//!   x-y -> LBP = RBP = 70
ELEMENT_BEGIN              (TOKEN_MINUS, 70)
ELEMENT_UNARY              (neg_expr_node, 90)
ELEMENT_BINARY             (sub_expr_node, LEFT)
ELEMENT_END                ()

//! Multiply
//!   x*y -> LBP = RBP = 80
ELEMENT_BEGIN              (TOKEN_STAR, 80)
ELEMENT_BINARY             (mul_expr_node, LEFT)
ELEMENT_END                ()

//! Divide
//!   x/y -> LBP = RBP = 80
ELEMENT_BEGIN              (TOKEN_SLASH, 80)
ELEMENT_BINARY             (div_expr_node, LEFT)
ELEMENT_END                ()

//! Assignment
//!   x = y -> LBP = 10, RBP = 9 (right associativity)
ELEMENT_BEGIN              (TOKEN_EQUALS, 10)
ELEMENT_BINARY             (assignment_expr_node, RIGHT)
ELEMENT_END                ()

//! Comma
//!   x, y -> LBP = RBP = 5
ELEMENT_BEGIN              (TOKEN_COMMA, 5)
ELEMENT_BINARY             (list_expr_node, LEFT)
ELEMENT_END                ()
