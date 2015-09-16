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

#include "nut/sem_declarator.h"

namespace sem
{
    /**************************************/
    /*** Private implementation section ***/
    /**************************************/
    
    declarator::declarator()
    {
        self = this;
    }
    
    declarator::~declarator()
    { }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    type* type_create(std::string const& name, int flags)
    {
        type* tp = new type();
        tp->tag = TYPE_DECLARATOR;
        tp->name = name;
        tp->flags = flags;
        return tp;
    }
    
    variable* variable_create(std::string const& name)
    {
        variable* var = new variable();
        var->tag = VARIABLE_DECLARATOR;
        var->name = name;
        var->tp = 0;
        return var;
    }
    
    function* function_create(std::string const& name)
    {
        function* fun = new function();
        fun->tag = FUNCTION_DECLARATOR;
        fun->name = name;
        fun->ret_tp = 0;
        return fun;
    }
    
    void declarator_free(declarator* decl)
    {
        switch (decl->tag)
        {
            case VARIABLE_DECLARATOR:
                break;
                
            case FUNCTION_DECLARATOR:
            {
                for (unsigned int i = 0; i < decl->as_function->arguments.size(); ++i)
                    declarator_free(decl->as_function->arguments[i]);
                break;
            }
        }
        
        delete decl;
    }
}
