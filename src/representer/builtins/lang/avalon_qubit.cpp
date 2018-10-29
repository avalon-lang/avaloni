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
#include "representer/builtins/lang/avalon_gate.hpp"
#include "representer/builtins/lang/avalon_void.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_qubit::avalon_qubit() : m_qubit_type(std::make_shared<type>(qubit_type_tok, VALID)), m_qubit_instance(qubit_type_tok, m_qubit_type, "*") {
        m_qubit_type -> is_quantum(true);
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
        // gate program
        avalon_gate avl_gate;
        program& gate_prog = avl_gate.get_program();
        type_instance gate_instance = avl_gate.get_type_instance();

        // void program
        avalon_void avl_void;
        program& void_prog = avl_void.get_program();
        type_instance void_instance = avl_void.get_type_instance();

        // bit program
        avalon_bit avl_bit;
        program& bit_prog = avl_bit.get_program();
        type_instance bit_instance = avl_bit.get_type_instance();

        // reference to qubits
        type_instance ref_qubit_instance(ref_type_instance_tok, "*");
        ref_qubit_instance.add_param(m_qubit_instance);
        ref_qubit_instance.is_reference(true);

        /* the program FQN */
        fqn l_fqn("__bifqn_qubit__", "__bifqn_qubit__");
        m_qubit_prog.set_fqn(l_fqn);
        m_qubit_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_qubit_prog.set_scope(l_scope);

        /* import the gate program into qubit */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import gate program
        std::shared_ptr<import> gate_import = std::make_shared<import>(import_tok, gate_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_gate_import = gate_import;
        m_qubit_prog.add_declaration(final_gate_import);
        // import void program
        std::shared_ptr<import> void_import = std::make_shared<import>(import_tok, void_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_void_import = void_import;
        m_qubit_prog.add_declaration(final_void_import);
        // import bit program
        std::shared_ptr<import> bit_import = std::make_shared<import>(import_tok, bit_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bit_import = bit_import;
        m_qubit_prog.add_declaration(final_bit_import);

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
        param_one.set_type_instance(gate_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(ref_qubit_instance);

        // functions
        // apply
        token qubit_apply_tok(IDENTIFIER, "apply", 0, 0, "__bif__");
        std::shared_ptr<function> qubit_apply_function = std::make_shared<function>(qubit_apply_tok);
        qubit_apply_function -> set_fqn(l_fqn);
        qubit_apply_function -> is_public(true);
        qubit_apply_function -> is_builtin(true);
        qubit_apply_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> qubit_apply_scope = std::make_shared<scope>();
        qubit_apply_scope -> set_parent(l_scope);
        qubit_apply_function -> set_scope(qubit_apply_scope);
        qubit_apply_function -> add_param(param_one);
        qubit_apply_function -> add_param(param_two);
        qubit_apply_function -> set_return_type_instance(void_instance);
        std::shared_ptr<decl> apply_function_decl = qubit_apply_function;
        l_namespace -> add_declaration(apply_function_decl);

        // measurement
        // function version
        token qubit_measure_tok(IDENTIFIER, "measure", 0, 0, "__bif__");
        std::shared_ptr<function> qubit_measure_function = std::make_shared<function>(qubit_measure_tok);
        qubit_measure_function -> set_fqn(l_fqn);
        qubit_measure_function -> is_public(true);
        qubit_measure_function -> is_builtin(true);
        qubit_measure_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> qubit_measure_scope = std::make_shared<scope>();
        qubit_measure_scope -> set_parent(l_scope);
        qubit_measure_function -> set_scope(qubit_measure_scope);
        qubit_measure_function -> add_param(param_two);
        qubit_measure_function -> set_return_type_instance(bit_instance);
        std::shared_ptr<decl> measure_function_decl = qubit_measure_function;
        l_namespace -> add_declaration(measure_function_decl);
        // operator version
        token bit_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> qubit_cast_function = std::make_shared<function>(bit_cast_tok);
        qubit_cast_function -> set_fqn(l_fqn);
        qubit_cast_function -> is_public(true);
        qubit_cast_function -> is_builtin(true);
        qubit_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> qubit_cast_scope = std::make_shared<scope>();
        qubit_cast_scope -> set_parent(l_scope);
        qubit_cast_function -> set_scope(qubit_cast_scope);
        qubit_cast_function -> add_param(param_two);
        qubit_cast_function -> set_return_type_instance(bit_instance);
        std::shared_ptr<decl> cast_function_decl = qubit_cast_function;
        l_namespace -> add_declaration(cast_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_qubit_prog.add_declaration(namespace_decl);

        return m_qubit_prog;
    }
}
