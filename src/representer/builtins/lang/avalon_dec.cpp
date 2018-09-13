/*
 *  This file is part of the Avalon programming language
 * 
 *  Copyright (c) 2018 Ntwali Bashige Toussadec
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
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_dec.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_dec::avalon_dec() : m_dec_type(std::make_shared<type>(dec_type_tok, VALID)), m_dec_instance(dec_type_tok, m_dec_type, "*") {
    }

    /**
     * get_type
     * returns the dec type
     */
    std::shared_ptr<type>& avalon_dec::get_type() {
        return m_dec_type;
    }

    /**
     * get_type_instance
     * returns the dec type instance
     */
    type_instance& avalon_dec::get_type_instance() {
        return m_dec_instance;
    }

    /**
     * get_program
     * returns a program that defines the dec type and all functions that operate on decs
     */
    program& avalon_dec::get_program() {
        // string type
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        type_instance string_instance = avl_string.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        program& bool_prog = avl_bool.get_program();
        type_instance bool_instance = avl_bool.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        program& maybe_prog = avl_maybe.get_program();
        type_instance maybe_instance = avl_maybe.get_type_instance(m_dec_instance);

        /* the program FQN */
        fqn l_fqn("__bifqn_dec__", "__bifqn_dec__");
        m_dec_prog.set_fqn(l_fqn);
        m_dec_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_dec_prog.set_scope(l_scope);

        /* import the string program deco dec */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import string program
        std::shared_ptr<import> string_import = std::make_shared<import>(import_tok, string_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_string_import = string_import;
        m_dec_prog.add_declaration(final_string_import);
        // import bool program
        std::shared_ptr<import> bool_import = std::make_shared<import>(import_tok, bool_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bool_import = bool_import;
        m_dec_prog.add_declaration(final_bool_import);
        // import maybe program
        std::shared_ptr<import> maybe_import = std::make_shared<import>(import_tok, maybe_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_maybe_import = maybe_import;
        m_dec_prog.add_declaration(final_maybe_import);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_dec_type);
        std::shared_ptr<decl> type_decl = m_dec_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(m_dec_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(m_dec_instance);

        // unary positive
        std::shared_ptr<function> dec_pos_function = std::make_shared<function>(pos_function_tok);
        dec_pos_function -> set_fqn(l_fqn);
        dec_pos_function -> is_public(true);
        dec_pos_function -> is_builtin(true);
        dec_pos_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_pos_scope = std::make_shared<scope>();
        dec_pos_scope -> set_parent(l_scope);
        dec_pos_function -> set_scope(dec_pos_scope);
        dec_pos_function -> add_param(param_one);
        dec_pos_function -> set_return_type_instance(m_dec_instance);
        std::shared_ptr<decl> pos_function_decl = dec_pos_function;
        l_namespace -> add_declaration(pos_function_decl);

        // unary negative
        std::shared_ptr<function> dec_neg_function = std::make_shared<function>(neg_function_tok);
        dec_neg_function -> set_fqn(l_fqn);
        dec_neg_function -> is_public(true);
        dec_neg_function -> is_builtin(true);
        dec_neg_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_neg_scope = std::make_shared<scope>();
        dec_neg_scope -> set_parent(l_scope);
        dec_neg_function -> set_scope(dec_neg_scope);
        dec_neg_function -> add_param(param_one);
        dec_neg_function -> set_return_type_instance(m_dec_instance);
        std::shared_ptr<decl> neg_function_decl = dec_neg_function;
        l_namespace -> add_declaration(neg_function_decl);

        // decimal addition
        std::shared_ptr<function> dec_add_function = std::make_shared<function>(add_function_tok);
        dec_add_function -> set_fqn(l_fqn);
        dec_add_function -> is_public(true);
        dec_add_function -> is_builtin(true);
        dec_add_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_add_scope = std::make_shared<scope>();
        dec_add_scope -> set_parent(l_scope);
        dec_add_function -> set_scope(dec_add_scope);
        dec_add_function -> add_param(param_one);
        dec_add_function -> add_param(param_two);
        dec_add_function -> set_return_type_instance(m_dec_instance);
        std::shared_ptr<decl> add_function_decl = dec_add_function;
        l_namespace -> add_declaration(add_function_decl);

        // decimal substraction
        std::shared_ptr<function> dec_sub_function = std::make_shared<function>(sub_function_tok);
        dec_sub_function -> set_fqn(l_fqn);
        dec_sub_function -> is_public(true);
        dec_sub_function -> is_builtin(true);
        dec_sub_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_sub_scope = std::make_shared<scope>();
        dec_sub_scope -> set_parent(l_scope);
        dec_sub_function -> set_scope(dec_sub_scope);
        dec_sub_function -> add_param(param_one);
        dec_sub_function -> add_param(param_two);
        dec_sub_function -> set_return_type_instance(m_dec_instance);
        std::shared_ptr<decl> sub_function_decl = dec_sub_function;
        l_namespace -> add_declaration(sub_function_decl);

        // decimal multiplication
        std::shared_ptr<function> dec_mul_function = std::make_shared<function>(mul_function_tok);
        dec_mul_function -> set_fqn(l_fqn);
        dec_mul_function -> is_public(true);
        dec_mul_function -> is_builtin(true);
        dec_mul_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_mul_scope = std::make_shared<scope>();
        dec_mul_scope -> set_parent(l_scope);
        dec_mul_function -> set_scope(dec_mul_scope);
        dec_mul_function -> add_param(param_one);
        dec_mul_function -> add_param(param_two);
        dec_mul_function -> set_return_type_instance(m_dec_instance);
        std::shared_ptr<decl> mul_function_decl = dec_mul_function;
        l_namespace -> add_declaration(mul_function_decl);

        // decimal division
        std::shared_ptr<function> dec_div_function = std::make_shared<function>(div_function_tok);
        dec_div_function -> set_fqn(l_fqn);
        dec_div_function -> is_public(true);
        dec_div_function -> is_builtin(true);
        dec_div_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_div_scope = std::make_shared<scope>();
        dec_div_scope -> set_parent(l_scope);
        dec_div_function -> set_scope(dec_div_scope);
        dec_div_function -> add_param(param_one);
        dec_div_function -> add_param(param_two);
        dec_div_function -> set_return_type_instance(maybe_instance);
        std::shared_ptr<decl> div_function_decl = dec_div_function;
        l_namespace -> add_declaration(div_function_decl);

        // dec cast to string
        // operator version
        token dec_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> dec_cast_function = std::make_shared<function>(dec_cast_tok);
        dec_cast_function -> set_fqn(l_fqn);
        dec_cast_function -> is_public(true);
        dec_cast_function -> is_builtin(true);
        dec_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_cast_scope = std::make_shared<scope>();
        dec_cast_scope -> set_parent(l_scope);
        dec_cast_function -> set_scope(dec_cast_scope);
        dec_cast_function -> add_param(param_one);
        dec_cast_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> cast_function_decl = dec_cast_function;
        l_namespace -> add_declaration(cast_function_decl);
        // functional version
        token dec_string_tok(IDENTIFIER, "string", 0, 0, "__bif__");
        std::shared_ptr<function> dec_string_function = std::make_shared<function>(dec_string_tok);
        dec_string_function -> set_fqn(l_fqn);
        dec_string_function -> is_public(true);
        dec_string_function -> is_builtin(true);
        dec_string_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_string_scope = std::make_shared<scope>();
        dec_string_scope -> set_parent(l_scope);
        dec_string_function -> set_scope(dec_string_scope);
        dec_string_function -> add_param(param_one);
        dec_string_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> string_function_decl = dec_string_function;
        l_namespace -> add_declaration(string_function_decl);

        // __eq__ comparator
        token dec_eq_tok(IDENTIFIER, "__eq__", 0, 0, "__bif__");
        std::shared_ptr<function> dec_eq_function = std::make_shared<function>(dec_eq_tok);
        dec_eq_function -> set_fqn(l_fqn);
        dec_eq_function -> is_public(true);
        dec_eq_function -> is_builtin(true);
        dec_eq_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_eq_scope = std::make_shared<scope>();
        dec_eq_scope -> set_parent(l_scope);
        dec_eq_function -> set_scope(dec_eq_scope);
        dec_eq_function -> add_param(param_one);
        dec_eq_function -> add_param(param_two);
        dec_eq_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> eq_function_decl = dec_eq_function;
        l_namespace -> add_declaration(eq_function_decl);

        // __ne__ comparator
        token dec_ne_tok(IDENTIFIER, "__ne__", 0, 0, "__bif__");
        std::shared_ptr<function> dec_ne_function = std::make_shared<function>(dec_ne_tok);
        dec_ne_function -> set_fqn(l_fqn);
        dec_ne_function -> is_public(true);
        dec_ne_function -> is_builtin(true);
        dec_ne_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_ne_scope = std::make_shared<scope>();
        dec_ne_scope -> set_parent(l_scope);
        dec_ne_function -> set_scope(dec_ne_scope);
        dec_ne_function -> add_param(param_one);
        dec_ne_function -> add_param(param_two);
        dec_ne_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> ne_function_decl = dec_ne_function;
        l_namespace -> add_declaration(ne_function_decl);

        // __gt__ comparator
        token dec_gt_tok(IDENTIFIER, "__gt__", 0, 0, "__bif__");
        std::shared_ptr<function> dec_gt_function = std::make_shared<function>(dec_gt_tok);
        dec_gt_function -> set_fqn(l_fqn);
        dec_gt_function -> is_public(true);
        dec_gt_function -> is_builtin(true);
        dec_gt_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_gt_scope = std::make_shared<scope>();
        dec_gt_scope -> set_parent(l_scope);
        dec_gt_function -> set_scope(dec_gt_scope);
        dec_gt_function -> add_param(param_one);
        dec_gt_function -> add_param(param_two);
        dec_gt_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> gt_function_decl = dec_gt_function;
        l_namespace -> add_declaration(gt_function_decl);

        // __ge__ comparator
        token dec_ge_tok(IDENTIFIER, "__ge__", 0, 0, "__bif__");
        std::shared_ptr<function> dec_ge_function = std::make_shared<function>(dec_ge_tok);
        dec_ge_function -> set_fqn(l_fqn);
        dec_ge_function -> is_public(true);
        dec_ge_function -> is_builtin(true);
        dec_ge_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_ge_scope = std::make_shared<scope>();
        dec_ge_scope -> set_parent(l_scope);
        dec_ge_function -> set_scope(dec_ge_scope);
        dec_ge_function -> add_param(param_one);
        dec_ge_function -> add_param(param_two);
        dec_ge_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> ge_function_decl = dec_ge_function;
        l_namespace -> add_declaration(ge_function_decl);

        // __lt__ comparator
        token dec_lt_tok(IDENTIFIER, "__lt__", 0, 0, "__bif__");
        std::shared_ptr<function> dec_lt_function = std::make_shared<function>(dec_lt_tok);
        dec_lt_function -> set_fqn(l_fqn);
        dec_lt_function -> is_public(true);
        dec_lt_function -> is_builtin(true);
        dec_lt_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_lt_scope = std::make_shared<scope>();
        dec_lt_scope -> set_parent(l_scope);
        dec_lt_function -> set_scope(dec_lt_scope);
        dec_lt_function -> add_param(param_one);
        dec_lt_function -> add_param(param_two);
        dec_lt_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> lt_function_decl = dec_lt_function;
        l_namespace -> add_declaration(lt_function_decl);

        // __le__ comparator
        token dec_le_tok(IDENTIFIER, "__le__", 0, 0, "__bif__");
        std::shared_ptr<function> dec_le_function = std::make_shared<function>(dec_le_tok);
        dec_le_function -> set_fqn(l_fqn);
        dec_le_function -> is_public(true);
        dec_le_function -> is_builtin(true);
        dec_le_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> dec_le_scope = std::make_shared<scope>();
        dec_le_scope -> set_parent(l_scope);
        dec_le_function -> set_scope(dec_le_scope);
        dec_le_function -> add_param(param_one);
        dec_le_function -> add_param(param_two);
        dec_le_function -> set_return_type_instance(bool_instance);
        std::shared_ptr<decl> le_function_decl = dec_le_function;
        l_namespace -> add_declaration(le_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_dec_prog.add_declaration(namespace_decl);

        return m_dec_prog;
    }
}
