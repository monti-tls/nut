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

#include "nut/pr_scope.h"
#include <stdexcept>

namespace pr
{
    symbol* scope_layer_find(scope_layer& lyr, std::string const& name)
    {
        for (unsigned int i = 0; i < lyr.symbols.size(); ++i)
            if (lyr.symbols[i].name == name)
                return &lyr.symbols[i];
        
        return 0;
    }
    
    void scope_layer_add(scope_layer& lyr, symbol const& sym)
    {
        lyr.symbols.push_back(sym);
    }
    
    scope scope_create()
    {
        scope_layer lyr;
        lyr.depth = 0;
        
        scope scp;
        scp.layers.push_back(lyr);
        scp.top = 0;
        
        return scp;
    }
    
    void scope_free(scope&)
    {}
    
    scope_layer& scope_push(scope& scp)
    {
        scope_layer lyr;
        lyr.depth = scp.layers[scp.top].depth + 1;
        
        scp.layers.push_back(lyr);
        return scp.layers[++scp.top];
    }
    
    scope_layer scope_pop(scope& scp)
    {
        if (scp.top == 0)
            throw std::logic_error("pr::scope_pop: can't pop root scope layer !");
        
        scope_layer lyr = scp.layers.back();
        scp.layers.pop_back();
        --scp.top;
        
        return lyr;
    }
    
    symbol* scope_find(scope& scp, std::string const& name)
    {
        for (int i = scp.top; i >= 0; --i)
        {
            symbol* sym = scope_layer_find(scp.layers[i], name);
            if (sym)
                return sym;
        }
        
        return 0;
    }
    
    symbol* scope_find_innermost(scope& scp, std::string const& name)
    {
        return scope_layer_find(scp.layers[scp.top], name);
    }
    
    void scope_add(scope& scp, symbol const& sym)
    {
        scope_layer_add(scp.layers[scp.top], sym);
    }
}
