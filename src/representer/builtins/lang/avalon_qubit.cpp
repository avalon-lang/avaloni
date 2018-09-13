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

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/import.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/program.hpp"

/* Symbol table */
#include "representer/symtable/scope.hpp"
#include "representer/symtable/fqn.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_qubit.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_qubit::avalon_qubit() : m_qubit_type(std::make_shared<type>(qubit_type_tok, VALID)), m_qubit_instance(qubit_type_tok, m_qubit_type, "*") {
    }

    /**
     * get_type
     * returns the qubit type
     */
    std::shared_ptr<type>& avalon_qubit::get_type() {
        return m_qubit_type;
    }

    /**
     * get_type_instance
     * returns the qubit type instance
     */
    type_instance& avalon_qubit::get_type_instance() {
        return m_qubit_instance;
    }

    /**
     * get_program
     * returns a program that defines the qubit type and all functions that operate on qubits
     */
    program& avalon_qubit::get_program() {
        /* the program FQN */
        fqn l_fqn("__bifqn_qubit__", "__bifqn_qubit__");
        m_qubit_prog.set_fqn(l_fqn);
        m_qubit_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_qubit_prog.set_scope(l_scope);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_qubit_type);
        std::shared_ptr<decl> type_decl = m_qubit_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(m_qubit_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(m_qubit_instance);

        // functions

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_qubit_prog.add_declaration(namespace_decl);

        return m_qubit_prog;
    }
}
