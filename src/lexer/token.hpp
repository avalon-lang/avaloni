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

#ifndef AVALON_LEXER_TOKEN_HPP_
#define AVALON_LEXER_TOKEN_HPP_

#include <iostream>
#include <utility>
#include <memory>
#include <string>

#include "lexer/token_type.hpp"


namespace avalon {
    class token {
    public:
        /**
         * the token expects the token type, the actual string (lexeme),
         * the line and the column where the token was found.
         */
        token(token_type tok_type, std::string lexeme, std::size_t line, std::size_t column, const std::string& source_path);

        /**
         * get_type
         * returns the type of token
         */
        token_type get_type() const;

        /**
         * get_lexeme
         * returns the string that matched this token
         */
        std::string get_lexeme() const;

        /**
         * get_line
         * returns the line where this token was found
         */
        std::size_t get_line() const;

        /**
         * get_column
         * returns the column where this token was found
         */
        std::size_t get_column() const;

        /**
         * get_source_path
         * returns the path to the source file where this token was foun
         */
        const std::string& get_source_path() const;
        
    protected:
        /**
         * the type of token we have
         */
        token_type m_type;

        /**
         * the string that matched this token
         */
        std::string m_lexeme;

        /**
         * the line where this token was found
         */
        std::size_t m_line;

        /**
         * the column where this token was found.
         * the column will be at the last avl_string of the lexeme,
         * not at the beginning of the lexeme.
         */
        std::size_t m_column;

        /**
         * the source file where this token was found
         */
        std::string m_source_path;

    /**
     * overloaded shift for std::cout
     * a friend function that allows us to print a token in std::cout
     */
    friend std::ostream& operator<<(std::ostream& os, const token& tok);
    };


    class number : public token {
    public:
        /**
         * a number, as a token, expects the token type, a lexeme, a line and a olumn
         */
        number(avalon::token_type tok_type, std::string lexeme, std::size_t line, std::size_t column, const std::string& source_path);

        /**
         * during lexing, the token type can change between INTEGER and FLOATING_POINT
         * the same for lexemes, lines and columns.
         */
        void update_type(avalon::token_type tok_type);
        void update_lexeme(std::string lexeme);
        void update_line(std::size_t line);
        void update_column(std::size_t column);

        /**
         * set_base
         * set the number's base
         */
        void set_base(std::string base);

        /**
         * get_base
         * returns the number's base
         * an empty string if no base was provided
         */
        std::string get_base() const;

        /**
         * set_integral
         * set the number's integral part
         */
        void set_integral(std::string integral);

        /**
         * get_integral
         * returns the number's integral part
         * should never return an empty string as every number has at least an integral part
         */
        std::string get_integral() const;

        /**
         * set_decimal
         * set the number's decimal part
         */
        void set_decimal(std::string decimal);

        /**
         * get_decimal
         * returns the number's decimal part
         * an empty string if no decimal was provided
         */
        std::string get_decimal() const;

        /**
         * set_exponent_base
         * set the number's exponent base
         */
        void set_exponent_base(std::string exponent_base);

        /**
         * get_exponent_base
         * returns the number's exponent base
         * and empty string if none was provided
         */
        std::string get_exponent_base() const;

        /**
         * set_exponent_power
         * set the number's exponent power
         */
        void set_exponent_power(std::string exponent_power);

        /**
         * get_exponent_power
         * returns the exponent power
         * an empty string if none was provided
         */
        std::string get_exponent_power() const;

        /**
         * set_data_type
         * set the number's data type
         */
        void set_data_type(std::string data_type);

        /**
         * get_data_type
         * returns the number's data type
         * an empty string if none was provided
         */
        std::string get_data_type() const;
        
    private:
        /**
         * the number's base
         * if none is provided, base 10 is default
         */
        std::string m_base;

        /**
         * the number's integral part
         * it can never be empty
         */
        std::string m_integral;

        /**
         * the number's decimal part
         * if this is set, we expect the number token type to be FLOATING_POINT
         */
        std::string m_decimal;

        /**
         * the number's exponent part
         * we expect both the base and the power to be available
         * one without the other is a bug
         */
        std::string m_exponent_base;
        std::string m_exponent_power;

        /**
         * the number's data type
         */
        std::string m_data_type;

