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

#include <cstddef>
#include <memory>


#include "representer/ast/stmt/return_stmt.hpp"
#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/expr/expr.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the token with source code information
     */
    return_stmt::return_stmt(token& tok) : m_tok(tok), m_expression(nullptr) {        
    }

    /**
     * copy constructor
     */
    return_stmt::return_stmt(const std::shared_ptr<return_stmt>& ret_stmt) : m_tok(ret_stmt -> get_token()), m_expression(ret_stmt -> get_expression() -> copy()) {        
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& return_stmt::get_token() const {
        return m_tok;
    }

    /**
     * set_expression
     * set the expression to be returned if any
     */
    void return_stmt::set_expression(std::shared_ptr<expr>& expression) {
        m_expression = expression;
    }

    /**
     * get_expression
     * returns the expression that is meant to be returned
     */
    std::shared_ptr<expr>& return_stmt::get_expression() {
        return m_expression;
    }

    /**
     * has_expression
     * return true if an expression was supplied to the return statement
     */
    bool return_stmt::has_expression() {
        if(m_expression == nullptr)
            return false;
        else
            return true;
    }
}
