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

#include "representer/ast/stmt/while_stmt.hpp"
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/expr/expr.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the token with source code information
     */
    while_stmt::while_stmt(token& tok) : m_tok(tok), m_condition(nullptr) {
    }

    /**
     * copy constructor
     */
    while_stmt::while_stmt(const std::shared_ptr<while_stmt>& wh_stmt, std::shared_ptr<scope>& parent_scope) : m_tok(wh_stmt -> get_token()), m_scope(std::make_shared<scope>(*(wh_stmt -> get_scope()))), m_condition(wh_stmt -> get_condition() -> copy()), m_blc(wh_stmt -> get_block(), m_scope) {
        m_scope -> set_parent(parent_scope);
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& while_stmt::get_token() const {
        return m_tok;
    }

    /**
     * set_scope
     * specify the scope bound to this while statement
     */
    void while_stmt::set_scope(std::shared_ptr<scope>& l_scope) {
        m_scope = l_scope;
    }

    /**
     * get_scope
     * return the scope bound to this while statement
     */
    std::shared_ptr<scope>& while_stmt::get_scope() {
        return m_scope;
    }

    /**
     * set_condition
     * sets the condition under which the loop will execute
     */
    void while_stmt::set_condition(std::shared_ptr<expr>& condition) {
        m_condition = condition;
    }

    /**
     * get_condition
     * returns the condition under which the loop will execute
     */
    std::shared_ptr<expr>& while_stmt::get_condition() {
        return m_condition;
    }

    /**
     * set_block
     * set the block of statements to execute
     */
    void while_stmt::set_block(block_stmt& blc) {
        m_blc = blc;
    }

    /**
     * get_block
     * returns the block of statements that are to be executed for this case
     */
    block_stmt& while_stmt::get_block() {
        return m_blc;
    }
}
