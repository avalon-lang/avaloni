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
#include "representer/builtins/lang/avalon_float.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_float::avalon_float() : m_float_type(std::make_shared<type>(float_type_tok, VALID)), m_float_instance(float_type_tok, m_float_type, "*") {
    }

    /**
     * get_type
     * returns the int type
     */
    std::shared_ptr<type>& avalon_float::get_type() {
        return m_float_type;
    }

    /**
     * get_type_instance
     * returns the int type instance
     */
    type_instance& avalon_float::get_type_instance() {
        return m_float_instance;
    }

    /**
     * get_program
     * returns a program that defines the int type and all functions that operate on ints
     */
    program& avalon_float::get_program() {
        // string type
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        type_instance string_instance = avl_string.get_type_instance();

        /* the program FQN */
        fqn l_fqn("__bifqn_float__", "__bifqn_float__");
        m_float_prog.set_fqn(l_fqn);
        m_float_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_float_prog.set_scope(l_scope);

        /* import the string program into float */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import string program
        std::shared_ptr<import> string_import = std::make_shared<import>(import_tok, string_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_string_import = string_import;
        m_float_prog.add_declaration(final_string_import);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_float_type);
        std::shared_ptr<decl> type_decl = m_float_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(m_float_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(m_float_instance);

        // unary positive
        std::shared_ptr<function> float_pos_function = std::make_shared<function>(pos_function_tok);
        float_pos_function -> set_fqn(l_fqn);
        float_pos_function -> is_public(true);
        float_pos_function -> is_builtin(true);
        float_pos_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_pos_scope = std::make_shared<scope>();
        float_pos_scope -> set_parent(l_scope);
        float_pos_function -> set_scope(float_pos_scope);
        float_pos_function -> add_param(param_one);
        float_pos_function -> set_return_type_instance(m_float_instance);
        std::shared_ptr<decl> pos_function_decl = float_pos_function;
        l_namespace -> add_declaration(pos_function_decl);

        // unary negative
        std::shared_ptr<function> float_neg_function = std::make_shared<function>(neg_function_tok);
        float_neg_function -> set_fqn(l_fqn);
        float_neg_function -> is_public(true);
        float_neg_function -> is_builtin(true);
        float_neg_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_neg_scope = std::make_shared<scope>();
        float_neg_scope -> set_parent(l_scope);
        float_neg_function -> set_scope(float_neg_scope);
        float_neg_function -> add_param(param_one);
        float_neg_function -> set_return_type_instance(m_float_instance);
        std::shared_ptr<decl> neg_function_decl = float_neg_function;
        l_namespace -> add_declaration(neg_function_decl);

        // floating point addition
        std::shared_ptr<function> float_add_function = std::make_shared<function>(add_function_tok);
        float_add_function -> set_fqn(l_fqn);
        float_add_function -> is_public(true);
        float_add_function -> is_builtin(true);
        float_add_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_add_scope = std::make_shared<scope>();
        float_add_scope -> set_parent(l_scope);
        float_add_function -> set_scope(float_add_scope);
        float_add_function -> add_param(param_one);
        float_add_function -> add_param(param_two);
        float_add_function -> set_return_type_instance(m_float_instance);
        std::shared_ptr<decl> add_function_decl = float_add_function;
        l_namespace -> add_declaration(add_function_decl);

        // floating point substraction
        std::shared_ptr<function> float_sub_function = std::make_shared<function>(sub_function_tok);
        float_sub_function -> set_fqn(l_fqn);
        float_neg_function -> is_public(true);
        float_neg_function -> is_builtin(true);
        float_sub_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_sub_scope = std::make_shared<scope>();
        float_sub_scope -> set_parent(l_scope);
        float_sub_function -> set_scope(float_sub_scope);
        float_sub_function -> add_param(param_one);
        float_sub_function -> add_param(param_two);
        float_sub_function -> set_return_type_instance(m_float_instance);
        std::shared_ptr<decl> sub_function_decl = float_sub_function;
        l_namespace -> add_declaration(sub_function_decl);

        // floating point multiplication
        std::shared_ptr<function> float_mul_function = std::make_shared<function>(mul_function_tok);
        float_mul_function -> set_fqn(l_fqn);
        float_mul_function -> is_public(true);
        float_mul_function -> is_builtin(true);
        float_mul_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_mul_scope = std::make_shared<scope>();
        float_mul_scope -> set_parent(l_scope);
        float_mul_function -> set_scope(float_mul_scope);
        float_mul_function -> add_param(param_one);
        float_mul_function -> add_param(param_two);
        float_mul_function -> set_return_type_instance(m_float_instance);
        std::shared_ptr<decl> mul_function_decl = float_mul_function;
        l_namespace -> add_declaration(mul_function_decl);

        // floating point division
        std::shared_ptr<function> float_div_function = std::make_shared<function>(div_function_tok);
        float_div_function -> set_fqn(l_fqn);
        float_div_function -> is_public(true);
        float_div_function -> is_builtin(true);
        float_div_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_div_scope = std::make_shared<scope>();
        float_div_scope -> set_parent(l_scope);
        float_div_function -> set_scope(float_div_scope);
        float_div_function -> add_param(param_one);
        float_div_function -> add_param(param_two);
        float_div_function -> set_return_type_instance(m_float_instance);
        std::shared_ptr<decl> div_function_decl = float_div_function;
        l_namespace -> add_declaration(div_function_decl);

        // float cast to string
        // operator version
        token float_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> float_cast_function = std::make_shared<function>(float_cast_tok);
        float_cast_function -> set_fqn(l_fqn);
        float_cast_function -> is_public(true);
        float_cast_function -> is_builtin(true);
        float_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_cast_scope = std::make_shared<scope>();
        float_cast_scope -> set_parent(l_scope);
        float_cast_function -> set_scope(float_cast_scope);
        float_cast_function -> add_param(param_one);
        float_cast_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> cast_function_decl = float_cast_function;
        l_namespace -> add_declaration(cast_function_decl);
        // functional version
        token float_string_tok(IDENTIFIER, "string", 0, 0, "__bif__");
        std::shared_ptr<function> float_string_function = std::make_shared<function>(float_string_tok);
        float_string_function -> set_fqn(l_fqn);
        float_string_function -> is_public(true);
        float_string_function -> is_builtin(true);
        float_string_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> float_string_scope = std::make_shared<scope>();
        float_string_scope -> set_parent(l_scope);
        float_string_function -> set_scope(float_string_scope);
        float_string_function -> add_param(param_one);
        float_string_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> string_function_decl = float_string_function;
        l_namespace -> add_declaration(string_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_float_prog.add_declaration(namespace_decl);

        return m_float_prog;
    }
}
