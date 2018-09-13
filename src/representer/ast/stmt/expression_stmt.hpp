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

#ifndef AVALON_AST_STMT_EXPRESSION_HPP_
#define AVALON_AST_STMT_EXPRESSION_HPP_

#include <memory>

#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    class expression_stmt: public stmt {
    public:
        /**
         * the constructor expects nothing
         */
        expression_stmt();

        /**
         * copy constructor
         */
        expression_stmt(const std::shared_ptr<expression_stmt>& expr_stmt);

        /**
         * set_expression
         * when storing expressions in the AST, everything must decay into a declaration.
         * this function decays any of those expressions above into a simple expression statement
         * which in turn will decay into a statement declaration.
         */
        void set_expression(std::shared_ptr<expr>& expression);

        /**
         * get_expression
         * returns the specific expression stored into this expression statement
         */
        std::shared_ptr<expr>& get_expression();

        /**
         * token
         * returns this statement token
         */
        virtual const token& stmt_token() const {
            return m_expression -> expr_token();
        }

        /**
         * is_expression
         * returns true since we are dealing with a statement expression
         */
        virtual bool is_expression() {
            return true;
        }

    private:
        /**
         * pointer to the expression stored into this expression statement
         */
        std::shared_ptr<expr> m_expression;
    };
}

#endif
