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

/* Symtable */
#include "representer/symtable/scope.hpp"
#include "representer/symtable/fqn.hpp"

/* AST */
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/program.hpp"


namespace avalon {
/**
 * the default contructor expects nothing
 */
program::program() : m_scope(nullptr), m_is_builtin(false) {
}

    /**
     * set_fqn
     * sets the fqn within which this program lives
     */
    void program::set_fqn(fqn& l_fqn) {
        m_fqn = l_fqn;
    }

    /**
     * get_fqn
     * returns the fqn within which this program lives
     */
    fqn& program::get_fqn() {
        return m_fqn;
    }

    /**
     * set_scope
     * specify the scope bound to this program
     */
    void program::set_scope(std::shared_ptr<scope>& l_scope) {
        m_scope = l_scope;
    }

    /**
     * get_scope
     * return the scope bound to this program
     */
    std::shared_ptr<scope>& program::get_scope() {
        return m_scope;
    }
    
    /**
     * add_declaration
     * a program is made of different types of declarations:
     * - import declarations
     * - namespace declarations
     */
    void program::add_declaration(std::shared_ptr<decl>& declaration) {
        m_declarations.push_back(declaration);
    }

    /**
     * get_declarations
     * for futher processing, this function is called to get all the declarations that
     * make up a program.
     */
    std::vector<std::shared_ptr<decl> >& program::get_declarations() {
        return m_declarations;
    }

    /**
     * is_builtin
     * sets and returns a boolean indicating whether this program contains builtin declarations
     */
    void program::is_builtin(bool is_builtin_) {
        m_is_builtin = is_builtin_;
    }

    bool program::is_builtin() {
        return m_is_builtin;
    }
}
