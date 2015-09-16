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
#include "nut/pr_symbol.h"
#include <cctype> // std::isdigit & cie

namespace pr
{   
    /*********************************************************/
    /*** Single-char tokens, declared with DECL_TOKEN_CHAR ***/
    /*********************************************************/
    
    struct char_token
    {
        int type;
        char name;
    };
    
    #define DECL_TOKEN(name)
    #define DECL_TOKEN_CHAR(name, char) { TOKEN_ ## name, char },
    #define DECL_TOKEN_OP(name, str)
    #define DECL_TOKEN_KW(name, str)
    
    static char_token char_tokens[] =
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int char_tokens_size = sizeof(char_tokens) / sizeof(char_token);
    
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    #undef DECL_TOKEN
    
    //! Search a single-char token by name.
    //! Return 0 if not found.
    static char_token* find_char_token(char name)
    {
        for (int i = 0; i < char_tokens_size; ++i)
            if (char_tokens[i].name == name)
                return char_tokens + i;
        
        return 0;
    }
    
    /***************************************************/
    /*** Operator tokens, defined with DECL_TOKEN_OP ***/
    /*** A special alphabet is built for those.      ***/
    /***************************************************/
    
    struct op_token
    {
        int type;
        std::string name;
    };
    
    #define DECL_TOKEN(name)
    #define DECL_TOKEN_CHAR(name, char)
    #define DECL_TOKEN_OP(name, str)    { TOKEN_ ## name, str },
    #define DECL_TOKEN_KW(name, str)
    
    static op_token op_tokens[]
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int op_tokens_size = sizeof(op_tokens) / sizeof(op_token);
    
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    #undef DECL_TOKEN
    
    //! Returns whether or not the character is present in the operators alphabet.
    //! The alphabet is built the first time this function is called.
    static bool is_char_in_op_alphabet(int ch)
    {
        static std::string alphabet = "";
        
        // The first time, build the alphabet, containing (uniquely)
        //   all characters present in the OP tokens.
        // std::string::find returns std::string::npos if not found.
        if (!alphabet.size())
        {
            for (int i = 0; i < op_tokens_size; ++i)
            {
                op_token& op = op_tokens[i];
                
                for (unsigned int j = 0; j < op.name.size(); ++j)
                    if (alphabet.find(op.name[j]) == std::string::npos)
                        alphabet += op.name[j];
            }
        }
        
        return alphabet.find(ch) != std::string::npos;
    }
    
    //! Find an operator by name.
    //! Returns 0 if not found.
    static op_token* find_op_token(std::string const& name)
    {
        for (int i = 0; i < op_tokens_size; ++i)
            if (op_tokens[i].name == name)
                return op_tokens + i;
        
        return 0;
    }
    
    /**************************************************/
    /*** Keyword tokens, defined with DECL_TOKEN_KW ***/
    /*** Those are identifier-based.                ***/
    /**************************************************/
    
    struct keyword_token
    {
        int type;
        std::string name;
    };
    
    #define DECL_TOKEN(name)
    #define DECL_TOKEN_CHAR(name, char)
    #define DECL_TOKEN_OP(name, str)
    #define DECL_TOKEN_KW(name, str)    { TOKEN_ ## name, str },
        
    static keyword_token keyword_tokens[] =
    {
        #include "nut/pr_tokens.inc"
    };
    
    static int keyword_tokens_size = sizeof(keyword_tokens)
                                   / sizeof(keyword_token);
    
    #undef DECL_TOKEN_KW
    #undef DECL_TOKEN_OP
    #undef DECL_TOKEN_CHAR
    #undef DECL_TOKEN
    
