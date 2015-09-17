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

#ifndef SEM_IR_H
#define SEM_IR_H

//!
//! sem_ir
//!

//! This modules defines the Intermediate Representation used by this compiler.
//! It is generated from the AST, and translated to target assembly in the
//!   generation process.
//!
//! This IR is a medium-level one, that consists on a sequence of "pieces".
//! Each piece is either a "label" or an "operation".
//! An operation represents some work on "targets", and can be a high-level
//!   operation like pushing a compound structure to the stack.
//! Therefore, at the generation time, an operation may translate
//!   to several instructions.
//! Some efforts are done to be as target independant as possible,
//!   therefore the calling ABI is here abstracted through the OP_CALL, OP_POP_RET and
//!   OP_PUSH_RET operations.

namespace sem
{
    //! Tag for the piece structure.
    enum
    {
        PIECE_LABEL,
        PIECE_OPERATION
    };
    
    //! An IR piece, that is either an operation (mapped
    //!   at generation time to an instruction)
    //!   or a label.
    struct piece
    {
        piece();
        virtual ~piece();
        
        //! Tag from PIECE_* enumeration constants.
        int tag;
        
        //! Automatic cast pointers.
        union
        {
            piece* self;
            label* as_label;
            operation* as_operation;
        };
    };
    
    //! A label in the IR code.
    //! They are dynamically allocated on request.
    struct label : public piece
    {
        int id;
    };
    
    //! Forward declaration.
    struct target;
    
    //! Tag for the operation structure.
    //!
    //! PUSH:     push a target onto the stack (may be compound)
    //! POP:      pop a target from the stack (")
    //! POP_RET:  pop a value from stack and set it to be returned by the current function
    //!           TODO: how to determine the return value nature ???
    //! PUSH_RET: push the return value of the last called function to the stack
    //!           TODO: how to determine the returned value's nature ??
    //! CALL:     call the given function with the given arguments
    enum
    {
        OP_PUSH,
        OP_POP,
        
        OP_POP_RET,
        OP_PUSH_RET,
        OP_CALL,
        
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV
    };
    
    //! An IR operation, that will be mapped later to some
    //!   real instruction(s) at the generation process.
    struct operation : public piece
    {
        //! Tag from OP_* enumeration constants.
        int tag;
        
        //! Targets of this instruction (mapped later to operands).
        std::vector<target*> targets;
    };
    
    //! Tag for the operation's target structure.
    enum
    {
        TG_CONSTANT,
        TG_LABEL,
        TG_OBJECT
    };
    
    struct target
    {
        //! Tag from TG_* enumeration constants.
        int tag;
    };
}

#endif // SEM_IR_H
