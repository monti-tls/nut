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

#include "nut/pr_pratt.h"
#include "nut/pr_parser.h"
#include "nut/pr_token.h"
    
namespace pr
{
    /***************************************/
    /*** Default implementation (throws) ***/
    /***************************************/
    
    //! The base class for expression elements.
    //! We use a Pratt parser for every expression, therefore
    //!   all values and operators have a struct deriving from this base
    //!   class.
    //! We used a struct to avoid public: qualifiers (and for
    //!   consistency with the rest of the code).
    //! The default nud and led handlers throws to signal a bogus operator
    //!   definitions.
    struct expr_element
    {
        token saved_tok;
        int token_type;
        int lbp;
        
        virtual ~expr_element() {}
        
        virtual ast_node* nud(parser& par)
        {
            parser_parse_error(par, lexer_peek(par.lex), "nud BOGUS!");
            return 0;
        }
        
        virtual ast_node* led(parser& par, ast_node*)
        {
            parser_parse_error(par, lexer_peek(par.lex), "led BOGUS!");
            return 0;
        }
    };
    
    //! Find an expression element by token.
    //! Returns 0 if not found.
    static expr_element* find_expr_element_by_token(token const& tok);
    
    /************************/
    /*** Special elements ***/
    /************************/
    
    //! An integer literal expression element.
    struct expr_integer_literal : public expr_element
    {
        int value;
        
        expr_integer_literal(token const& tok)
        {
            saved_tok = tok;
            value = std::stoi(tok.value);
        }
        
        ast_node* nud(parser&)
        {
            integer_literal_expr_node* node = new integer_literal_expr_node(saved_tok);
            node->value = value;
            return node;
        }
    };
    
    //! An identifier.
    struct expr_identifier : public expr_element
    {
        std::string name;
        
        expr_identifier(token const& tok)
        {
            saved_tok = tok;
            name = tok.value;
        }
        
        ast_node* nud(parser& par)
        {
            identifier_expr_node* node = new identifier_expr_node(saved_tok);
            node->name = name;
            
            if (!scope_find(par.ctx.scp, name))
                parser_parse_error(par, saved_tok, "use of undeclared identifier '" + name + "'");
            
            return node;
        }
    };
    
    /*************************************/
    /*** Automatic operator generation ***/
    /*************************************/

    //! Used to allow macro expansion even with paste operator.
    #define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
    #define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
        
    //! Defines the expression element structure's name.
    #define ELEMENT_STRUCT_NAME(token) expr_ ## token
    //! Defines the LBP for left-associative elements.
    #define ELEMENT_LBP_LEFT (lbp)
    //! Defines the LBP for right-associative elements.
    #define ELEMENT_LBP_RIGHT ((lbp)-1)
    //! Defines a custom LBP.
    #define ELEMENT_LBP_CUSTOM(x) (x)
    //! Defines the LBP for an element depending on its associativity.
    #define ELEMENT_LBP(associativity) CAT(ELEMENT_LBP_, associativity)

    //! Start an element structure declaration.
    #define ELEMENT_BEGIN(token_t, binary_lbp) \
        struct ELEMENT_STRUCT_NAME(token_t) : public expr_element \
        { \
            ELEMENT_STRUCT_NAME(token_t) (token tok) \
            { \
                saved_tok = tok; \
                token_type = token_t; lbp = binary_lbp; \
            }
            
    //! Define an unary operator without node creation and with separate LBP.
    #define ELEMENT_UNARY_SHELL(unary_lbp) \
        ast_node* nud(parser& par) \
        { \
            return pratt_expression(par, unary_lbp); \
        }
    
    //! Define an unary operator without node creation.
    //! This consumes the given token after parsing the sub-expression.
    #define ELEMENT_UNARY_SHELL_CONSUME(unary_lbp, token) \
        ast_node* nud(parser& par) \
        { \
            ast_node* node = pratt_expression(par, unary_lbp); \
            parser_expect(par, token); \
            return node; \
        }
    
