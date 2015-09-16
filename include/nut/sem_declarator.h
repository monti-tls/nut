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

#ifndef NUT_SEM_DECLARATOR_H
#define NUT_SEM_DECLARATOR_H

#include <string>
#include <vector>

//!
//! sem_declarator
//!

//! This module defines the declarator structures.
//! A declarator is an object attached to an AST node (when the node declares something)
//!   that contains semantic information about the latter declaration.

namespace sem
{
    //! Declarator tag enumeration.
    enum
    {
        TYPE_DECLARATOR,
        VARIABLE_DECLARATOR,
        FUNCTION_DECLARATOR
    };
    
    //! Forward declarations.
    struct type;
    struct variable;
    struct function;
    
    //! A semantic declarator, (similar to pr::symbol).
    struct declarator
    {
        declarator();
        virtual ~declarator();
        
        int tag;
        std::string name;
        
        union
        {
            declarator* self;
            type* as_type;
            variable* as_variable;
            function* as_function;
        };
    };
    
    //! Type flags.
    enum
    {
        TYPE_NONCOPYABLE = 0x0001
    };
    
    //! A type declarator.
    struct type : public declarator
    {
        int flags;
    };
    
    //! A variable declarator.
    struct variable : public declarator
    {
        type* tp;
    };
    
    //! A function declarator.
    struct function : public declarator
    {
        type* ret_tp;
        std::vector<variable*> arguments;
    };
    
    //! Create a new type declarator.
    type* type_create(std::string const& name);
    
    //! Create a new variable declarator.
    variable* variable_create(std::string const& name);
    
    //! Create a new function declarator.
    function* function_create(std::string const& name);
    
    //! Delete a declarator.
    void declarator_free(declarator* decl);
}

#endif // NUT_SEM_DECLARATOR_H
