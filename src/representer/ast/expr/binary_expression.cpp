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

#include "representer/ast/expr/binary_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects:
     * - the type of binary expression this is
     * - the left operand to the binary operator
     * - and the right operand to the binary operator
     */
    binary_expression::binary_expression(token& tok, binary_expression_type expr_type, std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval) : m_tok(tok), m_expr_type(expr_type), m_lval(lval), m_rval(rval), m_callee("") {
    }

    /**
     * copy constructor
     */
    binary_expression::binary_expression(const std::shared_ptr<binary_expression>& bin_expr) : m_tok(bin_expr -> get_token()), m_instance(bin_expr -> get_type_instance()), m_expr_type(bin_expr -> get_expression_type()), m_lval(bin_expr -> get_lval() -> copy()), m_rval(bin_expr -> get_rval() -> copy()), m_callee(bin_expr -> get_callee()) {
    }

    /**
     * assignment copy operator
     */
    binary_expression& binary_expression::operator=(const std::shared_ptr<binary_expression>& bin_expr) {
        m_tok = bin_expr -> get_token();
        m_expr_type = bin_expr -> get_expression_type();
        m_instance = bin_expr -> get_type_instance();
        m_lval = bin_expr -> get_lval() -> copy();
        m_rval = bin_expr -> get_rval() -> copy();
        m_callee = bin_expr -> get_callee();
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& binary_expression::get_token() const {
        return m_tok;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void binary_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& binary_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool binary_expression::has_type_instance() {
        if(m_instance.get_name() == "*")
            return false;
        else
            return true;
    }

    /**
     * get_expression_type
     * returns the type of expression we are dealing
     */
    const binary_expression_type& binary_expression::get_expression_type() const {
        return m_expr_type;
    }

    /**
     * get_lval
     * returns the left operand to the binary operator
     */
    std::shared_ptr<expr>& binary_expression::get_lval() {
        return m_lval;
    }

    /**
     * get_rval
     * returns the right operand to the binary operator
     */
    std::shared_ptr<expr>& binary_expression::get_rval() {
        return m_rval;
    }

    /**
     * set_callee
     * sets the final function name to call for this specific binary operation
     */
    void binary_expression::set_callee(const std::string& callee_name) {
        m_callee = callee_name;
    }

    /**
     * get_callee
     * returns the final function name to call for this binary expression
     */
    const std::string& binary_expression::get_callee() {
        return m_callee;
    }
}