    //! Search a keyword by name.
    //! Returns 0 if not found.
    static keyword_token* find_keyword_token(std::string const& name)
    {
        for (int i = 0; i < keyword_tokens_size; ++i)
            if (keyword_tokens[i].name == name)
                return keyword_tokens + i;
            
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
            //! Each rule starts with :
            //!   if (!eaten && start_char_constraint)
            //! Upon success, a rule will set eaten to true.
            //! This allows multiple rules to start with the same character.
            bool eaten = false;
            
            //! Numeric literals family :
            //!   - integers
            //!   - floatings
            //! Note that we only support positive numeric literals here,
            //!   to avoid a conflict with the unary minus operator.
            if (!eaten && (
                std::isdigit(lex.next_char) ||
                lex.next_char == '.'))
            {
                std::string value = "";
                bool ok = true;
                
                // If we start with a dot , and the second character is not a digit
                //   we must not continue further and let other rules process the character.
                if (!std::isdigit(lex.next_char))
                {
                    // Here we peek for the second character to be extracted
                    //   using std::istream::peek, to avoid lexer_get'ing it
                    //   and then std::istream::putback.
                    // Note we can't look further ahead that this !
                    ok = std::isdigit(lex.in.peek());
                    
                    // Get the dot.
                    if (ok)
                        value += lexer_get_char(lex);
                }
                
                if (ok)
                {
                    while (std::isdigit(lex.next_char) || lex.next_char == '.')
                    {
                        // Multiple dots are not allowed in numeric literals !
                        if (lex.next_char == '.' && value.find('.') != std::string::npos)
                        {
                            ok = false;
                            break;
                        }
                        
                        value += lexer_get_char(lex);
                    }
                    
                    tok.value = value;
                    if (ok)
                    {
                        eaten = true;
                        
                        if (value.find('.') != std::string::npos)
                            tok.type = TOKEN_FLOATING;
                        else
                            tok.type = TOKEN_INTEGER;
                    }
                }
            }
            
            //! Single-char tokens.
            char_token* cr = find_char_token(lex.next_char);
            if (!eaten && cr)
            {
                eaten = true;
                
                lexer_get_char(lex);
                tok.type = cr->type;
            }
            
            //! Operators.
            if (!eaten && (
                is_char_in_op_alphabet(lex.next_char)))
            {
                std::string name = "";
                
                // Get the operator
                do
                    name += lexer_get_char(lex);
                while (is_char_in_op_alphabet(lex.next_char));
                
                // Save its name
                tok.value = name;
                
                op_token* op = find_op_token(name);
                if (op)
                {
                    eaten = true;
                    tok.type = op->type;
                }
            }
            
            //! Identifiers family (listed higher priority first) :
            //!   - keywords from DECL_TOKEN_KW
            //!   - identifiers
            if (!eaten && (
                std::isalpha(lex.next_char) ||
                lex.next_char == '_'))
            {
               std::string name = "";
               
               // Get the identifier
               do
                   name += lexer_get_char(lex);
               while (std::isalnum(lex.next_char) || lex.next_char == '_');
               
               // Save its name
               tok.value = name;
               
               // Is is a keyword ?
               keyword_token* kw = find_keyword_token(name);
               if (kw)
               {
                   tok.type = kw->type;
                   eaten = true;
               }
               
               //! Otherwise, it is an identifier.
               if (!eaten)
               {
                   tok.type = TOKEN_IDENTIFIER;
                   eaten = true;
               }
            }
        }
        
        tok.info = info;
        return tok;
    }
    
    /*************************/
    /*** Public module API ***/
    /*************************/
    
    lexer lexer_create(std::istream& in, context& ctx)
    {
        lexer lex(in, ctx);
        
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
    
    token const& lexer_peek(lexer& lex)
    {
        return lex.next_token;
    }
    
    int lexer_peekt(lexer& lex)
    {
        return lex.next_token.type;
    }
    
    token lexer_get(lexer& lex)
    {
        token tok = lex.next_token;
        lex.next_token = lexer_get_token(lex);
        return tok;
    }
    
    std::string lexer_getline(lexer& lex, int n)
    {
        // Save current position and go to beginning of the stream
        int saved = lex.in.tellg();
        lex.in.clear();
        lex.in.seekg(0, std::ios::beg);
        
        std::string line = "";
        for (int i = 0; i < n; ++i)
            std::getline(lex.in, line);
        
        // Restore saved position, clearing bad (or eof !) bits
        lex.in.clear();
        lex.in.seekg(saved, std::ios::beg);
        
        return line;
    }
}
