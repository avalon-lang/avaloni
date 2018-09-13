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

#include "representer/ast/decl/statement.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/stmt/stmt.hpp"


namespace avalon {
    /**
     * the constructor expects nothing
     */
    statement::statement() : m_statement(nullptr), m_reachable(false), m_terminates(false), m_passes(true) {        
    }

    /**
     * copy constructor
     */
    statement::statement(const std::shared_ptr<statement>& a_statement, std::shared_ptr<scope>& parent_scope) : m_statement(nullptr), m_reachable(a_statement -> is_reachable()), m_terminates(a_statement -> terminates()), m_passes(a_statement -> passes()) {
        std::shared_ptr<stmt>& l_statement = a_statement -> get_statement();
        m_statement = l_statement -> copy(parent_scope);
    }

    /**
     * set_statement
     * since statements can vary but the AST can only contain declarations
     * we invoke this function to store the actual statement in this declaration.
     */
    void statement::set_statement(std::shared_ptr<stmt>& l_statement) {
        m_statement = l_statement;
    }

    /**
     * get_statement
     * return the statement stored in this declaration
     */
    std::shared_ptr<stmt>& statement::get_statement() {
        return m_statement;
    }
}
