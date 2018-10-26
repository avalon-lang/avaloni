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
#include "representer/builtins/lang/avalon_float.hpp"
#include "representer/builtins/lang/avalon_gate.hpp"


namespace avalon {
    /**
     * the default constructor expects the dependent type instance parameter
     */
    avalon_gate::avalon_gate() : m_gate_type(std::make_shared<type>(gate_type_tok, VALID)), m_gate_instance(gate_type_tok, m_gate_type, "*") {
        // float type instance
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // Set the Gate constructor on the type
        default_constructor gate_cons(gate_cons_tok, m_gate_type);
        gate_cons.add_param(float_instance);
        gate_cons.add_param(float_instance);
        gate_cons.add_param(float_instance);
        m_gate_type -> add_constructor(gate_cons);

        // set the category of the type instance
        m_gate_instance.set_category(USER);
        m_gate_instance.is_parametrized(false);
    }

    /**
     * get_type
     * returns the gate type
     */
    std::shared_ptr<type>& avalon_gate::get_type() {
        return m_gate_type;
    }

    /**
     * get_type_instance
     * returns the gate type instance
     */
    type_instance avalon_gate::get_type_instance() {
        return m_gate_instance;
    }

    /**
     * get_program
     * returns a program that defines the gate type and all functions that operate on gates
     */
    program& avalon_gate::get_program() {
        // float program
        avalon_float avl_float;
        program& float_prog = avl_float.get_program();
        type_instance float_instance = avl_float.get_type_instance();

        /* the program FQN */
        fqn l_fqn("__bifqn_gate__", "__bifqn_gate__");
        m_gate_prog.set_fqn(l_fqn);
        m_gate_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_gate_prog.set_scope(l_scope);

        /* import the float program into gate since the Gate constructor depends on it */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import float program
        std::shared_ptr<import> float_import = std::make_shared<import>(import_tok, float_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_float_import = float_import;
        m_gate_prog.add_declaration(final_float_import);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the scope and the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_gate_type);
        std::shared_ptr<decl> type_decl = m_gate_type;
        l_namespace -> add_declaration(type_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_gate_prog.add_declaration(namespace_decl);

        return m_gate_prog;
    }
}
