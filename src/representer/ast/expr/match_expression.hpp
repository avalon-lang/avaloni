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

#ifndef AVALON_AST_EXPR_MATCH_EXPRESSION_HPP_
#define AVALON_AST_EXPR_MATCH_EXPRESSION_HPP_

#include <memory>

#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    enum match_expression_type {
        MATCH_EXPR,
        NOT_MATCH_EXPR,
    };

    class match_expression : public expr {
    public:
        /**
         * the constructor expects:
         * - the type of match expression this is
         * - the left operand to the match operator
         * - and the right operand to match operator
         */
        match_expression(token& tok, match_expression_type expr_type, std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval);

        /**
         * copy constructor
         */
        match_expression(const std::shared_ptr<match_expression>& match_expr);

        /**
         * assignment copy operator
         */
        match_expression& operator=(const std::shared_ptr<match_expression>& match_expr);

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * get_type_instance
         * returns the type instance of this expression
         */
        type_instance& get_type_instance();

        /**
         * get_expression_type
         * returns the type of expression we are dealing
         */
        const match_expression_type& get_expression_type() const;

        /**
         * get_lval
         * returns the left operand to the match operator
         */
        std::shared_ptr<expr>& get_lval();

        /**
         * get_rval
         * returns the right operand to the match operator
         */
        std::shared_ptr<expr>& get_rval();

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
            return true;
        }
        
        /**
         * is_match_expression
         * return true as objects of this class are match expression
         */
        virtual bool is_match_expression() {
            return true;
        }

    private:
        /*
         * the equal sign token to be used to show errors
         */
        token m_tok;

        /*
         * type instance of this expression
         */
        type_instance m_instance;

        /*
         * the type of match expression we have
         */
        match_expression_type m_expr_type;

        /*
         * the left operand to the match operator
         */
        std::shared_ptr<expr> m_lval;

        /*
         * the right operand to the match operator
         */
        std::shared_ptr<expr> m_rval;
    };
}

#endif
