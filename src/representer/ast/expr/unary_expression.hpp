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

#ifndef AVALON_AST_EXPR_UNARY_EXPRESSION_HPP_
#define AVALON_AST_EXPR_UNARY_EXPRESSION_HPP_

#include <memory>

#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    enum unary_expression_type {
        PLUS_SIGN_EXPR,
        MINUS_SIGN_EXPR,
        BITWISE_NOT_EXPR,
        LOGICAL_NOT_EXPR
    };

    class unary_expression : public expr {
    public:
        /**
         * the constructor expects the type of unary expression and the operand of the unary operator
         */
        unary_expression(token& tok, unary_expression_type expr_type, std::shared_ptr<expr> val);

        /**
         * copy constructor
         */
        unary_expression(const std::shared_ptr<unary_expression>& un_expr);

        /**
         * assignment copy operator
         */
        unary_expression& operator=(const std::shared_ptr<unary_expression>& un_expr);

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * get_expression_type
         * returns the type of expression we are dealing
         */
        const unary_expression_type& get_expression_type() const;

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
         * get_val
         * returns the unary operator operand
         */
        std::shared_ptr<expr>& get_val();

        /**
         * set_callee
         * sets the final function name to call for this specific unary operation
         */
        void set_callee(const std::string& callee);

        /**
         * get_callee
         * returns the final function name to call for this unary expression
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
         * is_unary_expression
         * return true as objects of this class are unary expressions
         */
        virtual bool is_unary_expression() {
            return true;
        }

    private:
        /*
         * the token to be used to show errors
         */
        token m_tok;

        /*
         * the type of unary expression we have
         */
        unary_expression_type m_expr_type;

        /*
         * type instance of this expression
         */
        type_instance m_instance;

        /*
         * the operand to the unary operator
         */
        std::shared_ptr<expr> m_val;

        /*
         * the final name of the callee (we save this because the final includes type information)
         */        
        std::string m_callee;
    };
}

#endif
