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

#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the operand of the reference operator
     */
    reference_expression::reference_expression(token& tok, std::shared_ptr<expr> val) : m_tok(tok), m_expression_type(REF_VAR_EXPR), m_val(val), m_var(nullptr), m_var_expr(nullptr), m_index_expr(nullptr), m_index(0), m_callee("") {
    }

    /**
     * copy constructor
     */
    reference_expression::reference_expression(const std::shared_ptr<reference_expression>& ref_expr) : m_tok(ref_expr -> get_token()), m_instance(ref_expr -> get_type_instance()), m_expression_type(ref_expr -> get_expression_type()), m_val(ref_expr -> get_val() -> copy()), m_var(ref_expr -> get_variable()), m_var_expr(ref_expr -> get_variable_expression()), m_index_expr(ref_expr -> get_index_expression()), m_index(ref_expr -> get_index()), m_callee(ref_expr -> get_callee()) {
    }

    /**
     * assignment copy operator
     */
    reference_expression& reference_expression::operator=(const std::shared_ptr<reference_expression>& ref_expr) {
        m_tok = ref_expr -> get_token();
        m_instance = ref_expr -> get_type_instance();
        m_expression_type = ref_expr -> get_expression_type();
        m_val = ref_expr -> get_val() -> copy();
        m_var = ref_expr -> get_variable();
        m_var_expr = ref_expr -> get_variable_expression();
        m_index_expr = ref_expr -> get_index_expression();
        m_index = ref_expr -> get_index();
        m_callee = ref_expr -> get_callee();
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& reference_expression::get_token() const {
        return m_tok;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void reference_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool reference_expression::has_type_instance() {
        if(m_instance.get_name() == "*")
            return false;
        else
            return true;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& reference_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * set_expression_type
     * set the type of reference expression we are dealing with.
     * this can be a reference to a variable, to inside a tuple, list, map or a user defined value (and builtin values as bits, qubits and strings)
     */
    void reference_expression::set_expression_type(reference_expression_type expr_type) {
        m_expression_type = expr_type;
    }

    /**
     * get_expression_type
     * returns the type of expression we are dealing
     */
    const reference_expression_type& reference_expression::get_expression_type() const {
        return m_expression_type;
    }

    /**
     * get_val
     * returns the unary operator operand
     */
    std::shared_ptr<expr>& reference_expression::get_val() {
        return m_val;
    }

    /**
     * set_variable
     * set the variable that is referenced
     */
    void reference_expression::set_variable(std::shared_ptr<variable>& var) {
        m_var = var;
    }

    /**
     * get_variable
     * get the variable that is referenced
     */
    std::shared_ptr<variable>& reference_expression::get_variable() {
        return m_var;
    }

    /**
     * set_variable_expression
     * set the variable expression where to find the value inside tuples, lists, maps or custom values
     */
    void reference_expression::set_variable_expression(std::shared_ptr<expr>& var_expr) {
        m_var_expr = var_expr;
    }

    /**
     * get_variable_expression
     * get the variable expression where to find the value inside tuples, lists, maps or custom values
     */
    std::shared_ptr<expr>& reference_expression::get_variable_expression() {
        return m_var_expr;
    }

    /**
     * set_index_expression
     * set the index expression where to find the value inside tuples, lists, maps or custom values
     */
    void reference_expression::set_index_expression(std::shared_ptr<expr>& index_expr) {
        m_index_expr = index_expr;
    }

    /**
     * get_index_expression
     * get the index expression where to find the value inside tuples, lists, maps or custom values
     */
    std::shared_ptr<expr>& reference_expression::get_index_expression() {
        return m_index_expr;
    }

    /**
     * set_index
     * set the index where to find the value inside tuples, lists, maps or custom values
     */
    void reference_expression::set_index(std::size_t index) {
        m_index = index;
    }

    /**
     * get_index
     * get the index where to find the value inside tuples, lists, maps or custom values
     */
    const std::size_t reference_expression::get_index() const {
        return m_index;
    }

    /**
     * set_callee
     * sets the function name to call in case this reference expression points to a user type
     */
    void reference_expression::set_callee(const std::string& callee_name) {
        m_callee = callee_name;
    }

    /**
     * get_callee
     * returns the function name to call in case this reference expression points to a user type
     */
    const std::string& reference_expression::get_callee() {
        return m_callee;
    }
}
