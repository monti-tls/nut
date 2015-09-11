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

#ifndef NUT_PR_SCOPE_H
#define NUT_PR_SCOPE_H

#include "nut/pr_symbol.h"
#include <string>
#include <vector>
#include <stack>

namespace pr
{
    struct scope_layer
    {
        int depth;
        std::vector<symbol> symbols;
    };
    
    //! Find a symbol in a scope layer by name.
    //! This does not check for duplicates !
    //! Returns 0 if not found.
    symbol* scope_layer_find(scope_layer& lyr, std::string const& name);
    
    //! Add a new symbol in a scope layer.
    //! This does not check for duplicate symbols !
    void scope_layer_add(scope_layer& lyr, symbol const& sym);
    
    struct scope
    {
        unsigned int top;
        std::vector<scope_layer> layers;
    };
    
    //! Create a new scope.
    scope scope_create();
    
    //! Delete a scope.
    void scope_free(scope& scp);
    
    //! Enter a new layer down in the scope.
    //! Returns the newly created layer.
    scope_layer& scope_push(scope& scp);
    
    //! Exit the current scope layer (returning it).
    scope_layer scope_pop(scope& scp);
    
    //! Find a symbol in the current scope.
    //! This searches recursively in all layers (from the innermost),
    //!   and returns the first symbol that matches.
    //! This does not check for duplicates in the same layer.
    //! Returns 0 if not found anywhere.
    symbol* scope_find(scope& scp, std::string const& name);
    
    //! Find a symbol in the innermost layer only.
    symbol* scope_find_innermost(scope& scp, std::string const& name);
    
    //! Add a new symbol to the current scope layer.
    //! This does not check for duplicates.
    void scope_add(scope& scp, symbol const& sym);
}

#endif // NUT_PR_SCOPE_H