    //! Define an unary operator with ast node creattion and separate LBP.
    #define ELEMENT_UNARY(node_type, unary_lbp) \
        ast_node* nud(parser& par) \
        { \
            node_type* node = new node_type(saved_tok); \
            ast_add_child(node, pratt_expression(par, unary_lbp)); \
            return node; \
        }
            
    //! Define a binary operator (w/ associativity and node creation).
    #define ELEMENT_BINARY(node_type, associativity) \
        ast_node* led(parser& par, ast_node* left) \
        { \
            node_type* node = new node_type(saved_tok); \
            ast_add_child(node, left); \
            ast_add_child(node, pratt_expression(par, ELEMENT_LBP(associativity))); \
            return node; \
        }
            
    //! Define a binary operator (w/ associativity and node creation)
    //!   that matches another token after parsing (for example parentheses '(' & ')').
    #define ELEMENT_BINARY_CONSUME(node_type, associativity, token) \
        ast_node* led(parser& par, ast_node* left) \
        { \
            node_type* node = new node_type(saved_tok); \
            ast_add_child(node, left); \
            ast_add_child(node, pratt_expression(par, ELEMENT_LBP(associativity))); \
            parser_expect(par, token); \
            return node; \
        }
        
    //! Terminate an element structure declaration.
    #define ELEMENT_END() };
    
    #include "nut/pr_pratt_elements.inc"
            
    #undef ELEMENT_END
    #undef ELEMENT_BINARY_CONSUME
    #undef ELEMENT_BINARY
    #undef ELEMENT_UNARY
    #undef ELEMENT_UNARY_SHELL_CONSUME
    #undef ELEMENT_UNARY_SHELL
    #undef ELEMENT_BEGIN
    #undef PRIMITIVE_CAT
    #undef CAT
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    expr_element* find_expr_element_by_token(token const& tok)
    {
        switch (tok.type)
        {
        //!
        //! Switch cases for hard-coded elements.
        //!
            case TOKEN_INTEGER:
                return new expr_integer_literal(tok);
                
            case TOKEN_IDENTIFIER:
                return new expr_identifier(tok);
                break;
        
        //!
        //! Define switch cases for automatically generated operator elements.
        //!
        #define ELEMENT_BEGIN(token_t, binary_lbp) \
            case token_t: \
                return new ELEMENT_STRUCT_NAME(token_t)(tok);

        #define ELEMENT_UNARY(node_type, lbp)
        #define ELEMENT_UNARY_SHELL(lbp)
        #define ELEMENT_UNARY_SHELL_CONSUME(lbp, token)
        #define ELEMENT_BINARY(node_type, associativity)
        #define ELEMENT_BINARY_CONSUME(node_type, associativity, token)
        #define ELEMENT_END()
        
        #include "nut/pr_pratt_elements.inc"
        
        #undef ELEMENT_END
        #undef ELEMENT_BINARY_CONSUME
        #undef ELEMENT_BINARY
        #undef ELEMENT_UNARY
        #undef ELEMENT_UNARY_SHELL_CONSUME
        #undef ELEMENT_UNARY_SHELL
        #undef ELEMENT_BEGIN
        };
        
        return 0;
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    ast_node* pratt_expression(parser& par, int rbp)
    {
        token tok;
        expr_element* elem;
        
        // Attempt to get the first element
        tok = lexer_peek(par.lex);
        elem = find_expr_element_by_token(tok);
        if (!elem)
            parser_parse_error(par, lexer_peek(par.lex), "expected expression");
        
        // Eat the associated token and create the first ast node
        lexer_get(par.lex);
        ast_node* left = elem->nud(par);
        delete elem;
        
        for (;;)
        {
            // Find the operator element
            tok = lexer_peek(par.lex);
            elem = find_expr_element_by_token(tok);
            if (!elem)
                break;
            
            // If we reached the precedence limit, stop here
            if (rbp >= elem->lbp)
            {
                delete elem;
                break;
            }
            
            // Led expression
            lexer_get(par.lex);
            left = elem->led(par, left);
            delete elem;
        }
        
        return left;
    }
}
