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
//! sem_builtins.inc
//!

//! This file defines the language's built-in symbols.
//! It is included by:
//!   - pr_context.cpp:  to init the parsing scope.
//!   - sem_passman.cpp: to generate the built-in symbol declarators.
//!
//! The DECL_BUILTIN_TYPE(name, flags) is used to declare
//!   a built-in type.
//! The given name will be the one used in the language grammar,
//!   the flags are purely semantics and are defined in sem_declarators.h.

    
//! A helper macro for shorted flag names.
#define F(flag) TYPE_FLAG_ ## flag
    
DECL_BUILTIN_TYPE(void, F(NONCOPYABLE))
DECL_BUILTIN_TYPE(bool, F(NONE))
DECL_BUILTIN_TYPE(int,  F(NONE))
DECL_BUILTIN_TYPE(float,  F(NONE))

#undef F
