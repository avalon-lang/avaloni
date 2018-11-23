/*
 *  This file is part of the Avalon programming language
 * 
 *  Copyright (c) 2018 Ntwali Bashige Toussastring
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
#include "representer/builtins/lang/avalon_int.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_string::avalon_string() : m_string_type(std::make_shared<type>(string_type_tok, VALID)), m_string_instance(string_type_tok, m_string_type, "*") {
    }

    /**
     * get_type
     * returns the string type
     */
    std::shared_ptr<type>& avalon_string::get_type() {
        return m_string_type;
    }

    /**
     * get_type_instance
     * returns the string type instance
     */
    type_instance& avalon_string::get_type_instance() {
        return m_string_instance;
    }

    /**
     * get_program
     * returns a program that defines the string type and all functions that operate on strings
     */
    program& avalon_string::get_program() {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        type_instance maybe_int_instance = avl_maybe.get_type_instance(int_instance);

        /* the program FQN */
        fqn l_fqn("__bifqn_string__", "__bifqn_string__");
        m_string_prog.set_fqn(l_fqn);
        m_string_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_string_prog.set_scope(l_scope);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the scope and the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_string_type);
        std::shared_ptr<decl> type_decl = m_string_type;
        l_namespace -> add_declaration(type_decl);

        /* manually add declarations to avoid circular dependencies */
        // int type declaration
        std::shared_ptr<type>& int_type = avl_int.get_type();
        l_scope -> add_type("*", int_type);
        // maybe type declaration
        std::shared_ptr<type>& maybe_type = avl_maybe.get_type();
        l_scope -> add_type("*", maybe_type);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(m_string_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(m_string_instance);

        // string reverse
        std::shared_ptr<function> string_neg_function = std::make_shared<function>(neg_function_tok);
        string_neg_function -> set_fqn(l_fqn);
        string_neg_function -> is_public(true);
        string_neg_function -> is_builtin(true);
        string_neg_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> string_neg_scope = std::make_shared<scope>();
        string_neg_scope -> set_parent(l_scope);
        string_neg_function -> set_scope(string_neg_scope);
        string_neg_function -> add_param(param_one);
        string_neg_function -> set_return_type_instance(m_string_instance);
        std::shared_ptr<decl> neg_function_decl = string_neg_function;
        l_namespace -> add_declaration(neg_function_decl);

        // string concatenation
        std::shared_ptr<function> string_concat_function = std::make_shared<function>(add_function_tok);
        string_concat_function -> set_fqn(l_fqn);
        string_concat_function -> is_public(true);
        string_concat_function -> is_builtin(true);
        string_concat_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> string_concat_scope = std::make_shared<scope>();
        string_concat_scope -> set_parent(l_scope);
        string_concat_function -> set_scope(string_concat_scope);
        string_concat_function -> add_param(param_one);
        string_concat_function -> add_param(param_two);
        string_concat_function -> set_return_type_instance(m_string_instance);
        std::shared_ptr<decl> concat_function_decl = string_concat_function;
        l_namespace -> add_declaration(concat_function_decl);

        // string hashing
        token hash_function_tok(IDENTIFIER, "__hash__", 0, 0, "__bif__");
        std::shared_ptr<function> string_hash_function = std::make_shared<function>(hash_function_tok);
        string_hash_function -> set_fqn(l_fqn);
        string_hash_function -> is_public(true);
        string_hash_function -> is_builtin(true);
        string_hash_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> string_hash_scope = std::make_shared<scope>();
        string_hash_scope -> set_parent(l_scope);
        string_hash_function -> set_scope(string_hash_scope);
        string_hash_function -> add_param(param_one);
        string_hash_function -> set_return_type_instance(int_instance);
        std::shared_ptr<decl> hash_function_decl = string_hash_function;
        l_namespace -> add_declaration(hash_function_decl);

        // cast string to int
        // operator version
        token string_int_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> string_int_cast_function = std::make_shared<function>(string_int_cast_tok);
        string_int_cast_function -> set_fqn(l_fqn);
        string_int_cast_function -> is_public(true);
        string_int_cast_function -> is_builtin(true);
        string_int_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> string_int_cast_scope = std::make_shared<scope>();
        string_int_cast_scope -> set_parent(l_scope);
        string_int_cast_function -> set_scope(string_int_cast_scope);
        string_int_cast_function -> add_param(param_one);
        string_int_cast_function -> set_return_type_instance(maybe_int_instance);
        std::shared_ptr<decl> string_cast_function_decl = string_int_cast_function;
        l_namespace -> add_declaration(string_cast_function_decl);
        // functional version
        token string_int_tok(IDENTIFIER, "int", 0, 0, "__bif__");
        std::shared_ptr<function> string_int_function = std::make_shared<function>(string_int_tok);
        string_int_function -> set_fqn(l_fqn);
        string_int_function -> is_public(true);
        string_int_function -> is_builtin(true);
        string_int_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> string_int_scope = std::make_shared<scope>();
        string_int_scope -> set_parent(l_scope);
        string_int_function -> set_scope(string_int_scope);
        string_int_function -> add_param(param_one);
        string_int_function -> set_return_type_instance(maybe_int_instance);
        std::shared_ptr<decl> string_function_decl = string_int_function;
        l_namespace -> add_declaration(string_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_string_prog.add_declaration(namespace_decl);

        return m_string_prog;
    }
}
