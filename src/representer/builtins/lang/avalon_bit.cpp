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
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_int.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"
#include "representer/builtins/lang/avalon_ref.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_bit::avalon_bit() : m_bit_type(std::make_shared<type>(bit_type_tok, VALID)), m_bit_instance(bit_type_tok, m_bit_type, "*") {
    }

    /**
     * get_type
     * returns the bit type
     */
    std::shared_ptr<type>& avalon_bit::get_type() {
        return m_bit_type;
    }

    /**
     * get_type_instance
     * returns the bit type instance
     */
    type_instance& avalon_bit::get_type_instance() {
        return m_bit_instance;
    }

    /**
     * get_program
     * returns a program that defines the bit type and all functions that operate on bits
     */
    program& avalon_bit::get_program() {
        // string type
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        type_instance string_instance = avl_string.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        program& bool_prog = avl_bool.get_program();
        type_instance bool_instance = avl_bool.get_type_instance();

        // int type
        avalon_int avl_int;
        program& int_prog = avl_int.get_program();
        type_instance int_instance = avl_int.get_type_instance();

        // reference to string type
        avalon_ref avl_ref;
        type_instance ref_bit_instance = avl_ref.get_type_instance(m_bit_instance);

        /* the program FQN */
        fqn l_fqn("__bifqn_bit__", "__bifqn_bit__");
        m_bit_prog.set_fqn(l_fqn);
        m_bit_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_bit_prog.set_scope(l_scope);

        /* import the string program into bit */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import string program
        std::shared_ptr<import> string_import = std::make_shared<import>(import_tok, string_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_string_import = string_import;
        m_bit_prog.add_declaration(final_string_import);
        // import bool program
        std::shared_ptr<import> bool_import = std::make_shared<import>(import_tok, bool_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bool_import = bool_import;
        m_bit_prog.add_declaration(final_bool_import);
        // import int program
        std::shared_ptr<import> int_import = std::make_shared<import>(import_tok, int_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_int_import = int_import;
        m_bit_prog.add_declaration(final_int_import);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_bit_type);
        std::shared_ptr<decl> type_decl = m_bit_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(m_bit_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(m_bit_instance);
        variable param_three(var_three_tok, false);
        param_three.set_type_instance(ref_bit_instance);
        variable param_four(var_four_tok, false);
        param_four.set_type_instance(int_instance);

        // bitwise and
        token bit_and_tok(IDENTIFIER, "__band__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_and_function = std::make_shared<function>(bit_and_tok);
        bit_and_function -> set_fqn(l_fqn);
        bit_and_function -> is_public(true);
        bit_and_function -> is_builtin(true);
        bit_and_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_and_scope = std::make_shared<scope>();
        bit_and_scope -> set_parent(l_scope);
        bit_and_function -> set_scope(bit_and_scope);
        bit_and_function -> add_param(param_one);
        bit_and_function -> add_param(param_two);
        bit_and_function -> set_return_type_instance(m_bit_instance);
        std::shared_ptr<decl> and_function_decl = bit_and_function;
        l_namespace -> add_declaration(and_function_decl);

        // bitwise or
        token bit_or_tok(IDENTIFIER, "__bor__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_or_function = std::make_shared<function>(bit_or_tok);
        bit_or_function -> set_fqn(l_fqn);
        bit_or_function -> is_public(true);
        bit_or_function -> is_builtin(true);
        bit_or_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_or_scope = std::make_shared<scope>();
        bit_or_scope -> set_parent(l_scope);
        bit_or_function -> set_scope(bit_or_scope);
        bit_or_function -> add_param(param_one);
        bit_or_function -> add_param(param_two);
        bit_or_function -> set_return_type_instance(m_bit_instance);
        std::shared_ptr<decl> or_function_decl = bit_or_function;
        l_namespace -> add_declaration(or_function_decl);

        // bitwise xor
        token bit_xor_tok(IDENTIFIER, "__xor__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_xor_function = std::make_shared<function>(bit_xor_tok);
        bit_xor_function -> set_fqn(l_fqn);
        bit_xor_function -> is_public(true);
        bit_xor_function -> is_builtin(true);
        bit_xor_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_xor_scope = std::make_shared<scope>();
        bit_xor_scope -> set_parent(l_scope);
        bit_xor_function -> set_scope(bit_xor_scope);
        bit_xor_function -> add_param(param_one);
        bit_xor_function -> add_param(param_two);
        bit_xor_function -> set_return_type_instance(m_bit_instance);
        std::shared_ptr<decl> xor_function_decl = bit_xor_function;
        l_namespace -> add_declaration(xor_function_decl);

        // bitwise not
        token bit_not_tok(IDENTIFIER, "__bnot__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_not_function = std::make_shared<function>(bit_not_tok);
        bit_not_function -> set_fqn(l_fqn);
        bit_not_function -> is_public(true);
        bit_not_function -> is_builtin(true);
        bit_not_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_not_scope = std::make_shared<scope>();
        bit_not_scope -> set_parent(l_scope);
        bit_not_function -> set_scope(bit_not_scope);
        bit_not_function -> add_param(param_one);
        bit_not_function -> set_return_type_instance(m_bit_instance);
        std::shared_ptr<decl> not_function_decl = bit_not_function;
        l_namespace -> add_declaration(not_function_decl);

        // bit cast to string
        // operator version
        token bit_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_cast_function = std::make_shared<function>(bit_cast_tok);
        bit_cast_function -> set_fqn(l_fqn);
        bit_cast_function -> is_public(true);
        bit_cast_function -> is_builtin(true);
        bit_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_cast_scope = std::make_shared<scope>();
        bit_cast_scope -> set_parent(l_scope);
        bit_cast_function -> set_scope(bit_cast_scope);
        bit_cast_function -> add_param(param_one);
        bit_cast_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> cast_function_decl = bit_cast_function;
        l_namespace -> add_declaration(cast_function_decl);
        // function version
        token bit_string_tok(IDENTIFIER, "string", 0, 0, "__bif__");
        std::shared_ptr<function> bit_string_function = std::make_shared<function>(bit_string_tok);
        bit_string_function -> set_fqn(l_fqn);
        bit_string_function -> is_public(true);
        bit_string_function -> is_builtin(true);
        bit_string_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_string_scope = std::make_shared<scope>();
        bit_string_scope -> set_parent(l_scope);
        bit_string_function -> set_scope(bit_string_scope);
        bit_string_function -> add_param(param_one);
        bit_string_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> string_function_decl = bit_string_function;
        l_namespace -> add_declaration(string_function_decl);

        // __eq__ comparator
        token bit_eq_tok(IDENTIFIER, "__eq__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_eq_function = std::make_shared<function>(bit_eq_tok);
        bit_eq_function -> set_fqn(l_fqn);
        bit_eq_function -> is_public(true);
        bit_eq_function -> is_builtin(true);
        bit_eq_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_eq_scope = std::make_shared<scope>();
        bit_eq_scope -> set_parent(l_scope);
        bit_eq_function -> set_scope(bit_eq_scope);
        bit_eq_function -> add_param(param_one);
        bit_eq_function -> add_param(param_two);
        bit_eq_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> eq_function_decl = bit_eq_function;
        l_namespace -> add_declaration(eq_function_decl);

        // __ne__ comparator
        token bit_ne_tok(IDENTIFIER, "__ne__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_ne_function = std::make_shared<function>(bit_ne_tok);
        bit_ne_function -> set_fqn(l_fqn);
        bit_ne_function -> is_public(true);
        bit_ne_function -> is_builtin(true);
        bit_ne_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_ne_scope = std::make_shared<scope>();
        bit_ne_scope -> set_parent(l_scope);
        bit_ne_function -> set_scope(bit_ne_scope);
        bit_ne_function -> add_param(param_one);
        bit_ne_function -> add_param(param_two);
        bit_ne_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> ne_function_decl = bit_ne_function;
        l_namespace -> add_declaration(ne_function_decl);

        // bit subscript access by reference
        token refitem_function_tok(IDENTIFIER, "__refitem__", 0, 0, "__bif__");
        std::shared_ptr<function> bit_refitem_function = std::make_shared<function>(refitem_function_tok);
        bit_refitem_function -> set_fqn(l_fqn);
        bit_refitem_function -> is_public(true);
        bit_refitem_function -> is_builtin(true);
        bit_refitem_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bit_refitem_scope = std::make_shared<scope>();
        bit_refitem_scope -> set_parent(l_scope);
        bit_refitem_function -> set_scope(bit_refitem_scope);
        bit_refitem_function -> add_param(param_three);
        bit_refitem_function -> add_param(param_four);
        bit_refitem_function -> set_return_type_instance(ref_bit_instance);
        std::shared_ptr<decl> refitem_function_decl = bit_refitem_function;
        l_namespace -> add_declaration(refitem_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_bit_prog.add_declaration(namespace_decl);

        return m_bit_prog;
    }
}
