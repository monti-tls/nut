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

#include "nut/pr_lexer.h"
#include "nut/pr_context.h"
#include "nut/pr_parser.h"
#include "nut/sem_ast_node.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <map>

int expression(pr::lexer& lex, int rbp = 0);

struct op_base
{
    int rbp;
    virtual int nud(pr::lexer& lex) = 0;
    virtual int led(pr::lexer& lex, int left) = 0;
};

struct op_literal : public op_base
{
    int val;
    
    op_literal(int val) : val(val) {
        rbp = 1;
    }
    
    int nud(pr::lexer&) {
        std::cout << "int.nud" << std::endl;
        return val;
    }
    
    int led(pr::lexer&, int) { return 0; }
};

struct op_add : public op_base
{
    op_add() {
        rbp = 10;
    }
    
    // Unary version
    int nud(pr::lexer& lex) {
        std::cout << "add.nud" << std::endl;
        return expression(lex, rbp);
    }
    
    // Binary version
    int led(pr::lexer& lex, int left)
    {
        std::cout << "add.led" << std::endl;
        int right = expression(lex, rbp);
        return left+right;
    }
};

struct op_sub : public op_base
{
    op_sub() {
        rbp = 10;
    }
    
    // Unary version
    int nud(pr::lexer& lex) {
        std::cout << "sub.nud" << std::endl;
        return -expression(lex, rbp);
    }
    
    // Binary version
    int led(pr::lexer& lex, int left)
    {
        std::cout << "sub.led" << std::endl;
        int right = expression(lex, rbp);
        return left-right;
    }
};

op_base* find_by_tok(pr::token tok)
{
    switch (tok.type)
    {
        case pr::TOKEN_INTEGER:
            return new op_literal(std::stoi(tok.value));
        case pr::TOKEN_PLUS:
            return new op_add();
        case pr::TOKEN_MINUS:
            return new op_sub();
            
        default:
            return 0;
    }
}

int expression(pr::lexer& lex, int rbp)
{
    using namespace pr;
    
    token operand_tok = lexer_peek(lex);
    op_base* operand = find_by_tok(operand_tok);
    if (!operand)
        throw std::logic_error("expected expression");
    
    lexer_get(lex);
    int left = operand->nud(lex);
    
    for (;;)
    {
        token op_tok = lexer_peek(lex);
        op_base* op = find_by_tok(op_tok);
        if (!op)
            break;
        
        lexer_get(lex);
        left = op->led(lex, left);
    }
    
    return left;
}

int main()
{
    using namespace pr;
    
    try
    {
        std::string s = "-6+1";
        std::istringstream ss;
        ss.str(s);
        
        context ctx = context_create();
        lexer lex = lexer_create(ss, ctx);
        
        std::cout << expression(lex, 0);
        
        lexer_free(lex);
        context_free(ctx);
    }
    catch (std::exception const& exc)
    {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    
    /*try
    {
        std::ifstream fs("scratch/test.nut");
        
        context ctx = context_create();
        lexer lex = lexer_create(fs, ctx);
        parser par = parser_create(lex, ctx);
        
        sem::ast_node* ast = parser_parse_program(par);
        
        parser_free(par);
        lexer_free(lex);
        context_free(ctx);
        
        sem::ast_pretty_print(ast);
        
        sem::ast_free(ast);
    }
    catch (std::exception const& exc)
    {
        std::cerr << exc.what() << std::endl;
        return -1;
    }*/
    
    return 0;
}
