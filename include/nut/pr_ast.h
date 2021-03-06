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

#ifndef NUT_PR_AST_H
#define NUT_PR_AST_H

#include "nut/pr_token.h"
#include "nut/sem_declarator.h"
#include <string>
#include <iostream>
#include <vector>

//!
//! pr_ast
//!

//! This file defines the Abstract Syntax Tree representation of the source code.
//! Each node have a node.tag integer field that can take values from the enumeration below.
//! Each kind of node defines its own structure (inheriting ast_node) to hold its data members.
//! The pointer union in ast_node is here for simple (but unsafe !) cast :
//!   node->as_type_specifier will be a type_specifier_node pointer.
//!
//! The nodes are defined in pr_ast_nodes.inc w/ the DECL_NODE macro.
//! This file is included several times below in order to build up enumeration constants, casting pointers
//!   and public node structures.

namespace pr
{
    //! Tag enumeration constants.
    #define DECL_NODE(tag_name, name, members) tag_name,
    enum
    {
        #include "nut/pr_ast_nodes.inc"
    };
    #undef DECL_NODE
    
    //! Forward declaration of node struct's.
    #define DECL_NODE(tag_name, name, members) struct name ## _node;    
    #include "nut/pr_ast_nodes.inc"
    #undef DECL_NODE
    
    //! Ast node structure.
    struct ast_node
    {
        ast_node(token const& tok);
        virtual ~ast_node();
        
        //! Tag enumeration for cast.
        int tag;
        
        //! Saved token, from which this node originates.
        token saved_tok;
        
        //! These pointers are init'ed to 0,
        //!   and updated by the pass_fix_ast pass.
        //! parent: the node's parent.
        //! prev:   if the node is in a list (such as a statement block), a pointer
        //!   to the previous element.
        //! next:   same as above, pointing to the next node in the list.
        ast_node* parent;
        ast_node* prev;
        ast_node* next;
        
        //! This is the declarator eventually associated to this node.
        //! It is init'ed to 0, and eventually set by pass_create_declarators.
        sem::declarator* decl;
        //! The result type of the expression, if applicable (statements will get res_tp == 0
        //!   for example).
        //! It is updated by the pass_resolve_result_types.
        //! It is NEVER deallocated (it should point to a node.decl declarator).
        sem::type* res_tp;
        
        //! Children nodes vector.
        std::vector<ast_node*> children;
        
        //! Union containing automatically casted pointers to
        //!   specialized nodes.
        #define DECL_NODE(tag, name, members) name ## _node* as_ ## name;
        union
        {
            ast_node* self;
            
            #include "nut/pr_ast_nodes.inc"
        };
        #undef DECL_NODE
    };
    
    //! Specialized node structures definition.
    #define DECL_NODE(tag_name, name, members) \
        struct name ## _node : public ast_node \
        { \
            name ## _node(token tok) : ast_node(tok) { tag = tag_name; } \
            members \
        };
    #include "nut/pr_ast_nodes.inc"
    #undef DECL_NODE
    
    //! Print an AST node in a human-readable format.
    void ast_node_pretty_print(ast_node* node, std::ostream& os = std::cout);
    
    //! Print an AST tree in a human-readable format.
    void ast_pretty_print(ast_node* root, std::ostream& os = std::cout);
    
    //! Delete an AST tree.
    void ast_free(ast_node* root);
    
    //! Add a children to an AST node.
    void ast_add_child(ast_node* node, ast_node* child);
}

#endif // NUT_PR_AST_H
