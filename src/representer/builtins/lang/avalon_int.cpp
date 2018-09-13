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
#include "representer/builtins/lang/avalon_maybe.hpp"
#include "representer/builtins/lang/avalon_float.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_int.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_int::avalon_int() : m_int_type(std::make_shared<type>(int_type_tok, VALID)), m_int_instance(int_type_tok, m_int_type, "*") {
    }

    /**
     * get_type
     * returns the int type
     */
    std::shared_ptr<type>& avalon_int::get_type() {
        return m_int_type;
    }

    /**
     * get_type_instance
     * returns the int type instance
     */
    type_instance& avalon_int::get_type_instance() {
        return m_int_instance;
    }

    /**
     * get_program
     * returns a program that defines the int type and all functions that operate on ints
     */
    program& avalon_int::get_program() {
        // string type
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        type_instance string_instance = avl_string.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        program& bool_prog = avl_bool.get_program();
        type_instance bool_instance = avl_bool.get_type_instance();

        // float type
        avalon_float avl_float;
        program& float_prog = avl_float.get_program();
        type_instance float_instance = avl_float.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        program& maybe_prog = avl_maybe.get_program();
        type_instance maybe_instance = avl_maybe.get_type_instance(m_int_instance);

        /* the program FQN */
        fqn l_fqn("__bifqn_int__", "__bifqn_int__");
        m_int_prog.set_fqn(l_fqn);
        m_int_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_int_prog.set_scope(l_scope);

        /* import the string program into int */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import string program
        std::shared_ptr<import> string_import = std::make_shared<import>(import_tok, string_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_string_import = string_import;
        m_int_prog.add_declaration(final_string_import);
        // import bool program
        std::shared_ptr<import> bool_import = std::make_shared<import>(import_tok, bool_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bool_import = bool_import;
        m_int_prog.add_declaration(final_bool_import);
        // import float program
        std::shared_ptr<import> float_import = std::make_shared<import>(import_tok, float_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_float_import = float_import;
        m_int_prog.add_declaration(final_float_import);
        // import maybe program
        std::shared_ptr<import> maybe_import = std::make_shared<import>(import_tok, maybe_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_maybe_import = maybe_import;
        m_int_prog.add_declaration(final_maybe_import);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_int_type);
        std::shared_ptr<decl> type_decl = m_int_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(m_int_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(m_int_instance);

        // unary positive
        std::shared_ptr<function> int_pos_function = std::make_shared<function>(pos_function_tok);
        int_pos_function -> set_fqn(l_fqn);
        int_pos_function -> is_public(true);
        int_pos_function -> is_builtin(true);
        int_pos_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_pos_scope = std::make_shared<scope>();
        int_pos_scope -> set_parent(l_scope);
        int_pos_function -> set_scope(int_pos_scope);
        int_pos_function -> add_param(param_one);
        int_pos_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> pos_function_decl = int_pos_function;
        l_namespace -> add_declaration(pos_function_decl);

        // unary negative
        std::shared_ptr<function> int_neg_function = std::make_shared<function>(neg_function_tok);
        int_neg_function -> set_fqn(l_fqn);
        int_neg_function -> is_public(true);
        int_neg_function -> is_builtin(true);
        int_neg_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_neg_scope = std::make_shared<scope>();
        int_neg_scope -> set_parent(l_scope);
        int_neg_function -> set_scope(int_neg_scope);
        int_neg_function -> add_param(param_one);
        int_neg_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> neg_function_decl = int_neg_function;
        l_namespace -> add_declaration(neg_function_decl);

        // integer addition
        std::shared_ptr<function> int_add_function = std::make_shared<function>(add_function_tok);
        int_add_function -> set_fqn(l_fqn);
        int_add_function -> is_public(true);
        int_add_function -> is_builtin(true);
        int_add_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_add_scope = std::make_shared<scope>();
        int_add_scope -> set_parent(l_scope);
        int_add_function -> set_scope(int_add_scope);
        int_add_function -> add_param(param_one);
        int_add_function -> add_param(param_two);
        int_add_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> add_function_decl = int_add_function;
        l_namespace -> add_declaration(add_function_decl);

        // integer substraction
        std::shared_ptr<function> int_sub_function = std::make_shared<function>(sub_function_tok);
        int_sub_function -> set_fqn(l_fqn);
        int_sub_function -> is_public(true);
        int_sub_function -> is_builtin(true);
        int_sub_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_sub_scope = std::make_shared<scope>();
        int_sub_scope -> set_parent(l_scope);
        int_sub_function -> set_scope(int_sub_scope);
        int_sub_function -> add_param(param_one);
        int_sub_function -> add_param(param_two);
        int_sub_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> sub_function_decl = int_sub_function;
        l_namespace -> add_declaration(sub_function_decl);

        // integer multiplication
        std::shared_ptr<function> int_mul_function = std::make_shared<function>(mul_function_tok);
        int_mul_function -> set_fqn(l_fqn);
        int_mul_function -> is_public(true);
        int_mul_function -> is_builtin(true);
        int_mul_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_mul_scope = std::make_shared<scope>();
        int_mul_scope -> set_parent(l_scope);
        int_mul_function -> set_scope(int_mul_scope);
        int_mul_function -> add_param(param_one);
        int_mul_function -> add_param(param_two);
        int_mul_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> mul_function_decl = int_mul_function;
        l_namespace -> add_declaration(mul_function_decl);

        // integer division
        std::shared_ptr<function> int_div_function = std::make_shared<function>(div_function_tok);
        int_div_function -> set_fqn(l_fqn);
        int_div_function -> is_public(true);
        int_div_function -> is_builtin(true);
        int_div_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_div_scope = std::make_shared<scope>();
        int_div_scope -> set_parent(l_scope);
        int_div_function -> set_scope(int_div_scope);
        int_div_function -> add_param(param_one);
        int_div_function -> add_param(param_two);
        int_div_function -> set_return_type_instance(maybe_instance);
        std::shared_ptr<decl> div_function_decl = int_div_function;
        l_namespace -> add_declaration(div_function_decl);

        // integer modulo
        token int_mod_tok(IDENTIFIER, "__mod__", 0, 0, "__bif__");
        std::shared_ptr<function> int_mod_function = std::make_shared<function>(int_mod_tok);
        int_mod_function -> set_fqn(l_fqn);
        int_mod_function -> is_public(true);
        int_mod_function -> is_builtin(true);
        int_mod_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_mod_scope = std::make_shared<scope>();
        int_mod_scope -> set_parent(l_scope);
        int_mod_function -> set_scope(int_mod_scope);
        int_mod_function -> add_param(param_one);
        int_mod_function -> add_param(param_two);
        int_mod_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> mod_function_decl = int_mod_function;
        l_namespace -> add_declaration(mod_function_decl);

        // integer power
        token int_pow_tok(IDENTIFIER, "__pow__", 0, 0, "__bif__");
        std::shared_ptr<function> int_pow_function = std::make_shared<function>(int_pow_tok);
        int_pow_function -> set_fqn(l_fqn);
        int_pow_function -> is_public(true);
        int_pow_function -> is_builtin(true);
        int_pow_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_pow_scope = std::make_shared<scope>();
        int_pow_scope -> set_parent(l_scope);
        int_pow_function -> set_scope(int_pow_scope);
        int_pow_function -> add_param(param_one);
        int_pow_function -> add_param(param_two);
        int_pow_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> pow_function_decl = int_pow_function;
        l_namespace -> add_declaration(pow_function_decl);

        // integer hashing
        std::shared_ptr<function> int_hash_function = std::make_shared<function>(hash_function_tok);
        int_hash_function -> set_fqn(l_fqn);
        int_hash_function -> is_public(true);
        int_hash_function -> is_builtin(true);
        int_hash_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_hash_scope = std::make_shared<scope>();
        int_hash_scope -> set_parent(l_scope);
        int_hash_function -> set_scope(int_hash_scope);
        int_hash_function -> add_param(param_one);
        int_hash_function -> set_return_type_instance(m_int_instance);
        std::shared_ptr<decl> hash_function_decl = int_hash_function;
        l_namespace -> add_declaration(hash_function_decl);

        // int cast to string
        // operator version
        token int_string_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> int_string_cast_function = std::make_shared<function>(int_string_cast_tok);
        int_string_cast_function -> set_fqn(l_fqn);
        int_string_cast_function -> is_public(true);
        int_string_cast_function -> is_builtin(true);
        int_string_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_string_cast_scope = std::make_shared<scope>();
        int_string_cast_scope -> set_parent(l_scope);
        int_string_cast_function -> set_scope(int_string_cast_scope);
        int_string_cast_function -> add_param(param_one);
        int_string_cast_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> string_cast_function_decl = int_string_cast_function;
        l_namespace -> add_declaration(string_cast_function_decl);
        // functional version
        token int_string_tok(IDENTIFIER, "string", 0, 0, "__bif__");
        std::shared_ptr<function> int_string_function = std::make_shared<function>(int_string_tok);
        int_string_function -> set_fqn(l_fqn);
        int_string_function -> is_public(true);
        int_string_function -> is_builtin(true);
        int_string_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_string_scope = std::make_shared<scope>();
        int_string_scope -> set_parent(l_scope);
        int_string_function -> set_scope(int_string_scope);
        int_string_function -> add_param(param_one);
        int_string_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> string_function_decl = int_string_function;
        l_namespace -> add_declaration(string_function_decl);

        // int cast to float
        // operator version
        token int_float_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> int_float_cast_function = std::make_shared<function>(int_float_cast_tok);
        int_float_cast_function -> set_fqn(l_fqn);
        int_float_cast_function -> is_public(true);
        int_float_cast_function -> is_builtin(true);
        int_float_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_float_cast_scope = std::make_shared<scope>();
        int_float_cast_scope -> set_parent(l_scope);
        int_float_cast_function -> set_scope(int_float_cast_scope);
        int_float_cast_function -> add_param(param_one);
        int_float_cast_function -> set_return_type_instance(float_instance);
        std::shared_ptr<decl> float_cast_function_decl = int_float_cast_function;
        l_namespace -> add_declaration(float_cast_function_decl);
        // functional version
        token int_float_tok(IDENTIFIER, "float", 0, 0, "__bif__");
        std::shared_ptr<function> int_float_function = std::make_shared<function>(int_float_tok);
        int_float_function -> set_fqn(l_fqn);
        int_float_function -> is_public(true);
        int_float_function -> is_builtin(true);
        int_float_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_float_scope = std::make_shared<scope>();
        int_float_scope -> set_parent(l_scope);
        int_float_function -> set_scope(int_float_scope);
        int_float_function -> add_param(param_one);
        int_float_function -> set_return_type_instance(float_instance);
        std::shared_ptr<decl> float_function_decl = int_float_function;
        l_namespace -> add_declaration(float_function_decl);

        // __eq__ comparator
        token int_eq_tok(IDENTIFIER, "__eq__", 0, 0, "__bif__");
        std::shared_ptr<function> int_eq_function = std::make_shared<function>(int_eq_tok);
        int_eq_function -> set_fqn(l_fqn);
        int_eq_function -> is_public(true);
        int_eq_function -> is_builtin(true);
        int_eq_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_eq_scope = std::make_shared<scope>();
        int_eq_scope -> set_parent(l_scope);
        int_eq_function -> set_scope(int_eq_scope);
        int_eq_function -> add_param(param_one);
        int_eq_function -> add_param(param_two);
        int_eq_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> eq_function_decl = int_eq_function;
        l_namespace -> add_declaration(eq_function_decl);

        // __ne__ comparator
        token int_ne_tok(IDENTIFIER, "__ne__", 0, 0, "__bif__");
        std::shared_ptr<function> int_ne_function = std::make_shared<function>(int_ne_tok);
        int_ne_function -> set_fqn(l_fqn);
        int_ne_function -> is_public(true);
        int_ne_function -> is_builtin(true);
        int_ne_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_ne_scope = std::make_shared<scope>();
        int_ne_scope -> set_parent(l_scope);
        int_ne_function -> set_scope(int_ne_scope);
        int_ne_function -> add_param(param_one);
        int_ne_function -> add_param(param_two);
        int_ne_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> ne_function_decl = int_ne_function;
        l_namespace -> add_declaration(ne_function_decl);

        // __gt__ comparator
        token int_gt_tok(IDENTIFIER, "__gt__", 0, 0, "__bif__");
        std::shared_ptr<function> int_gt_function = std::make_shared<function>(int_gt_tok);
        int_gt_function -> set_fqn(l_fqn);
        int_gt_function -> is_public(true);
        int_gt_function -> is_builtin(true);
        int_gt_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_gt_scope = std::make_shared<scope>();
        int_gt_scope -> set_parent(l_scope);
        int_gt_function -> set_scope(int_gt_scope);
        int_gt_function -> add_param(param_one);
        int_gt_function -> add_param(param_two);
        int_gt_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> gt_function_decl = int_gt_function;
        l_namespace -> add_declaration(gt_function_decl);

        // __ge__ comparator
        token int_ge_tok(IDENTIFIER, "__ge__", 0, 0, "__bif__");
        std::shared_ptr<function> int_ge_function = std::make_shared<function>(int_ge_tok);
        int_ge_function -> set_fqn(l_fqn);
        int_ge_function -> is_public(true);
        int_ge_function -> is_builtin(true);
        int_ge_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_ge_scope = std::make_shared<scope>();
        int_ge_scope -> set_parent(l_scope);
        int_ge_function -> set_scope(int_ge_scope);
        int_ge_function -> add_param(param_one);
        int_ge_function -> add_param(param_two);
        int_ge_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> ge_function_decl = int_ge_function;
        l_namespace -> add_declaration(ge_function_decl);

        // __lt__ comparator
        token int_lt_tok(IDENTIFIER, "__lt__", 0, 0, "__bif__");
        std::shared_ptr<function> int_lt_function = std::make_shared<function>(int_lt_tok);
        int_lt_function -> set_fqn(l_fqn);
        int_lt_function -> is_public(true);
        int_lt_function -> is_builtin(true);
        int_lt_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_lt_scope = std::make_shared<scope>();
        int_lt_scope -> set_parent(l_scope);
        int_lt_function -> set_scope(int_lt_scope);
        int_lt_function -> add_param(param_one);
        int_lt_function -> add_param(param_two);
        int_lt_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> lt_function_decl = int_lt_function;
        l_namespace -> add_declaration(lt_function_decl);

        // __le__ comparator
        token int_le_tok(IDENTIFIER, "__le__", 0, 0, "__bif__");
        std::shared_ptr<function> int_le_function = std::make_shared<function>(int_le_tok);
        int_le_function -> set_fqn(l_fqn);
        int_le_function -> is_public(true);
        int_le_function -> is_builtin(true);
        int_le_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> int_le_scope = std::make_shared<scope>();
        int_le_scope -> set_parent(l_scope);
        int_le_function -> set_scope(int_le_scope);
        int_le_function -> add_param(param_one);
        int_le_function -> add_param(param_two);
        int_le_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> le_function_decl = int_le_function;
        l_namespace -> add_declaration(le_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_int_prog.add_declaration(namespace_decl);

        return m_int_prog;
    }
}
