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

#ifndef AVALON_AST_EXPR_REFERENCE_EXPRESSION_HPP_
#define AVALON_AST_EXPR_REFERENCE_EXPRESSION_HPP_

#include <memory>

#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"
#include "lexer/token.hpp"


namespace avalon {
    enum reference_expression_type {
        REF_VAR_EXPR,
        REF_TUPLE_EXPR,
        REF_LIST_EXPR,
        REF_MAP_EXPR,
        REF_CUSTOM_EXPR,
    };

    class reference_expression : public expr {
    public:
        /**
         * the constructor expects the operand of the reference operator
         */
        reference_expression(token& tok, std::shared_ptr<expr> val);

        /**
         * copy constructor
         */
        reference_expression(const std::shared_ptr<reference_expression>& ref_expr);

        /**
         * assignment copy operator
         */
        reference_expression& operator=(const std::shared_ptr<reference_expression>& ref_expr);

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
         * has_type_instance
         * returns true if the type instance is not star
         */
        bool has_type_instance();

        /**
         * get_type_instance
         * returns the type instance of this expression
         */
        type_instance& get_type_instance();

        /**
         * set_expression_type
         * set the type of reference expression we are dealing with.
         * this can be a reference to a variable, to inside a tuple, list, map or a user defined value (and builtin values as bits, qubits and strings)
         */
        void set_expression_type(reference_expression_type expr_type);

        /**
         * get_expression_type
         * returns the type of expression we are dealing
         */
        const reference_expression_type& get_expression_type() const;

        /**
         * get_val
         * returns the unary operator operand
         */
        std::shared_ptr<expr>& get_val();

        /**
         * set_variable
         * set the variable that is referenced
         */
        void set_variable(std::shared_ptr<variable>& var);

        /**
         * get_variable
         * get the variable that is referenced
         */
        std::shared_ptr<variable>& get_variable();

        /**
         * set_variable_expression
         * set the variable expression where to find the value inside tuples, lists, maps or custom values
         */
        void set_variable_expression(std::shared_ptr<expr>& var_expr);

        /**
         * get_variable_expression
         * get the variable expression where to find the value inside tuples, lists, maps or custom values
         */
        std::shared_ptr<expr>& get_variable_expression();

        /**
         * set_index_expression
         * set the index expression where to find the value inside tuples, lists, maps or custom values
         */
        void set_index_expression(std::shared_ptr<expr>& index_expr);

        /**
         * get_index_expression
         * get the index expression where to find the value inside tuples, lists, maps or custom values
         */
        std::shared_ptr<expr>& get_index_expression();

        /**
         * set_index
         * set the index where to find the value inside tuples, lists, maps or custom values
         */
        void set_index(std::size_t index);

        /**
         * get_index
         * get the index where to find the value inside tuples, lists, maps or custom values
         */
        const std::size_t get_index() const;

        /**
         * set_callee
         * sets the function name to call in case this reference expression points to a user type
         */
        void set_callee(const std::string& callee_name);

        /**
         * get_callee
         * returns the function name to call in case this reference expression points to a user type
         */
        const std::string& get_callee();

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
            return m_val -> has_match_expression();
        }
        
        /**
         * is_reference_expression
         * return true as objects of this class are reference expressions
         */
        virtual bool is_reference_expression() {
            return true;
        }

    private:
        /*
         * the token to be used to show errors
         */
        token m_tok;

        /*
         * type instance of this expression
         */
        type_instance m_instance;

        /*
         * the type of expression we are dealing with
         */
        reference_expression_type m_expression_type;

        /*
         * the operand to the reference operator
         */
        std::shared_ptr<expr> m_val;

        /*
         * the variable that is referenced
         */
        std::shared_ptr<variable> m_var;

        /*
         * the identifier expression from which the variable expression is deduced
         */
        std::shared_ptr<expr> m_var_expr;

        /*
         * the expression that will decay to an index
         */
        std::shared_ptr<expr> m_index_expr;

        /*
         * the index within the value in case we have tuple, list, map or custom types
         */
        std::size_t m_index;

        /*
         * the name of the function to call in case we are dealing with a reference from a custom type
         */        
        std::string m_callee;
    };
}

#endif
