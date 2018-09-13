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

#ifndef AVALON_AST_STMT_CONTINUE_STMT_HPP_
#define AVALON_AST_STMT_CONTINUE_STMT_HPP_

#include "representer/ast/stmt/stmt.hpp"
#include "lexer/token.hpp"


namespace avalon {
    class continue_stmt : public stmt {
    public:
        /**
         * the constructor expects the token with source code information
         */
        continue_stmt(token& tok);

        /**
         * copy constructor
         */
        continue_stmt(const std::shared_ptr<continue_stmt>& cont_stmt);

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * token
         * returns this statement token
         */
        virtual const token& stmt_token() const {
            return m_tok;
        }

        /**
         * is_continue
         * returns true as this is a continue statement
         */
        virtual bool is_continue() {
            return true;
        }

    private:
        /*
         * token with source code information
         */
        token m_tok;
    };
}

#endif
