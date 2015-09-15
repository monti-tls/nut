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

#ifndef NUT_PR_SYMBOL_H
#define NUT_PR_SYMBOL_H

#include "nut/pr_token.h"
#include <string>

//!
//! pr_symbol
//!

//! This header defines a simple symbol structure, w/
//!   name and flags.

namespace pr
{
    //! The symbol flags enumeration.
    //!
    //! TYPE:     the symbol is a type name
    //! VARIABLE: the symbol is a variable name
    //! FUNCTION: the symbol is a function name
    //! BUILTIN:  the symbol is a built-in language feature
    enum
    {
        SYM_FLAG_TYPE     = 0x0001,
        SYM_FLAG_VARIABLE = 0x0002,
        SYM_FLAG_FUNCTION = 0x0004,
        
        SYM_FLAG_BUILTIN  = 0x8000
    };
    
    //! The symbol structure.
    struct symbol
    {
        unsigned int flags;
        std::string name;
        token_info info;
    };
}

#endif // NUT_PR_SYMBOL_H
