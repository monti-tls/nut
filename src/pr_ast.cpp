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

#include "nut/pr_ast.h"

namespace pr
{
    ast_node::ast_node(token const& tok)
    {
        saved_tok = tok;
        self = this;
        
        parent = prev = next = 0;
        decl = 0;
        res_tp = 0;
    }
    
    ast_node::~ast_node()
    { }
    
    //! Associates a node tag value to a name string.
    struct named_node
    {
        int tag;
        std::string name;
    };
    
    #define DECL_NODE(tag, name, members) { tag, #name },
    
    static named_node named_nodes[] =
    {
        #include "nut/pr_ast_nodes.inc"
    };
    
    static int named_nodes_size = sizeof(named_nodes) / sizeof(named_node);
    
    #undef DECL_NODE
    
    //! Find a named node by tag.
    //! Returns 0 if not found.
    static named_node* ast_find_named_node(int tag)
    {
        for (int i = 0; i < named_nodes_size; ++i)
            if (named_nodes[i].tag == tag)
                return named_nodes + i;
        
        return 0;
    }
    
    //! Print an AST tree, with indent.
    void ast_pretty_print_indented(ast_node* node, std::ostream& os, int indent)
    {
        std::string pre = "";
        for (int i = 0; i < indent; ++i) pre += " ";
        
        os << pre;
        ast_node_pretty_print(node);
        os << std::endl;
        
        for (unsigned int i = 0; i < node->children.size(); ++i)
            ast_pretty_print_indented(node->children[i], os, indent + 2);
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    void ast_node_pretty_print(ast_node* node, std::ostream& os)
    {
        named_node* name = ast_find_named_node(node->tag);
        
        if (name)
            os << "(" << name->name << ")";
        else
            os << "(bogus node)";
    }
    
    void ast_pretty_print(ast_node* root, std::ostream& os)
    {
        ast_pretty_print_indented(root, os, 0);
    }
    
    void ast_free(ast_node* root)
    {
        for (unsigned int i = 0; i < root->children.size(); ++i)
            ast_free(root->children[i]);
        
        if (root->decl)
            declarator_free(root->decl);
        
        delete root;
    }
    
    void ast_add_child(ast_node* node, ast_node* child)
    {
        node->children.push_back(child);
    }
}
