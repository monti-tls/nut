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

#ifndef NUT_PR_CONTEXT_H
#define NUT_PR_CONTEXT_H

#include "nut/pr_scope.h"

//!
//! pr_context
//!

//! This file defines the parsing context.
//! It currently hold a single stack scope object.

namespace pr
{
    //! The parsing context structure.
    struct context
    {
        scope scp;
    };
    
    //! Create an empty parsing context.
    context context_create();
    
    //! Free a parsing context.
    void context_free(context& ctx);
}

#endif // NUT_PR_CONTEXT_H
