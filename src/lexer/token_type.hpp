/*
 *  This file is part of the Avalon programming language
 * 
 *  Copyright (c) 2018 Ntwali Bashige Toussaint
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifndef AVALON_LEXER_TOKEN_TYPE_HPP_
#define AVALON_LEXER_TOKEN_TYPE_HPP_

namespace avalon {
    enum token_type {
        /* single-character tokens - these tokens can happen in circumstances where they serve different purposes */
        DOT = 0,            // .
        LOGICAL_NOT,        // !  (not)
        BITWISE_NOT,        // ~  (bnot)
        BITWISE_OR,         // |  (bor)
        BITWISE_AND,        // &  (band)
        BITWISE_XOR,        // ^  (xor)
        LEFT_PAREN,         // (
        RIGHT_PAREN,        // )
        LEFT_BRACE,         // {
        RIGHT_BRACE,        // }
        LEFT_BRACKET,       // [
        RIGHT_BRACKET,      // ]
        VERTICAL_BAR,       // |
        UNDERSCORE,         // _
        PLUS,               // +
        DIV,                // /
        MOD,                // %
        QUOTE,              // '
        COMMA,              // ,
        NEWLINE,            // \n
        INDENT,             // space or tab
        DEDENT,             // backspace(s)

        /* one or two character tokens - even if we get one character, we must make sure it is not actually part of a two or more character lexeme */
        EQUAL,              // =
        EQUAL_EQUAL,        // ==
        MATCH,              // ===
        NOT_EQUAL,          // !=
        NOT_MATCH,          // =!=
        GREATER,            // >
        GREATER_EQUAL,      // >=
        LESS,               // <
        LESS_EQUAL,         // <=
        IN,                 // in
        NOT_IN,             // not in
        NEXT_IN,            // next in
        PREV_IN,            // previously in
        IS,                 // is
        IS_NOT,             // is not
        MINUS,              // -
        RETURN_TYPE,        // ->
        NS_LEFT_PAREN,      // -(
        NS_RIGHT_PAREN,     // )-
        LOGICAL_OR,         // || (or)
        LOGICAL_AND,        // && (and)
        LEFT_SHIFT,         // << (lsh)
        RIGHT_SHIFT,        // >> (rsh)
        COLON,              // :
        COLON_COLON,        // ::
        MUL,                // *
        POW,                // **

        /* literals */
        IDENTIFIER,         // variable, function or type name
        INTEGER,            // integers
        FLOATING_POINT,     // floating point numbers
        DECIMAL,            // decimal numbers
        STRING,             // strings
        BITS,               // binary strings
        QUBITS,             // qubits

        /* keywords */
        IMPORT,             // import
        NAMESPACE,          // namespace
        PUBLIC,             // public
        PRIVATE,            // private
        REF,                // reference
        DREF,                // dereference
        TYPE,               // type
        FUNCTION,           // function
        VAR,                // var
        VAL,                // val
        CAST,               // cast
        SWITCH,             // switch for pattern matching
        CASE,               // switch instances
        DEFAULT,            // default case
        IF,                 // if
        ELIF,               // else if
        ELSE,               // else
        FOR,                // for
        EMPTY,              // empty
        WHILE,              // while
        CONTINUE,           // continue
        BREAK,              // break
        RETURN,             // return
        PASS,               // pass

        /* specials */
        AVL_EOF             // end file
    };

    std::string get_token_type_string(token_type type);

    std::ostream& operator<<(std::ostream& os, const token_type& type);
}

#endif
