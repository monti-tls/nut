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

namespace pr
{   
    /*********************************************************/
    /*** Single-char tokens, declared with DECL_TOKEN_CHAR ***/
    /*********************************************************/
    
    struct lexer_char
    {
        int type;
        char name;
    };
    
    #define DECL_TOKEN_KW(name, str)
    #define DECL_TOKEN_OP(name, str)
    #define DECL_TOKEN_CHAR(name, char) \
        { TOKEN_ ## name, char },
    
    static lexer_char lexer_chars[] =
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int lexer_chars_size = sizeof(lexer_chars) / sizeof(lexer_char);
    
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_CHAR
    
    //! Search a single-char token by name.
    //! Return 0 if not found.
    static lexer_char* lexer_find_char(char name)
    {
        for (int i = 0; i < lexer_chars_size; ++i)
            if (lexer_chars[i].name == name)
                return lexer_chars + i;
        
        return 0;
    }
    
    /***************************************************/
    /*** Operator tokens, defined with DECL_TOKEN_OP ***/
    /*** A special alphabet is built for those.      ***/
    /***************************************************/
    
    struct lexer_op
    {
        int type;
        std::string name;
    };
    
    #define DECL_TOKEN_KW(name, str)
    #define DECL_TOKEN_CHAR(name, char)
    #define DECL_TOKEN_OP(name, str)\
        { TOKEN_ ## name, str },
    
    static lexer_op lexer_ops[]
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int lexer_ops_size = sizeof(lexer_ops) / sizeof(lexer_op);
    
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    #undef DECL_TOKEN_KW
    
    //! Returns whether or not the character is present in the operators alphabet.
    //! The alphabet is built the first time this function is called.
    static bool lexer_is_in_op_alphabet(int ch)
    {
        static std::string alphabet = "";
        
        // The first time, build the alphabet, containing (uniquely)
        //   all characters present in the OP tokens.
        // std::string::find returns std::string::npos if not found.
        if (!alphabet.size())
        {
            for (int i = 0; i < lexer_ops_size; ++i)
            {
                lexer_op& op = lexer_ops[i];
                
                for (unsigned int j = 0; j < op.name.size(); ++j)
                    if (alphabet.find(op.name[j]) == std::string::npos)
                        alphabet += op.name[j];
            }
        }
        
        return alphabet.find(ch) != std::string::npos;
    }
    
    //! Find an operator by name.
    //! Returns 0 if not found.
    static lexer_op* lexer_find_op(std::string const& name)
    {
        for (int i = 0; i < lexer_ops_size; ++i)
            if (lexer_ops[i].name == name)
                return lexer_ops + i;
        
        return 0;
    }
    
    /**************************************************/
    /*** Keyword tokens, defined with DECL_TOKEN_KW ***/
    /*** Those are identifier-based.                ***/
    /**************************************************/
    
    struct lexer_keyword
    {
        int type;
        std::string name;
    };
    
    #define DECL_TOKEN_CHAR(name, char)
    #define DECL_TOKEN_OP(name, str)
    #define DECL_TOKEN_KW(name, str) \
        { TOKEN_ ## name, str },
        
    static lexer_keyword lexer_keywords[] =
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int lexer_keywords_size = sizeof(lexer_keywords)
                                   / sizeof(lexer_keyword);
    
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    
    //! Search a keyword by name.
    //! Returns 0 if not found.
    static lexer_keyword* lexer_find_keyword(std::string const& name)
    {
        for (int i = 0; i < lexer_keywords_size; ++i)
            if (lexer_keywords[i].name == name)
                return lexer_keywords + i;
            
        return 0;
    }
    
    /**************************************/
    /*** Private implementation section ***/
    /**************************************/
    
    //! Forward declarations.
    static int lexer_get_char(lexer&);
    static token lexer_get_token(lexer&);
    
    //! Init the lexer's internal variables.
    static void lexer_init(lexer& lex)
    {
        lex.current_info.line = 1;
        lex.current_info.column = 0;
        
        // Get first char from the stream
        lex.next_char = 0;
        lexer_get_char(lex);
        
        // Get the first token ready to be peek'd
        lex.next_token = lexer_get_token(lex);
    }
    
    //! Get the next char in the input stream.
    static int lexer_get_char(lexer& lex)
    {
        int ch = lex.next_char;
        lex.next_char = lex.in.get();
        
        // Update location in the input stream
        if (ch == '\n')
        {
            ++lex.current_info.line;
            lex.current_info.column = 0;
        }
        ++lex.current_info.column;
        
        return ch;
    }
    
    //! Skip whitespaces (including new lines).
    static void lexer_skip_ws(lexer& lex)
    {
        while (std::isspace(lex.next_char))
            lexer_get_char(lex);
    }
    
    //! Skip single-line comments.
    static void lexer_skip_comments(lexer& lex)
    {
        lexer_skip_ws(lex);
        
        while (lex.next_char == '#')
        {
            while (lex.next_char != '\n')
            {
                lexer_get_char(lex);
                // Pay attention to EOF
                if (lex.next_char < 0) return;
            }
            
            lexer_skip_ws(lex);
        }
    }
    
    //! Skip unwanted input.
    static void lexer_skip(lexer& lex)
    {
        lexer_skip_comments(lex);
    }
    
    //! Get a token in the input stream.
    static token lexer_get_token(lexer& lex)
    {
        // Ignore unwanted characters
        lexer_skip(lex);
        
        // Prepare the token, bad by default
        token tok;
        tok.type = TOKEN_BAD;
        
        // Save the current location in the input stream
        token_info info = lex.current_info;
        
        if (lex.next_char < 0)
            tok.type = TOKEN_EOF;
        else
        {
            // Single-char tokens
            lexer_char* cr = lexer_find_char(lex.next_char);
            if (cr)
            {
                lexer_get_char(lex);
                tok.type = cr->type;
            }
            // Operators
            else if (lexer_is_in_op_alphabet(lex.next_char))
            {
                std::string name = "";
                
                // Get the operator
                do
                    name += lexer_get_char(lex);
                while (lexer_is_in_op_alphabet(lex.next_char));
                
                // Save its name
                tok.value = name;
                
                lexer_op* op = lexer_find_op(name);
                if (op)
                    tok.type = op->type;
            }
            // Identifiers :
            //   - keywords from DECL_TOKEN_KW
            else if (std::isalpha(lex.next_char) || lex.next_char == '_')
            {
               std::string name = "";
               
               // Get the identifier
               do
                   name += lexer_get_char(lex);
               while (std::isalnum(lex.next_char) || lex.next_char == '_');
               
               // Save its name
               tok.value = name;
               
               lexer_keyword* kw = lexer_find_keyword(name);
               // others ...
               
               if (kw)
                   tok.type = kw->type;
               // else if (other) ...
            }
        }
        
        tok.info = info;
        return tok;
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    lexer lexer_create(std::istream& in)
    {
        lexer lex(in);
        lexer_init(lex);
        return lex;
    }
    
    void lexer_free(lexer&)
    {}
    
    void lexer_reset(lexer& lex)
    {
        lex.in.clear();
        lex.in.seekg(0, std::ios::beg);
        lexer_init(lex);
    }
    
    token const& lexer_seek(lexer& lex)
    {
        return lex.next_token;
    }
    
    int lexer_seekt(lexer& lex)
    {
        return lex.next_token.type;
    }
    
    token lexer_get(lexer& lex)
    {
        token tok = lex.next_token;
        lex.next_token = lexer_get_token(lex);
        return tok;
    }
}
