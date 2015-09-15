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



//! Single-character tokens.
 
DECL_TOKEN_CHAR (COMMA,         ',')
DECL_TOKEN_CHAR (SEMICOLON,     ';')
DECL_TOKEN_CHAR (EQUALS,        '=')
DECL_TOKEN_CHAR (LEFT_PAREN,    '(')
DECL_TOKEN_CHAR (RIGHT_PAREN,   ')')
DECL_TOKEN_CHAR (LEFT_CURLY,    '{')
DECL_TOKEN_CHAR (RIGHT_CURLY,   '}')

//! Multi-character operators.
//! Alphabet : any character contained below.
//! Must not overlap with other alphabets !

DECL_TOKEN_OP   (LESS,          "<")
DECL_TOKEN_OP   (GREATER,       ">")
DECL_TOKEN_OP   (EQUAL,         "==")
DECL_TOKEN_OP   (LESS_EQUAL,    "<=")
DECL_TOKEN_OP   (GREATER_EQUAL, ">=")
DECL_TOKEN_OP   (NOT_EQUAL,     "!=")
DECL_TOKEN_OP   (PLUS,          "+")
DECL_TOKEN_OP   (MINUS,         "-")
DECL_TOKEN_OP   (STAR,          "*")
DECL_TOKEN_OP   (SLASH,         "/")
DECL_TOKEN_OP   (PERCENT,       "%")
DECL_TOKEN_OP   (DOT,           ".")
DECL_TOKEN_OP   (BANG,          "!")
DECL_TOKEN_OP   (TILDE,         "~")
DECL_TOKEN_OP   (PLUSPLUS,      "++")
DECL_TOKEN_OP   (MINUSMINUS,    "--")

//! Keywords.
//! Alphabet : alphanumeric and underscore, no number for the first character.

DECL_TOKEN_KW   (IF,            "if")
DECL_TOKEN_KW   (ELSE,          "else")
DECL_TOKEN_KW   (WHILE,         "while")
DECL_TOKEN_KW   (FOR,           "for")
DECL_TOKEN_KW   (DO,            "do")
DECL_TOKEN_KW   (BREAK,         "break")
DECL_TOKEN_KW   (CONTINUE,      "continue")
DECL_TOKEN_KW   (RETURN,        "return")

//! Ordinary tokens, with hard-coded rules in the lexer.
//! They are here for automatic name generation and consistency only.

DECL_TOKEN      (EOF)
DECL_TOKEN      (BAD)
DECL_TOKEN      (IDENTIFIER)
DECL_TOKEN      (INTEGER)
DECL_TOKEN      (FLOATING)
