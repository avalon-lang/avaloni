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

#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/symtable/fqn.hpp"
#include "representer/ast/decl/ns.hpp"
#include "lexer/token.hpp"

#include "representer/exceptions/symbol_already_declared.hpp"


namespace avalon {
    /**
     * the constructor expects the token with source code information
     */
    ns::ns(token& tok) : m_name(tok.get_lexeme()), m_tok(tok) {        
    }

    /**
     * copy constructor
     */
    ns::ns(ns& ns_decl) : m_name(ns_decl.get_name()), m_tok(ns_decl.get_token()), m_fqn(ns_decl.get_fqn()) {
        std::vector<std::shared_ptr<decl> >& n_decls = ns_decl.get_declarations();
        for(auto& n_decl : n_decls) {
            if(n_decl -> is_type()) {
                std::shared_ptr<type> const & type_decl = std::static_pointer_cast<type>(n_decl);
                std::unordered_map<std::string, std::shared_ptr<type> >& specializations = type_decl -> get_specializations();
                for(auto& specialization : specializations) {
                    std::shared_ptr<decl> specialization_decl = specialization.second;
                    m_declarations.push_back(specialization_decl);
                }
            }
            else if(n_decl -> is_function()) {
                std::shared_ptr<function> const & function_decl = std::static_pointer_cast<function>(n_decl);
                std::unordered_map<std::string, std::shared_ptr<function> >& specializations = function_decl -> get_specializations();
                for(auto& specialization : specializations) {
                    std::shared_ptr<decl> specialization_decl = specialization.second;
                    m_declarations.push_back(specialization_decl);
                }
            }
            else if(n_decl -> is_variable()) {
                std::shared_ptr<variable> const & variable_decl = std::static_pointer_cast<variable>(n_decl);
                std::shared_ptr<variable> new_variable = variable_decl;
                m_declarations.push_back(new_variable);
            }
            else {
                throw std::runtime_error("[compiler error] unexpected declaration in namespace class. Are we now allowing statements declarations inside namespaces?");
            }
        }
    }

    /**
     * copy assignment operator
     */
    ns& ns::operator=(ns& ns_decl) {
        m_name = ns_decl.get_name();
        m_tok = ns_decl.get_token();
        m_fqn = ns_decl.get_fqn();

        // copy declarations
        std::vector<std::shared_ptr<decl> >& n_decls = ns_decl.get_declarations();
        for(auto& n_decl : n_decls) {
            if(n_decl -> is_type()) {
                std::shared_ptr<type> const & type_decl = std::static_pointer_cast<type>(n_decl);
                std::unordered_map<std::string, std::shared_ptr<type> >& specializations = type_decl -> get_specializations();
                for(auto& specialization : specializations) {
                    std::shared_ptr<decl> specialization_decl = specialization.second;
                    m_declarations.push_back(specialization_decl);
                }
            }
            else if(n_decl -> is_function()) {
                std::shared_ptr<function> const & function_decl = std::static_pointer_cast<function>(n_decl);
                std::unordered_map<std::string, std::shared_ptr<function> >& specializations = function_decl -> get_specializations();
                for(auto& specialization : specializations) {
                    std::shared_ptr<decl> specialization_decl = specialization.second;
                    m_declarations.push_back(specialization_decl);
                }
            }
            else if(n_decl -> is_variable()) {
                std::shared_ptr<variable> const & variable_decl = std::static_pointer_cast<variable>(n_decl);
                std::shared_ptr<variable> new_variable = variable_decl;
                m_declarations.push_back(new_variable);
            }
            else {
                throw std::runtime_error("[compiler error] unexpected declaration in namespace class. Are we now allowing statements declarations inside namespaces?");
            }
        }

        return * this;
    }

    /**
     * set_name
     * sets the name of the namespace
     */
    void ns::set_name(const std::string& name) {
        m_name = name;
    }

    /**
     * get_name
     * returns the name of the namespace
     */
    const std::string& ns::get_name() const {
        return m_name;
    }

    /**
     * set_token
     * sets a token with the namespace source information
     */
    void ns::set_token(const token& tok) {
        m_tok = tok;
    }

    /**
     * get_token
     * returns a token with namespace source information
     */
    const token& ns::get_token() const {
        return m_tok;
    }

    /**
     * set_fqn
     * sets the fqn where this namespace was found
     */
    void ns::set_fqn(fqn& l_fqn) {
        m_fqn = l_fqn;
    }

    /**
     * get_fqn
     * returns the fqn where this namespace was found
     */
    fqn& ns::get_fqn() {
        return m_fqn;
    }

    /**
     * set_scope
     * specify the scope of all declarations found in this namespace
     */
    void ns::set_scope(std::shared_ptr<scope>& l_scope) {
        for(auto& m_declaration : m_declarations) {
            if(m_declaration -> is_type()) {
                std::shared_ptr<type> type_decl = std::static_pointer_cast<type>(m_declaration);
                try {
                    l_scope -> add_type(m_name, type_decl);
                } catch(symbol_already_declared err) {
                }
            }
            else if(m_declaration -> is_function()) {
                std::shared_ptr<function> function_decl = std::static_pointer_cast<function>(m_declaration);
                std::shared_ptr<scope>& f_scope = function_decl -> get_scope();
                f_scope -> set_parent(l_scope);
                try {
                    l_scope -> add_function(m_name, function_decl);
                } catch(symbol_already_declared err) {
                }
            }
            else if(m_declaration -> is_variable()) {
                std::shared_ptr<variable> variable_decl = std::static_pointer_cast<variable>(m_declaration);
                variable_decl -> set_scope(l_scope);
                try {
                    l_scope -> add_variable(m_name, variable_decl);
                } catch(symbol_already_declared err) {
                }
            }
            else {
                throw std::runtime_error("[compiler error] unexpected declaration in namespace class. Are we now allowing statements declarations inside namespaces?");
            }
        }
    }
    
    /**
     * add_declaration
     * a namespace can contain the following declarations:
     * - type declarations
     * - function declarations
     * - variable declarations
     */
    void ns::add_declaration(std::shared_ptr<decl>& declaration) {
        m_declarations.push_back(declaration);
    }

    /**
     * get_declarations
     * for futher processing, this function is called to get all the declarations that can be found in the namespace
     */
    std::vector<std::shared_ptr<decl> >& ns::get_declarations() {
        return m_declarations;
    }
}
