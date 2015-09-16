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

#include "nut/pr_context.h"

namespace pr
{
    /**************************************/
    /*** Private implementation section ***/
    /**************************************/
    
    //! Expose builtin symbols to the context's scope.
    //! This consists mainly in builtin scalar types.
    static void context_expose_builtins(context& ctx)
    {
        symbol sym;
        sym.flags = SYM_FLAG_TYPE | SYM_FLAG_BUILTIN;
        
        #define DECL_BUILTIN_TYPE(nm, flags) sym.name = #nm; scope_add(ctx.scp, sym);
        #include "nut/sem_builtins.inc"
        #undef DECL_BUILTIN_TYPE
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    context context_create()
    {
        context ctx;
        ctx.scp = scope_create();
        
        context_expose_builtins(ctx);
        
        return ctx;
    }
    
    void context_free(context& ctx)
    {
        scope_free(ctx.scp);
    }
}
