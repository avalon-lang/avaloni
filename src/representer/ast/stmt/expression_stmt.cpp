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

#include "representer/ast/expr/expr.hpp"
#include "representer/ast/stmt/expression_stmt.hpp"


namespace avalon {
    /**
     * the constructor expects nothing
     */
    expression_stmt::expression_stmt() : m_expression(nullptr) {
    }

    /**
     * copy constructor
     */
    expression_stmt::expression_stmt(const std::shared_ptr<expression_stmt>& expr_stmt) : m_expression(expr_stmt -> get_expression() -> copy()) {        
    }

    /**
     * set_expression
     * when storing expressions in the AST, everything must decay into a declaration.
     * this function decays any of those expressions above into a simple expression statement
     * which in turn will decay into a statement declaration.
     */
    void expression_stmt::set_expression(std::shared_ptr<expr>& expression) {
        m_expression = expression;
    }

    /**
     * get_expression
     * returns the specific expression stored into this expression statement
     */
    std::shared_ptr<expr>& expression_stmt::get_expression() {
        return m_expression;
    }
}
