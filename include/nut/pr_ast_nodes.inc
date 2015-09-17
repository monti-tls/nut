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
//! pr_ast_nodes.inc
//!

//! This file defines the abstract syntax tree nodes.
//! It is included by :
//!   - pr_ast_node.h:   to declare the node's structures and tag enumeration constants.
//!   - pr_ast_node.cpp: to declare pretty-print node names.
//! Each file above defines the DECL_NODE macro for the appropriate behavior.
//! The syntax is DECL_NODE(tag_name, struct_name, members), where
//!   the "members" field stands for the AST node's struct members.
//!
//! In the descriptions below, [i] stands for the i-th children pointer.
  
////////////////////////////////////////////////////////////////
//////////////// Helper nodes //////////////////////////////////
////////////////////////////////////////////////////////////////
 
//! A simple type specifier node.
//!
//! name: name of the type
DECL_NODE(TYPE_SPECIFIER, type_specifier,
          std::string name;)

//! An argument declaration node.
//!
//! name: name of the argument symbol
//! [0]: TYPE_SPECIFIER
DECL_NODE(ARGUMENT, argument,
          std::string name;)
          
//! An argument declaration list node.
//!
//! [i] -> ARGUMENT
DECL_NODE(ARGUMENT_LIST, argument_list,)

////////////////////////////////////////////////////////////////
//////////////// Literals //////////////////////////////////////
////////////////////////////////////////////////////////////////

//! An integer literal.
//!
//! value: the literal value.
DECL_NODE(INTEGER_LITERAL_EXPR, integer_literal_expr,
          int value;)

//! An identifier expression literal.
//!
//! name: the name of the symbol.
DECL_NODE(IDENTIFIER_EXPR, identifier_expr,
          std::string name;)

////////////////////////////////////////////////////////////////
//////////////// Arithmetic expressions ////////////////////////
////////////////////////////////////////////////////////////////

//! Prefix increment and decrement.
DECL_NODE(INC_EXPR, inc_expr,)
DECL_NODE(DEC_EXPR, dec_expr,)
//! Negation.
DECL_NODE(NEG_EXPR, neg_expr,)
//! Boolean negation.
DECL_NODE(NOT_EXPR, not_expr,)
//! Addition, subtraction, multiplication and division.
DECL_NODE(ADD_EXPR, add_expr,)
DECL_NODE(SUB_EXPR, sub_expr,)
DECL_NODE(MUL_EXPR, mul_expr,)
DECL_NODE(DIV_EXPR, div_expr,)

////////////////////////////////////////////////////////////////
/////////////// Expressions w/ side effects ////////////////////
////////////////////////////////////////////////////////////////

//! An assignment expression.
//!
//! [0] -> lhs l-value (writable)
//! [1] -> rhs r-value (readable)
DECL_NODE(ASSIGNMENT_EXPR, assignment_expr,)

//! A function call expression.
//!
//! [0] -> *_EXPR (function symbol)
//! [1] -> LIST_EXPR  (argument values)
DECL_NODE(FUNCTION_CALL_EXPR, function_call_expr,)

//! A member access expression.
//!
//! [0] -> *_EXPR (struct symbol)
//! [1] -> *_EXPR (member symbol)
DECL_NODE(MEMBER_ACCESS_EXPR, member_access_expr,)

//! An expression list node, in a linked-list fashion.
//!
//! [0] -> LIST_EXPR or *_EXPR
//! [1] -> *_EXPR
DECL_NODE(LIST_EXPR, list_expr,)

//! A node to wrap expressions.
//! It is only used to easily distinguish between
//!   statements and expressions.
//!
//! [0] -> *_EXPR
DECL_NODE(EXPRESSION, expression,)

////////////////////////////////////////////////////////////////
//////////////// Statement nodes ///////////////////////////////
////////////////////////////////////////////////////////////////
          
//! A variable declaration statement.
//!
//! name: name of the variable symbol.
//! [0] -> TYPE_SPECIFIER
//! [1] -> EXPRESSION or 0 (initializer)
DECL_NODE(DECLARATION_STMT, declaration_stmt,
          std::string name;)

//! A return statement.
//!
//! [0] -> EXPRESSION or 0
DECL_NODE(RETURN_STMT, return_stmt,)
          
//! A generic statement.
//!
//! [0] -> EXPRESSION or *_STMT
DECL_NODE(STATEMENT, statement,)

//! A statement block.
//!
//! [i] -> STATEMENT
DECL_NODE(STATEMENT_BLOCK, statement_block,)

////////////////////////////////////////////////////////////////
//////////////// Top-level declarators /////////////////////////
////////////////////////////////////////////////////////////////

//! A function declaration.
//!
//! name: name of the declared function symbol.
//! [0] -> TYPE_SPECIFIER (return type)
//! [1] -> ARGUMENT_LIST
//! [2] -> STATEMENT_BLOCK
DECL_NODE(FUNCTION_DECL, function_decl,
          std::string name;)

//! A program declaration.
//!
//! [i] -> FUNCTION_DECL
DECL_NODE(PROGRAM_DECL, program_decl,)
