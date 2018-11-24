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

#include "representer/ast/expr/conditional_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the token with source code information
     */
    conditional_expression::conditional_expression(token& tok) : m_tok(tok), m_condition(nullptr), m_if_expression(nullptr), m_else_expression(nullptr) {
    }

    /**
     * copy constructor
     */
    conditional_expression::conditional_expression(const std::shared_ptr<conditional_expression>& cond_expr) : m_tok(cond_expr -> get_token()), m_instance(cond_expr -> get_type_instance()), m_condition(cond_expr -> get_condition() -> copy()), m_if_expression(cond_expr -> get_if_expression() -> copy()), m_else_expression(cond_expr -> get_else_expression() -> copy()) {
    }

    /**
     * assignment copy operator
     */
    conditional_expression& conditional_expression::operator=(const std::shared_ptr<conditional_expression>& cond_expr) {
        m_tok = cond_expr -> get_token();
        m_instance = cond_expr -> get_type_instance();
        m_condition = cond_expr -> get_condition() -> copy();
        m_if_expression = cond_expr -> get_if_expression() -> copy();
        m_else_expression = cond_expr -> get_else_expression() -> copy();
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& conditional_expression::get_token() const {
        return m_tok;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void conditional_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool conditional_expression::has_type_instance() {
        if(m_instance.get_name() == "*")
            return false;
        else
            return true;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& conditional_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * set_condition
     * sets the condition upon which the if or else branch expression gets returned
     */
    void conditional_expression::set_condition(std::shared_ptr<expr>& condition) {
        m_condition = condition;
    }

    /**
     * get_condition
     * returns the condition upon which the if or else branch expression gets returned
     */
    std::shared_ptr<expr>& conditional_expression::get_condition() {
        return m_condition;
    }

    /**
     * set_if_expression
     * sets the expression to return in case the condition is true
     */
    void conditional_expression::set_if_expression(std::shared_ptr<expr>& if_expression) {
        m_if_expression = if_expression;
    }

    /**
     * get_if_expression
     * returns the expression to return in case the condition is true
     */
    std::shared_ptr<expr>& conditional_expression::get_if_expression() {
        return m_if_expression;
    }

    /**
     * set_else_expression
     * sets the expression to return in case the condition is fase
     */
    void conditional_expression::set_else_expression(std::shared_ptr<expr>& else_expression) {
        m_else_expression = else_expression;
    }

    /**
     * get_else_expression
     * returns the expression to return in case the condition is false
     */
    std::shared_ptr<expr>& conditional_expression::get_else_expression() {
        return m_else_expression;
    }
}
