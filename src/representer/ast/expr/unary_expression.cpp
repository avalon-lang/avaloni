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

#include "representer/ast/expr/unary_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the type of unary expression and the operand of the unary operator
     */
    unary_expression::unary_expression(token& tok, unary_expression_type expr_type, std::shared_ptr<expr> val) : m_tok(tok), m_expr_type(expr_type), m_val(val), m_callee("") {
    }

    /**
     * copy constructor
     */
    unary_expression::unary_expression(const std::shared_ptr<unary_expression>& un_expr) : m_tok(un_expr -> get_token()), m_expr_type(un_expr -> get_expression_type()), m_instance(un_expr -> get_type_instance()), m_val(un_expr -> get_val() -> copy()), m_callee(un_expr -> get_callee()) {
    }

    /**
     * assignment copy operator
     */
    unary_expression& unary_expression::operator=(const std::shared_ptr<unary_expression>& un_expr) {
        m_tok = un_expr -> get_token();
        m_expr_type = un_expr -> get_expression_type();
        m_instance = un_expr -> get_type_instance();
        m_val = un_expr -> get_val() -> copy();
        m_callee = un_expr -> get_callee();
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& unary_expression::get_token() const {
        return m_tok;
    }

    /**
     * get_expression_type
     * returns the type of expression we are dealing
     */
    const unary_expression_type& unary_expression::get_expression_type() const {
        return m_expr_type;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void unary_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool unary_expression::has_type_instance() {
        if(m_instance.get_name() == "*")
            return false;
        else
            return true;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& unary_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * get_val
     * returns the unary operator operand
     */
    std::shared_ptr<expr>& unary_expression::get_val() {
        return m_val;
    }

    /**
     * set_callee
     * sets the final function name to call for this specific unary operation
     */
    void unary_expression::set_callee(const std::string& callee) {
        m_callee = callee;
    }

    /**
     * get_callee
     * returns the final function name to call for this unary expression
     */
    const std::string& unary_expression::get_callee() {
        return m_callee;
    }
}
