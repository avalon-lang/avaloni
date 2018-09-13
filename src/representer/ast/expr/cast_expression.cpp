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

#include <memory>

#include "representer/ast/expr/cast_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the type instance to cast and the expression to cast
     */
    cast_expression::cast_expression(token& tok, type_instance& cast_type_instance, std::shared_ptr<expr> val) : m_tok(tok), m_cast_type_instance(cast_type_instance), m_val(val), m_callee("") {
    }

    /**
     * copy constructor
     */
    cast_expression::cast_expression(const std::shared_ptr<cast_expression>& cast_expr) : m_tok(cast_expr -> get_token()), m_cast_type_instance(cast_expr -> get_cast_type_instance()), m_val(cast_expr -> get_val() -> copy()), m_callee(cast_expr -> get_callee()) {
    }

    /**
     * assignment copy operator
     */
    cast_expression& cast_expression::operator=(const std::shared_ptr<cast_expression>& cast_expr) {
        m_tok = cast_expr -> get_token();
        m_cast_type_instance = cast_expr -> get_cast_type_instance();
        m_val = cast_expr -> get_val() -> copy();
        m_callee = cast_expr -> get_callee();
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& cast_expression::get_token() const {
        return m_tok;
    }

    /**
     * get_cast_type_instance
     * returns the type we are casting to
     */
    type_instance& cast_expression::get_cast_type_instance() {
        return m_cast_type_instance;
    }

    /**
     * get_val
     * returns the cast operator operand
     */
    std::shared_ptr<expr>& cast_expression::get_val() {
        return m_val;
    }

    /**
     * set_callee
     * sets the final function name to call for this specific cast operation
     */
    void cast_expression::set_callee(const std::string& callee) {
        m_callee = callee;
    }

    /**
     * get_callee
     * returns the final function name to call for this cast expression
     */
    const std::string& cast_expression::get_callee() {
        return m_callee;
    }
}
