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

/* AST */
#include "representer/ast/expr/match_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_bool.hpp"

/* Lexer */
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects:
     * - the type of match expression this is
     * - the left operand to the match operator
     * - and the right operand to match operator
     */
    match_expression::match_expression(token& tok, match_expression_type expr_type, std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval) : m_tok(tok), m_expr_type(expr_type), m_lval(lval), m_rval(rval) {
        avalon_bool avl_bool;
        m_instance = avl_bool.get_type_instance();
    }

    /**
     * copy constructor
     */
    match_expression::match_expression(const std::shared_ptr<match_expression>& match_expr) : m_tok(match_expr -> get_token()), m_instance(match_expr -> get_type_instance()), m_expr_type(match_expr -> get_expression_type()), m_lval(match_expr -> get_lval() -> copy()), m_rval(match_expr -> get_rval() -> copy()) {
    }

    /**
     * assignment copy operator
     */
    match_expression& match_expression::operator=(const std::shared_ptr<match_expression>& match_expr) {
        m_tok = match_expr -> get_token();
        m_expr_type = match_expr -> get_expression_type();
        m_instance = match_expr -> get_type_instance();
        m_lval = match_expr -> get_lval() -> copy();
        m_rval = match_expr -> get_rval() -> copy();
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& match_expression::get_token() const {
        return m_tok;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& match_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * get_expression_type
     * returns the type of expression we are dealing
     */
    const match_expression_type& match_expression::get_expression_type() const {
        return m_expr_type;
    }

    /**
     * get_lval
     * returns the left operand to the match operator
     */
    std::shared_ptr<expr>& match_expression::get_lval() {
        return m_lval;
    }

    /**
     * get_rval
     * returns the right operand to the match operator
     */
    std::shared_ptr<expr>& match_expression::get_rval() {
        return m_rval;
    }
}
