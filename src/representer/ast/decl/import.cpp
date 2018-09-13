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

#include <string>

#include "representer/ast/decl/import.hpp"
#include "representer/symtable/fqn.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the import token and the fqn to import declaration from
     */
    import::import(token& tok, const std::string& fqn_name) : m_tok(tok), m_fqn_name(fqn_name) {
    }

    /**
     * copy constructor
     */
    import::import(import& imp) : m_tok(imp.get_token()), m_fqn(imp.get_fqn()), m_fqn_name(imp.get_fqn_name()) {
    }

    /**
     * copy assignment operator
     */
    import& import::operator=(import& imp) {
        m_tok = imp.get_token();
        m_fqn = imp.get_fqn();
        m_fqn_name = imp.get_fqn_name();
        return * this;
    }

    /**
     * get_token
     * returns a token with import source information
     */
    const token& import::get_token() const {
        return m_tok;
    }

    /**
     * set_fqn
     * sets the fqn where this namespace was found
     */
    void import::set_fqn(fqn& l_fqn) {
        m_fqn = l_fqn;
    }

    /**
     * get_fqn
     * returns the fqn where this namespace was found
     */
    fqn& import::get_fqn() {
        return m_fqn;
    }

    /**
     * get_name
     * returns the fully qualified name of the module to import
     */
    std::string& import::get_fqn_name() {
        return m_fqn_name;
    }
}
