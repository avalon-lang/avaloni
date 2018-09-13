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
#include <vector>

#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/decl.hpp"


namespace avalon {
    /**
     * the constructor expects nothing
     */
    block_stmt::block_stmt() {        
    }

    /**
     * copy constructor
     */
    block_stmt::block_stmt(const block_stmt& blc, std::shared_ptr<scope>& parent_scope) {
        const std::vector<std::shared_ptr<decl> >& declarations = blc.get_declarations();
        for(const auto& declaration : declarations)
            m_declarations.push_back(declaration -> copy(parent_scope));
    }

    /**
     * add_declaration
     * add a declaration to this block
     */
    void block_stmt::add_declaration(std::shared_ptr<decl>& declaration) {
        m_declarations.push_back(declaration);
    }

    /**
     * get_declarations
     * return a vector of declarations that make up this block
     */
    std::vector<std::shared_ptr<decl> >& block_stmt::get_declarations() {
        return m_declarations;
    }

    const std::vector<std::shared_ptr<decl> >& block_stmt::get_declarations() const {
        return m_declarations;
    }

    /**
     * is_empty
     * returns true if this block contains no declarations
     */
    bool block_stmt::is_empty() {
        return m_declarations.empty();
    }
}
