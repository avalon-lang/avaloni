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

#include <algorithm>
#include <iostream>
#include <utility>
#include <string>

#include "lexer/token_type.hpp"
#include "lexer/token.hpp"


namespace avalon {
/**
 * token
 */
/**
 * the token expects the token type, the actual string (lexeme),
 * the line and the column where the token was found.
 */
token::token(
    token_type tok_type,
    std::string lexeme,
    std::size_t line,
    std::size_t column,
    const std::string& source_path
) : m_type(tok_type), m_lexeme(lexeme), m_line(line), m_column(column), m_source_path(source_path) {
    // we remove new lines, carriage returns and empty spaces in lexemes to facilitate display
    m_lexeme.erase(std::remove(m_lexeme.begin(), m_lexeme.end(), '\n'), m_lexeme.end());
    m_lexeme.erase(std::remove(m_lexeme.begin(), m_lexeme.end(), '\r'), m_lexeme.end());
}

    /**
     * get_type
     * returns the type of token
     */
    token_type token::get_type() const {
        return m_type;
    }

    /**
     * get_lexeme
     * returns the string that matched this token
     */
    std::string token::get_lexeme() const {        
        return m_lexeme;
    }

    /**
     * get_line
     * returns the line where this token was found
     */
    std::size_t token::get_line() const {
        return m_line;
    }

    /**
     * get_column
     * returns the column where this token was found
     */
    std::size_t token::get_column() const {
        return m_column;
    }

    /**
     * get_source_path
     * returns the path to the source file where this token was foun
     */
    const std::string& token::get_source_path() const {
        return m_source_path;
    }

    /**
     * overloaded shift for std::cout
     * a friend function that allows us to print a token in std::cout
     */
    std::ostream& operator<<(std::ostream& os, const token& tok) {
        os << tok.get_type() << " <" << tok.get_lexeme() << "> on line " << tok.get_line() << ", column " << tok.get_column();
        return os;
    }


/**
 * number token
 */

/**
 * a number, as a token, expects the token type, a lexeme, a line and a olumn
 */
number::number(
    token_type tok_type,
    std::string lexeme,
    std::size_t line,
    std::size_t column,
    const std::string& source_path
) : token(tok_type, lexeme, line, column, source_path), m_base(""), m_integral(""), m_decimal(""), m_exponent_base(""), m_exponent_power(""), m_data_type("") {    
}
    
    /**
     * during lexing, the token type can change between INTEGER and FLOATING_POINT
     * the same for lexemes, lines and columns.
     */
    void number::update_type(token_type tok_type) {
        m_type = tok_type;
    }

    void number::update_lexeme(std::string lexeme) {
        m_lexeme = lexeme;
    }

    void number::update_line(std::size_t line) {
        m_line = line;
    }

    void number::update_column(std::size_t column) {
        m_column = column;
    }

    /**
     * set_base
     * set the number's base
     */
    void number::set_base(std::string base) {
        m_base = base;
    }

    /**
     * get_base
     * returns the number's base
     * an empty string if no base was provided
     */
    std::string number::get_base() const {
        return m_base;
    }

    /**
     * set_integral
     * set the number's integral part
     */
    void number::set_integral(std::string integral) {
        m_integral = integral;
    }
    
    /**
     * get_integral
     * returns the number's integral part
     * should never return an empty string as every number has at least an integral part
     */
    std::string number::get_integral() const {
        return m_integral;
    }

    /**
     * set_decimal
     * set the number's decimal part
     */
    void number::set_decimal(std::string decimal) {
        m_decimal = decimal;
    }

    /**
     * get_decimal
     * returns the number's decimal part
     * an empty string if no decimal was provided
     */
    std::string number::get_decimal() const {
        return m_decimal;
    }

    /**
     * set_exponent_base
     * set the number's exponent base
     */
    void number::set_exponent_base(std::string exponent_base) {
        m_exponent_base = exponent_base;
    }

    /**
     * get_exponent_base
     * returns the number's exponent base
     * and empty string if none was provided
     */
    std::string number::get_exponent_base() const {
        return m_exponent_base;
    }

    /**
     * set_exponent_base
     * set the number's exponent base
     */
    void number::set_exponent_power(std::string exponent_power) {
        m_exponent_power = exponent_power;
    }
    
    /**
     * get_exponent_base
     * returns the number's exponent base
     * and empty string if none was provided
     */
    std::string number::get_exponent_power() const {
        return m_exponent_power;
    }

    /**
     * set_data_type
     * set the number's data type
     */
    void number::set_data_type(std::string data_type) {
        m_data_type = data_type;
    }

