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

#ifndef AVALON_AST_EXPR_CONDITIONAL_EXPRESSION_HPP_
#define AVALON_AST_EXPR_CONDITIONAL_EXPRESSION_HPP_

#include <memory>

#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    class conditional_expression : public expr {
    public:
        /**
         * the constructor expects the token with source code information
         */
        conditional_expression(token& tok);

        /**
         * copy constructor
         */
        conditional_expression(const std::shared_ptr<conditional_expression>& cond_expr);

        /**
         * assignment copy operator
         */
        conditional_expression& operator=(const std::shared_ptr<conditional_expression>& cond_expr);

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
         * set_condition
         * sets the condition upon which the if or else branch expression gets returned
         */
        void set_condition(std::shared_ptr<expr>& condition);

        /**
         * get_condition
         * returns the condition upon which the if or else branch expression gets returned
         */
        std::shared_ptr<expr>& get_condition();

        /**
         * set_if_expression
         * sets the expression to return in case the condition is true
         */
        void set_if_expression(std::shared_ptr<expr>& if_expression);

        /**
         * get_if_expression
         * returns the expression to return in case the condition is true
         */
        std::shared_ptr<expr>& get_if_expression();

        /**
         * set_else_expression
         * sets the expression to return in case the condition is fase
         */
        void set_else_expression(std::shared_ptr<expr>& else_expression);

        /**
         * get_else_expression
         * returns the expression to return in case the condition is false
         */
        std::shared_ptr<expr>& get_else_expression();

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
            return m_if_expression -> has_match_expression() || m_else_expression -> has_match_expression();
        }
        
        /**
         * is_conditional_expression
         * return true as objects of this class are conditional expressions
         */
        virtual bool is_conditional_expression() {
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
         * the condition upon which this expression depends
         */
        std::shared_ptr<expr> m_condition;

        /*
         * the expression to return if the condition is true
         */
        std::shared_ptr<expr> m_if_expression;

        /*
         * the expression to return if the condition is false
         */
        std::shared_ptr<expr> m_else_expression;
    };
}

#endif
