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

#ifndef NUT_SEM_PASSMAN_H
#define NUT_SEM_PASSMAN_H

#include "nut/pr_ast.h"
#include "nut/pr_parser.h"

//!
//! sem_passman
//!

//! This module defines the AST pass manager, the semantic analyzer's core.
//! A lot of work is done in fairly simple passes that are also defined below.

namespace sem
{
    //! The pass manager structure.
    //! It holds a parser structure to generate useful errors
    //!   (it needs to read some lines upon generating messages).
    struct passman
    {
        passman(pr::parser&);
        
        pr::parser& par;
    };
    
    //! Below are the semantic analyzer passes.
    //! They are presented in order of execution.
    //! Each pass assumes that the preceding ones have been ran,
    //!   without sanity checks, so please be careful !
    
    //! Create a pass manager object.
    passman passman_create(pr::parser& par);
    
    //! Free a pass manager object.
    void passman_free(passman& pman);
    
    //! Fix the AST parent, prev and next pointers.
    void pass_fix_ast(passman& pman, pr::ast_node* node);
    
    //! Create declarators in the AST for types, variables and functions.
    void pass_create_declarators(passman& pman, pr::ast_node* node);
    
    //! Check function calls :
    //!   - check if called object is an identifier
    //!   - check if called object is a function
    //!   - check call arity
    void pass_check_calls(passman& pman, pr::ast_node* node);
    
    //! Generate the expression result type information.
    //! It is located in node.res_tp.
    //! This checks for :
    //!   - invalid use of identifers (for ex types and function names
    //!     are not allowed in arithmetic expressions)
    //!   - type incompatibilities in expressions (void = int, float + int, ...)
    void pass_resolve_result_types(passman& pman, pr::ast_node* node);
    
    //! Type-check pass.
    //! It checks :
    //!   - variables and arguments declared void (i.e. NONCOPYABLE)
    //!   - variables initialized w/ incompatible type
    //!   - function call arguments typing consistency
    void pass_type_check(passman& pman, pr::ast_node* node);
    
    //! This pass checks for unused expression results
    //!   and emit warnings.
    void pass_unused_expression_results(passman& pman, pr::ast_node* node);
}

#endif // NUT_SEM_PASSMAN_H
