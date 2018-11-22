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
#include <iterator>
#include <cstdlib>
#include <memory>
#include <bitset>
#include <vector>

#include "qpp.h"

#include "representer/ast/expr/literal_expression.hpp"
#include "representer/exceptions/value_error.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the operand of the literal operator
     */
    literal_expression::literal_expression(token& tok, literal_expression_type expr_type, const std::string& val) : m_tok(tok), m_type_instance_from_parser(false), m_expr_type(expr_type), m_val(val), m_start_index(0), m_end_index(0), m_was_measured(false) {
    }

    /**
     * copy constructor
     */
    literal_expression::literal_expression(const std::shared_ptr<literal_expression>& lit_expr) : m_tok(lit_expr -> get_token()), m_instance(lit_expr -> get_type_instance()), m_type_instance_from_parser(lit_expr -> type_instance_from_parser()), m_expr_type(lit_expr -> get_expression_type()), m_val(lit_expr -> get_value()) {
        if(lit_expr -> get_expression_type() == QUBIT_EXPR) {
            m_ket = lit_expr -> get_qubit_value();
            m_start_index = lit_expr -> get_start_index();
            m_end_index = lit_expr -> get_end_index();
            m_was_measured = lit_expr -> was_measured();
        }
    }

    /**
     * assignment copy operator
     */
    literal_expression& literal_expression::operator=(const std::shared_ptr<literal_expression>& lit_expr) {
        m_tok = lit_expr -> get_token();
        m_instance = lit_expr -> get_type_instance();
        m_type_instance_from_parser = lit_expr -> type_instance_from_parser();
        m_expr_type = lit_expr -> get_expression_type();
        m_val = lit_expr -> get_value();
        if(lit_expr -> get_expression_type() == QUBIT_EXPR) {
            m_ket = lit_expr -> get_qubit_value();
            m_start_index = lit_expr -> get_start_index();
            m_end_index = lit_expr -> get_end_index();
            m_was_measured = lit_expr -> was_measured();
        }
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& literal_expression::get_token() const {
        return m_tok;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void literal_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression with the possibility of specifying
     * if the parser (true case) set the type instance or the inference engine (false case)
     */
    void literal_expression::set_type_instance(type_instance& instance, bool from_parser) {
        m_instance = instance;
        m_type_instance_from_parser = from_parser;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& literal_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool literal_expression::has_type_instance() {
        if(m_instance.get_name() == "*")
            return false;
        else
            return true;
    }

    /**
     * type_instance_from_parser
     * returns true if the type instance was set by the parser
     * this will happen when the user specifies a type directly on an expression
     */
    bool literal_expression::type_instance_from_parser() const {
        return m_type_instance_from_parser;
    }

    /**
     * get_expression_type
     * returns the type of literal this expression holds
     */
    const literal_expression_type& literal_expression::get_expression_type() const {
        return m_expr_type;
    }

    /**
     * get_value
     * returns the literal operator operand
     */
    const std::string& literal_expression::get_value() const {
        return m_val;
    }

    /**
     * get_length
     * returns the length of the string representation of the literal within this expression
     */
    std::size_t literal_expression::get_length() const {
        return m_val.length();
    }

    /**
     * get_int_value
     * returns the integer representation of this literal if it is an integer
     * throws a value_error exception if an attempt to retreive the wrong value type is attempted
     */
    long long int literal_expression::get_int_value() {
        if(m_expr_type == INTEGER_EXPR) {
            char * end = NULL;
            long long int value = std::strtoll(m_val.c_str(), &end, 10);
            return value;
        }
        else {
            throw value_error("This literal expression doesn't contain an integer.");
        }
    }

    /**
     * get_float_value
     * returns the double-precision representation of this literal
     * throws a value_error exception if it contains a different literal type
     */
    double literal_expression::get_float_value() {
        if(m_expr_type == FLOATING_POINT_EXPR) {
            double value = ::atof(m_val.c_str());
            return value;
        }
        else {
            throw value_error("This literal expression doesn't contain a decimal.");
        }
    }

    /**
     * get_string_value
     * returns the string representation of this literal
     * throws a value_error exception if it contains a different literal type
     */
    std::string literal_expression::get_string_value() {
        if(m_expr_type == STRING_EXPR) {
            return m_val;
        }
        else {
            throw value_error("This literal expression doesn't contain a string.");
        }
    }

    /**
     * get_bit_value
     * returns the 1 bit bitset representating of this literal
     * throws a value_error exception if it contains a different literal type
     */
    std::bitset<1> literal_expression::get_bit_value() {
        if(m_expr_type == BIT_EXPR) {
            std::bitset<1> bitstring(m_val);
            return bitstring;
        }
        else {
            throw value_error("This literal expression doesn't contain a bit string.");
        }
    }

    /**
     * get_bit2_value
     * returns the 2 bits bitset representating of this literal
     * throws a value_error exception if it contains a different literal type
     */
    std::bitset<2> literal_expression::get_bit2_value() {
        if(m_expr_type == BIT_EXPR) {
            std::bitset<2> bitstring(m_val);
            return bitstring;
        }
        else {
            throw value_error("This literal expression doesn't contain a bit string.");
        }
    }

    /**
     * get_bit4_value
     * returns the 4 bits bitset representating of this literal
     * throws a value_error exception if it contains a different literal type
     */
    std::bitset<4> literal_expression::get_bit4_value() {
        if(m_expr_type == BIT_EXPR) {
            std::bitset<4> bitstring(m_val);
            return bitstring;
        }
        else {
            throw value_error("This literal expression doesn't contain a bit string.");
        }
    }

    /**
     * get_bit8_value
     * returns the 8 bits bitset representating of this literal
     * throws a value_error exception if it contains a different literal type
     */
    std::bitset<8> literal_expression::get_bit8_value() {
        if(m_expr_type == BIT_EXPR) {
            std::bitset<8> bitstring(m_val);
            return bitstring;
        }
        else {
            throw value_error("This literal expression doesn't contain a bit string.");
        }
    }

    /**
     * get_qubit_value
     * returns the 1 qubit bitset representating of this literal
     * throws a value_error exception if it contains a different literal type
     */
    qpp::ket literal_expression::get_qubit_value() {
        if(m_expr_type == QUBIT_EXPR) {
            std::vector<char> expr_data(m_val.begin(), m_val.end());
            std::vector<std::size_t> ket_data;
            std::transform(expr_data.begin(), expr_data.end(), std::back_inserter(ket_data), [](const char bit) { return (std::size_t) bit - '0'; });
            m_ket = qpp::mket(ket_data);
            return m_ket;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }

    /**
     * set_qubit_value
     * sets the ket value if we have a qubit expression.
     * throws a value_error exception if this literal doesn't contain qubits
     */
    void literal_expression::set_qubit_value(qpp::ket new_ket) {
        if(m_expr_type == QUBIT_EXPR) {
            m_ket = new_ket;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }

    /**
     * set_start_index
     * sets the starting index of this qubit inside the quantum processor
     * throws a value_error exception if this lteral doesn't contain qubits
     */
    void literal_expression::set_start_index(std::size_t index) {
        if(m_expr_type == QUBIT_EXPR) {
            m_start_index = index;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }

    /**
     * get_start_index
     * returns the starting index of this qubit inside the quantum processor
     * throws a value_error exception if this lteral doesn't contain qubits
     */
    std::size_t literal_expression::get_start_index() {
        if(m_expr_type == QUBIT_EXPR) {
            return m_start_index;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }

    /**
     * set_end_index
     * sets the ending index of this qubit inside the quantum processor
     * throws a value_error exception if this lteral doesn't contain qubits
     */
    void literal_expression::set_end_index(std::size_t index) {
        if(m_expr_type == QUBIT_EXPR) {
            m_end_index = index;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }

    /**
     * get_end_index
     * returns the ending index of this qubit inside the quantum processor
     * throws a value_error exception if this lteral doesn't contain qubits
     */
    std::size_t literal_expression::get_end_index() {
        if(m_expr_type == QUBIT_EXPR) {
            return m_end_index;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }

    /**
     * was_measured
     * sets the measument status of this ket
     * throws a value_error exception if this lteral doesn't contain qubits
     */
    void literal_expression::was_measured(bool was_measured) {
        if(m_expr_type == QUBIT_EXPR) {
            m_was_measured = was_measured;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }

    /**
     * was_measured
     * and returns true if the qubit stored in this literal expression was measured.
     * throws a value_error exception if this literal doesn't contain qubits
     */
    bool literal_expression::was_measured() {
        if(m_expr_type == QUBIT_EXPR) {
            return m_was_measured;
        }
        else {
            throw value_error("This literal expression doesn't contain a qubit string.");
        }
    }
}
