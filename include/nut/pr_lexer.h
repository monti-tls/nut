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

#ifndef NUT_PR_LEXER_H
#define NUT_PR_LEXER_H

#include "nut/pr_token.h"
#include <istream>

namespace pr
{
    struct lexer
    {
        lexer(std::istream& in) : in(in) {};
        
        std::istream& in;
        int next_char;
        
        token next_token;
        token_info current_info;
    };
    
    //! Create a lexer from an input stream.
    lexer lexer_create(std::istream& in);
    
    //! Delete a lexer.
    void lexer_free(lexer& lex);
    
    //! Reset a lexer to the beginning of the stream.
    void lexer_reset(lexer& lex);
    
    //! Seek for the next token ahead in the stream.
    token const& lexer_seek(lexer& lex);
    
    //! Seek for the next token's type ahead in the stream.
    int lexer_seekt(lexer& lex);
    
    //! Get the next token from the inptu stream.
    token lexer_get(lexer& lex);
}

#endif // NUT_PR_LEXER_H
