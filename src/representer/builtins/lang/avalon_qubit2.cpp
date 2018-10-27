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
#include "representer/builtins/lang/avalon_qubit2.hpp"
#include "representer/builtins/lang/avalon_gate.hpp"
#include "representer/builtins/lang/avalon_void.hpp"
#include "representer/builtins/lang/avalon_bit2.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_qubit2::avalon_qubit2() : m_qubit2_type(std::make_shared<type>(qubit2_type_tok, VALID)), m_qubit2_instance(qubit2_type_tok, m_qubit2_type, "*") {
        m_qubit2_type -> is_quantum(true);
    }

    /**
     * get_type
     * returns the qubit2 type
     */
    std::shared_ptr<type>& avalon_qubit2::get_type() {
        return m_qubit2_type;
    }

    /**
     * get_type_instance
     * returns the qubit2 type instance
     */
    type_instance& avalon_qubit2::get_type_instance() {
        return m_qubit2_instance;
    }

    /**
     * get_program
     * returns a program that defines the qubit2 type and all functions that operate on qubit2s
     */
    program& avalon_qubit2::get_program() {
        // gate program
        avalon_gate avl_gate;
        program& gate_prog = avl_gate.get_program();
        type_instance gate_instance = avl_gate.get_type_instance();

        // void program
        avalon_void avl_void;
        program& void_prog = avl_void.get_program();
        type_instance void_instance = avl_void.get_type_instance();

        // bit program
        avalon_bit2 avl_bit2;
        program& bit2_prog = avl_bit2.get_program();
        type_instance bit2_instance = avl_bit2.get_type_instance();

        // reference to qubit2s
        type_instance ref_qubit2_instance(ref_type_instance_tok, "*");
        ref_qubit2_instance.add_param(m_qubit2_instance);
        ref_qubit2_instance.is_reference(true);

        /* the program FQN */
        fqn l_fqn("__bifqn_qubit2__", "__bifqn_qubit2__");
        m_qubit2_prog.set_fqn(l_fqn);
        m_qubit2_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_qubit2_prog.set_scope(l_scope);

        /* import the gate program into qubit2 */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import gate program
        std::shared_ptr<import> gate_import = std::make_shared<import>(import_tok, gate_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_gate_import = gate_import;
        m_qubit2_prog.add_declaration(final_gate_import);
        // import void program
        std::shared_ptr<import> void_import = std::make_shared<import>(import_tok, void_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_void_import = void_import;
        m_qubit2_prog.add_declaration(final_void_import);
        // import bit2 program
        std::shared_ptr<import> bit2_import = std::make_shared<import>(import_tok, bit2_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bit2_import = bit2_import;
        m_qubit2_prog.add_declaration(final_bit2_import);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_qubit2_type);
        std::shared_ptr<decl> type_decl = m_qubit2_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(gate_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(ref_qubit2_instance);

        // functions
        // apply
        token qubit2_apply_tok(IDENTIFIER, "apply", 0, 0, "__bif__");
        std::shared_ptr<function> qubit2_apply_function = std::make_shared<function>(qubit2_apply_tok);
        qubit2_apply_function -> set_fqn(l_fqn);
        qubit2_apply_function -> is_public(true);
        qubit2_apply_function -> is_builtin(true);
        qubit2_apply_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> qubit2_apply_scope = std::make_shared<scope>();
        qubit2_apply_scope -> set_parent(l_scope);
        qubit2_apply_function -> set_scope(qubit2_apply_scope);
        qubit2_apply_function -> add_param(param_one);
        qubit2_apply_function -> add_param(param_two);
        qubit2_apply_function -> set_return_type_instance(void_instance);
        std::shared_ptr<decl> apply_function_decl = qubit2_apply_function;
        l_namespace -> add_declaration(apply_function_decl);

        // measure
        token qubit2_measure_tok(IDENTIFIER, "measure", 0, 0, "__bif__");
        std::shared_ptr<function> qubit2_measure_function = std::make_shared<function>(qubit2_measure_tok);
        qubit2_measure_function -> set_fqn(l_fqn);
        qubit2_measure_function -> is_public(true);
        qubit2_measure_function -> is_builtin(true);
        qubit2_measure_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> qubit2_measure_scope = std::make_shared<scope>();
        qubit2_measure_scope -> set_parent(l_scope);
        qubit2_measure_function -> set_scope(qubit2_measure_scope);
        qubit2_measure_function -> add_param(param_two);
        qubit2_measure_function -> set_return_type_instance(bit2_instance);
        std::shared_ptr<decl> measure_function_decl = qubit2_measure_function;
        l_namespace -> add_declaration(measure_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_qubit2_prog.add_declaration(namespace_decl);

        return m_qubit2_prog;
    }
}
