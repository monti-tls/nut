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

#ifndef NUT_PR_PRATT_H
#define NUT_PR_PRATT_H

#include "nut/pr_parser.h"
#include "nut/pr_token.h"
#include "nut/pr_ast.h"

//!
//! pr_pratt
//!

//! The Pratt (aka top-down precedence parser) parser for Nut.
//! It builds an AST subtree from an expression.
//! Internally, all expression 'elements' (standing for operators and operands)
//!   is represented by an inherited expr_element structure.
//! Each expr_element is mapped to a single token, and defines two handlers :
//!   nud() -> node:     used for literals and unary operators
//!   led(left) -> node: used for binary operators
//!
//! Most operators are defined in pr_pratt_elements.inc, using macros (like tokens).
//! Because some operands (literals, ...) needs special care, they are hard-coded in the parser
//!   and therefore not defined in the latter file.

namespace pr
{
    //! Parse an expression.
    ast_node* pratt_expression(parser&, int rbp = 0);
}

#endif // NUT_PR_PRATT_H
