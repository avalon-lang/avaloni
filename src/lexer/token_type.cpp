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

#include <iostream>
#include <string>

#include "lexer/token_type.hpp"


namespace avalon {
    static std::string token_type_to_string[] = {
        "DOT",
        "LOGICAL_NOT",
        "BITWISE_NOT",
        "BITWISE_OR",
        "BITWISE_AND",
        "BITWISE_XOR",
        "LEFT_PAREN",
        "RIGHT_PAREN",
        "LEFT_BRACE",
        "RIGHT_BRACE",
        "LEFT_BRACKET",
        "RIGHT_BRACKET",
        "VERTICAL_BAR",
        "UNDERSCORE",
        "PLUS",
        "DIV",
        "MOD",
        "QUOTE",
        "COMMA",
        "NEWLINE",
        "INDENT",
        "DEDENT",

        /* One or two character tokens - even if we get one character, we must make sure it is not actually part of a two or more character lexeme */
        "EQUAL",
        "EQUAL_EQUAL",
        "MATCH",
        "NOT_EQUAL",
        "NOT_MATCH",
        "GREATER",
        "GREATER_EQUAL",
        "LESS",
        "LESS_EQUAL",
        "IN",
        "NOT_IN",
        "NEXT_IN",
        "PREV_IN",
        "IS",
        "IS_NOT",
        "MINUS",
        "RETURN_TYPE",
        "NS_OPEN",
        "NS_CLOSE",
        "LOGICAL_OR",
        "LOGICAL_AND",
        "LEFT_SHIFT",
        "RIGHT_SHIFT",
        "COLON",
        "COLON_COLON",
        "MUL",
        "POW",

        /* Literals */
        "IDENTIFIER",
        "INTEGER",
        "FLOATING_POINT",
        "DECIMAL",
        "STRING",
        "BITS",
        "QUBITS",

        /* Keywords */
        "IMPORT",
        "NAMESPACE",
        "PUBLIC",
        "PRIVATE",
        "REF",
        "DREF",
        "TYPE",
        "FUNCTION",
        "VAR",
        "VAL",
        "CAST",
        "SWITCH",
        "CASE",
        "DEFAULT",
        "IF",
        "ELIF",
        "ELSE",
        "FOR",
        "EMPTY",
        "WHILE",
        "CONTINUE",
        "BREAK",
        "RETURN",
        "PASS",

        /* Specials */
        "AVL_EOF",
    };

    std::string get_token_type_string(token_type type) {
        return std::string(token_type_to_string[type]);
    }

    std::ostream& operator<<(std::ostream& os, const token_type& type) {
        if(type != AVL_EOF)
            return os << std::string(token_type_to_string[type]);
        else
            return os << "";
    }
}
