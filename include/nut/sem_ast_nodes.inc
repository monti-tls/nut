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
//! sem_ast_nodes.inc
//!

//! This file defines the abstract syntax tree nodes.
//! It is included by :
//!   - sem_ast_node.h:   to declare the node's structures and tag enumeration constants.
//!   - sem_ast_node.cpp: to declare pretty-print node names.
//! Each file above defines the DECL_NODE macro for the appropriate behavior.
//! The syntax is DECL_NODE(tag_name, struct_name, members), where
//!   the "members" field stands for the AST node's struct members.
//!
//! In the descriptions below, [i] stands for the i-th children pointer.
 
//! A simple type specifier node.
//!
//! name: name of the type
DECL_NODE(TYPE_SPECIFIER, type_specifier,
          std::string name;)

//! An argument node.
//!
//! name: name of the argument symbol
//! [0]: TYPE_SPECIFIER
DECL_NODE(ARGUMENT, argument,
          std::string name;)
          
//! An argument list node.
//!
//! [i] -> ARGUMENT
DECL_NODE(ARGUMENT_LIST, argument_list,)

//! A function call expression.
//!
//! name: name of the function symbol
//! [0] -> EXPRESSION_LIST
DECL_NODE(FUNCTION_CALL_EXPR, function_call_expr,
          std::string name;)

DECL_NODE(EXPRESSION, expression,)

//! A list of expressions.
//!
//! [i] -> EXPRESSION
DECL_NODE(EXPRESSION_LIST, expression_list,)
          
//! A variable declaration statement.
//!
//! name: name of the variable symbol.
//! [0] -> EXPRESSION or 0 (initializer)
DECL_NODE(DECLARATION_STMT, declaration_stmt,
          std::string name;)

//! A variable assignment statement.
//!
//! name: name of the variable symbol.
//! [0] -> EXPRESSION
DECL_NODE(ASSIGNMENT_STMT, assignment_stmt,
          std::string name;)

//! A function call statement.
//!
//! [0] -> FUNCTION_CALL_EXPR
DECL_NODE(FUNCTION_CALL_STMT, function_call_stmt,)

//! A generic statement.
//!
//! [0] -> *_STMT
DECL_NODE(STATEMENT, statement,)

//! A statement block.
//!
//! [i] -> STATEMENT
DECL_NODE(STATEMENT_BLOCK, statement_block,)

//! A function declaration.
//!
//! name: name of the declared function symbol.
//! [0] -> TYPE_SPECIFIER (return type)
//! [1] -> ARGUMENT_LIST
//! [2] -> STATEMENT_BLOCK
DECL_NODE(FUNCTION_DECL, function_decl,
          std::string name;)
