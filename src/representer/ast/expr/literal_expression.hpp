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

#ifndef AVALON_AST_EXPR_LITERAL_EXPRESSION_HPP_
#define AVALON_AST_EXPR_LITERAL_EXPRESSION_HPP_

#include <cstddef>
#include <memory>
#include <string>
#include <bitset>
#include <vector>

#include "qpp.h"

#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    enum literal_expression_type {
        INTEGER_EXPR,
        FLOATING_POINT_EXPR,
        STRING_EXPR,
        BIT_EXPR,
        QUBIT_EXPR,
    };

    enum qubit_propagation_direction {
        LEFT_PROPAGATION,
        RIGHT_PROPAGATION,
        NO_PROPAGATION,
    };

    class literal_expression : public expr {
    public:
        /**
         * the constructor expects the token with source code information
         */
        literal_expression(token& tok, literal_expression_type expr_type, const std::string& val);

        /**
         * copy constructor
         */
        literal_expression(const std::shared_ptr<literal_expression>& lit_expr);

        /**
         * assignment copy operator
         */
        literal_expression& operator=(const std::shared_ptr<literal_expression>& lit_expr);

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * set_type_instance
         * sets the type instance of this expression
         */
        void set_type_instance(type_instance& instance);

        /**
         * set_type_instance
         * sets the type instance of this expression with the possibility of specifying
         * if the parser (true case) set the type instance or the inference engine (false case)
         */
        void set_type_instance(type_instance& instance, bool from_parser);

        /**
         * get_type_instance
         * returns the type instance of this expression
         */
        type_instance& get_type_instance();

        /**
         * has_type_instance
         * returns true if the type instance is not star
         */
        bool has_type_instance();

        /**
         * type_instance_from_parser
         * returns true if the type instance was set by the parser
         * this will happen when the user specifies a type directly on an expression
         */
        bool type_instance_from_parser() const;

        /**
         * get_expression_type
         * returns the type of literal this expression holds
         */
        const literal_expression_type& get_expression_type() const;

        /**
         * get_value
         * returns the string representing the value stored within this literal
         */
        const std::string& get_value() const;

        /**
         * get_int_value
         * returns the integer representation of this literal if it is an integer
         * throws a value_error exception if an attempt to retreive the wrong value type is attempted
         */
        long long int get_int_value();

        /**
         * get_float_value
         * returns the double-precision representation of this literal
         * throws a value_error exception if it contains a different literal type
         */
        double get_float_value();

        /**
         * get_string_value
         * returns the string representation of this literal
         * throws a value_error exception if it contains a different literal type
         */
        std::string get_string_value();

        /**
         * get_bit_value
         * returns the 1 bit bitset representating of this literal
         * throws a value_error exception if it contains a different literal type
         */
        std::bitset<1> get_bit_value();

        /**
         * get_bit2_value
         * returns the 2 bits bitset representating of this literal
         * throws a value_error exception if it contains a different literal type
         */
        std::bitset<2> get_bit2_value();

        /**
         * get_bit4_value
         * returns the 4 bits bitset representating of this literal
         * throws a value_error exception if it contains a different literal type
         */
        std::bitset<4> get_bit4_value();

        /**
         * get_bit8_value
         * returns the 8 bits bitset representating of this literal
         * throws a value_error exception if it contains a different literal type
         */
        std::bitset<8> get_bit8_value();

        /**
         * get_qubit_value
         * returns the 1 qubit bitset representating of this literal
         * throws a value_error exception if it contains a different literal type
         */
        qpp::ket get_qubit_value();

        /**
         * set_qubit_value
         * sets the ket value if we have a qubit expression.
         * throws a value_error exception if this literal doesn't contain qubits
         */
        void set_qubit_value(qpp::ket new_ket);

        /**
         * ket_evolved
         * returns true if we have a qubit has had a quantum gate applied
         * throws a value_error exception if this lteral doesn't contain qubits
         */
        bool ket_evolved();

        /**
         * set_index
         * sets the index of this qubit inside a ket
         * throws a value_error exception if this lteral doesn't contain qubits
         */
        void set_index(std::size_t index);

        /**
         * get_index
         * returns the index of this qubit inside a ket
         * throws a value_error exception if this lteral doesn't contain qubits
         */
        std::size_t get_index();

        /**
         * was_measured
         * sets the measument status of this ket
         * throws a value_error exception if this lteral doesn't contain qubits
         */
        void was_measured(bool was_measured);

        /**
         * was_measured
         * returns true if the qubit stored in this literal expression was measured.
         * throws a value_error exception if this literal doesn't contain qubits
         */
        bool was_measured();

        /**
         * add_bound_qubit
         * add a qubit bound to this ket if any
         * throws a value_error exception if this literal doesn't contain qubits
         */
        void add_bound_qubit(std::shared_ptr<literal_expression> const & bound_qubit);

        /**
         * get_bound_qubits
         * returns a vector of qubits bound to this one
         * throws a value_error exception if this literal doesn't contain qubits
         */
        std::vector<std::shared_ptr<literal_expression> >& get_bound_qubits();

        /**
         * token
         * returns this expression token
         * this is useful because it prevents us from casting each expression just to display error messages
         */
        virtual const token& expr_token() const {
            return m_tok;
        }

        /**
         * expr_type_instance
         * returns the type instance of this expression, bypassing casting
         */
        virtual type_instance& expr_type_instance() {
            return m_instance;
        }

        /**
         * has_match_expression
         * returns true if the current expression depends on a match expression
         * this is useful during checking to make sure that variables and function parameters are not initialized with expressions containing match expressions
         */
        virtual bool has_match_expression() {
            return false;
        }
        
        /**
         * is_literal_expression
         * returns true as this is an literal expression
         */
        virtual bool is_literal_expression() {
            return true;
        }

    private:
        /*
         * the token with source code information
         */
        token m_tok;

        /*
         * type instance of this expression
         */
        type_instance m_instance;

        /*
         * whether the type instance is from the parser
         */
        bool m_type_instance_from_parser;

        /*
         * the type of literal expression this is
         */
        literal_expression_type m_expr_type;

        /*
         * the string representation of the literal
         */
        std::string m_val;

        /*
         * Qubits special case:
         * while evolving qubits by the application of quantum gates, we keep a copy of the ket here.
         * This allows us to avoid casting from strings to kets before applying futher gates.
         */
        qpp::ket m_ket;

        /*
         * if we have multiqubits, this sets their index in the list of qubits.
         * the index is from right to left.
         */
        std::size_t m_index;

        /*
         * this flag lets us know if a qubit has been evolved from its initial value.
         */
        bool m_ket_evolved;

        /*
         * this flag lets us know if the qubit in this literal has been measured.
         */
        bool m_was_measured;

        /*
         * if this literal expression contains a vector of qubits that got tensored with this qubit
         */
        std::vector<std::shared_ptr<literal_expression> > m_bound_qubits;
    };
}

#endif
