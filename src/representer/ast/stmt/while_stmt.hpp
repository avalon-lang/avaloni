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

#ifndef AVALON_AST_STMT_WHILE_STMT_HPP_
#define AVALON_AST_STMT_WHILE_STMT_HPP_

#include <memory>

#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/expr/expr.hpp"
#include "lexer/token.hpp"


namespace avalon {
    class while_stmt : public stmt {
    public:
        /**
         * the constructor expects the token with source code information
         */
        while_stmt(token& tok);

        /**
         * copy constructor
         */
        while_stmt(const std::shared_ptr<while_stmt>& wh_stmt, std::shared_ptr<scope>& parent_scope);

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * set_scope
         * specify the scope bound to this while statement
         */
        void set_scope(std::shared_ptr<scope>& l_scope);

        /**
         * get_scope
         * return the scope bound to this while statement
         */
        std::shared_ptr<scope>& get_scope();

        /**
         * set_condition
         * sets the condition under which the loop will execute
         */
        void set_condition(std::shared_ptr<expr>& condition);

        /**
         * get_condition
         * returns the condition under which the loop will execute
         */
        std::shared_ptr<expr>& get_condition();

        /**
         * set_block
         * set the block of statements to execute
         */
        void set_block(block_stmt& blc);

        /**
         * get_block
         * returns the block of statements that are to be executed for this loop
         */
        block_stmt& get_block();

        /**
         * token
         * returns this statement token
         */
        virtual const token& stmt_token() const {
            return m_tok;
        }

        /**
         * is_while
         * returns true as this is a while statement
         */
        virtual bool is_while() {
            return true;
        }

    private:
        /*
         * token with source code information
         */
        token m_tok;

        /*
         * scope introduced by this case
         */
        std::shared_ptr<scope> m_scope;

        /*
         * the condition undder which the loop will execute its body (or not)
         */
        std::shared_ptr<expr> m_condition;

        /*
         * the body of the loop
         */
        block_stmt m_blc;
    };
}

#endif