    /**
     * get_data_type
     * returns the number's data type
     * an empty string if none was provided
     */
    std::string number::get_data_type() const {
        return m_data_type;
    }

    /**
     * a friend function that helps us std::cout number tokens
     */
    std::ostream& operator<<(std::ostream& os, const number& num_tok) {
        os << num_tok.get_type() << " \"" << num_tok.get_lexeme() << "\" on line " << num_tok.get_line() << ", column " << num_tok.get_column();        
        return os;
    }


/**
 * string token
 */
avl_string::avl_string(
    token_type tok_type,
    std::string lexeme,
    std::size_t line,
    std::size_t column,
    const std::string& source_path
) : token(tok_type, lexeme, line, column, source_path) {
}
    
    /**
     * a friend function that helps us std::cout avl_string tokens
     */
    std::ostream& operator<<(std::ostream& os, const avl_string& char_tok) {
        os << char_tok.get_type() << " \"" << char_tok.get_lexeme() << "\" on line " << char_tok.get_line() << ", column " << char_tok.get_column();
        return os;
    }


/**
 * General token
 */
token star_tok(MUL, "*", 0, 0, "__bit__");
token type_param_a_tok(IDENTIFIER, "a", 0, 0, "__bit__");

/**
 * Type tokens
 */
token void_type_tok(TYPE, "void", 0, 0, "__bit__");
token bool_type_tok(TYPE, "bool", 0, 0, "__bit__");
token bit_type_tok(TYPE, "bit", 0, 0, "__bit__");
token qubit_type_tok(TYPE, "qubit", 0, 0, "__bit__");
token int_type_tok(TYPE, "int", 0, 0, "__bit__");
token dec_type_tok(TYPE, "dec", 0, 0, "__bit__");
token float_type_tok(TYPE, "float", 0, 0, "__bit__");
token string_type_tok(TYPE, "string", 0, 0, "__bit__");
token tuple_type_tok(TYPE, "(", 0, 0, "__bit__");
token list_type_tok(TYPE, "[", 0, 0, "__bit__");
token map_type_tok(TYPE, "{", 0, 0, "__bit__");
token maybe_type_tok(TYPE, "maybe", 0, 0, "__bit__");
token ref_type_tok(REF, "*", 0, 0, "__bit__");

/**
 * constructor tokens
 */
token true_cons_tok(IDENTIFIER, "True", 0, 0, "__bic__");
token false_cons_tok(IDENTIFIER, "False", 0, 0, "__bic__");
token none_cons_tok(IDENTIFIER, "None", 0, 0, "__bic__");
token just_cons_tok(IDENTIFIER, "Just", 0, 0, "__bic__");

/**
 * Function tokens
 */
token and_function_tok(FUNCTION, "__and__", 0, 0, "__bif__");
token or_function_tok(FUNCTION, "__or__", 0, 0, "__bif__");
token xor_function_tok(FUNCTION, "__xor__", 0, 0, "__bif__");
token not_function_tok(FUNCTION, "__not__", 0, 0, "__bif__");
token pos_function_tok(FUNCTION, "__pos__", 0, 0, "__bif__");
token neg_function_tok(FUNCTION, "__neg__", 0, 0, "__bif__");
token add_function_tok(FUNCTION, "__add__", 0, 0, "__bif__");
token sub_function_tok(FUNCTION, "__sub__", 0, 0, "__bif__");
token mul_function_tok(FUNCTION, "__mul__", 0, 0, "__bif__");
token div_function_tok(FUNCTION, "__div__", 0, 0, "__bif__");
token mod_function_tok(FUNCTION, "__mod__", 0, 0, "__bif__");
token getatrr_function_tok(FUNCTION, "__getattr__", 0, 0, "__bif__");
token setattr_function_tok(FUNCTION, "__setattr__", 0, 0, "__bif__");
token getitem_function_tok(FUNCTION, "__getitem__", 0, 0, "__bif__");
token setitem_function_tok(FUNCTION, "__setitem__", 0, 0, "__bif__");
token hash_function_tok(FUNCTION, "__hash__", 0, 0, "__bif__");

/**
 * variable tokens
 */
token var_one_tok(IDENTIFIER, "__var_one__", 0, 0, "__biv__");
token var_two_tok(IDENTIFIER, "__var_two__", 0, 0, "__biv__");

/**
 * constraint parameters tokens
 */
token param_a_tok(IDENTIFIER, "__param_a__", 0, 0, "__bip__");
token param_b_tok(IDENTIFIER, "__param_a__", 0, 0, "__bip__");
}