    /**
     * a friend function that helps us std::cout number tokens
     */
    friend std::ostream& operator<<(std::ostream& os, const number& num_tok);
    };


    class avl_string : public token {
    public:
        /**
         * a string, as a token, expects the token type, a lexeme, a line and a olumn
         */
        avl_string(token_type tok_type, std::string lexeme, std::size_t line, std::size_t column, const std::string& source_path);

    /**
     * a friend function that helps us std::cout avl_string tokens
     */
    friend std::ostream& operator<<(std::ostream& os, const avl_string& char_tok);
    };

    std::ostream& operator<<(std::ostream& os, const token& tok);
    std::ostream& operator<<(std::ostream& os, const number& num_tok);
    std::ostream& operator<<(std::ostream& os, const avl_string& char_tok);
    
    /**
     * operator==
     * operator!=
     *
     * compare two tokens for equality.
     * this used to compare say two type names (since we don't allow the same type names to be reused)
     */
    inline bool operator==(const token& lhs, const token& rhs) {
        if (lhs.get_type() == rhs.get_type() && lhs.get_lexeme() == rhs.get_lexeme())
            return true;
        else
            return false;
    }

    inline bool operator!=(const token& lhs, const token& rhs) {
        return !(lhs == rhs);
    }
    
    /**
     * operator<
     * operator>
     * operator<=
     * operator<=
     *
     * lexicographic comparison of tokens
     * the less than overload is required by the map container where we use tokens as map keys
     */
    inline bool operator<(const token& lhs, const token& rhs) {
        std::string lhs_lexeme = lhs.get_lexeme();
        //std::size_t lhs_line = lhs.get_line();
        //std::size_t lhs_column = lhs.get_column();

        std::string rhs_lexeme = rhs.get_lexeme();
        //std::size_t rhs_line = rhs.get_line();
        //std::size_t rhs_column = rhs.get_column();

        if(lhs_lexeme.compare(rhs_lexeme)  < 0)
            return true;
        if(lhs_lexeme.compare(rhs_lexeme) > 0)
            return false;

        return false;

        //return std::tie(lhs_line, lhs_column) < std::tie(rhs_line, rhs_column);
    }

    inline bool operator>(const token& lhs, const token& rhs) {
        return rhs < lhs;
    }

    inline bool operator<=(const token& lhs, const token& rhs) {
        return !(lhs > rhs);
    }

    inline bool operator>=(const token& lhs, const token& rhs) {
        return !(lhs < rhs);
    }


    /**
     * General token
     */
    extern token star_tok;
    extern token type_param_a_tok;

    /**
     * Type tokens
     */
    extern token void_type_tok;
    extern token bool_type_tok;
    extern token int_type_tok;
    extern token dec_type_tok;
    extern token float_type_tok;
    extern token string_type_tok;
    extern token tuple_type_tok;
    extern token list_type_tok;
    extern token map_type_tok;
    extern token maybe_type_tok;
    extern token ref_type_tok;
    extern token bit_type_tok;
    extern token bit2_type_tok;
    extern token bit4_type_tok;
    extern token bit8_type_tok;
    extern token gate_type_tok;
    extern token qubit_type_tok;
    extern token qubit2_type_tok;
    extern token qubit4_type_tok;
    extern token qubit8_type_tok;

    /**
     * Type instance tokens
     */
    extern token ref_type_instance_tok;

    /**
     * constructor tokens
     */
    extern token true_cons_tok;
    extern token false_cons_tok;
    extern token none_cons_tok;
    extern token just_cons_tok;
    extern token gate_cons_tok;

    /**
     * Function tokens
     */
    extern token and_function_tok;
    extern token or_function_tok;
    extern token xor_function_tok;
    extern token not_function_tok;
    extern token pos_function_tok;
    extern token neg_function_tok;
    extern token add_function_tok;
    extern token sub_function_tok;
    extern token mul_function_tok;
    extern token div_function_tok;
    extern token mod_function_tok;
    extern token getattr_function_tok;
    extern token setattr_function_tok;
    extern token getitem_function_tok;
    extern token setitem_function_tok;
    extern token hash_function_tok;

    /**
     * variable tokens
     */
    extern token var_one_tok;
    extern token var_two_tok;

    /**
     * constraint parameters tokens
     */
    extern token param_a_tok;
    extern token param_b_tok;
}

#endif
