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


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_bool::avalon_bool() : m_bool_type(std::make_shared<type>(bool_type_tok, VALID)), m_bool_instance(bool_type_tok, m_bool_type, "*") {
        default_constructor true_cons(true_cons_tok, m_bool_type);
        default_constructor false_cons(false_cons_tok, m_bool_type);
        m_bool_type -> add_constructor(true_cons);
        m_bool_type -> add_constructor(false_cons);
    }

    /**
     * get_type
     * returns the bool type
     */
    std::shared_ptr<type>& avalon_bool::get_type() {
        return m_bool_type;
    }

    /**
     * get_type_instance
     * returns the bool type instance
     */
    type_instance& avalon_bool::get_type_instance() {
        return m_bool_instance;
    }

    /**
     * get_program
     * returns a program that defines the bool type and all functions that operate on bools
     */
    program& avalon_bool::get_program() {
        // string type
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        type_instance string_instance = avl_string.get_type_instance();

        /* the program FQN */
        fqn l_fqn("__bifqn_bool__", "__bifqn_bool__");
        m_bool_prog.set_fqn(l_fqn);
        m_bool_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_bool_prog.set_scope(l_scope);

        /* import the string program into bool */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import string program
        std::shared_ptr<import> string_import = std::make_shared<import>(import_tok, string_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_string_import = string_import;
        m_bool_prog.add_declaration(final_string_import);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_bool_type);
        std::shared_ptr<decl> type_decl = m_bool_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(m_bool_instance);
        variable param_two(var_two_tok, false);
        param_two.set_type_instance(m_bool_instance);

        // boolean and
        std::shared_ptr<function> bool_and_function = std::make_shared<function>(and_function_tok);
        bool_and_function -> set_fqn(l_fqn);
        bool_and_function -> is_public(true);
        bool_and_function -> is_builtin(true);
        bool_and_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bool_and_scope = std::make_shared<scope>();
        bool_and_scope -> set_parent(l_scope);
        bool_and_function -> set_scope(bool_and_scope);
        bool_and_function -> add_param(param_one);
        bool_and_function -> add_param(param_two);
        bool_and_function -> set_return_type_instance(m_bool_instance);
        std::shared_ptr<decl> and_function_decl = bool_and_function;
        l_namespace -> add_declaration(and_function_decl);

        // boolean or
        std::shared_ptr<function> bool_or_function = std::make_shared<function>(or_function_tok);
        bool_or_function -> set_fqn(l_fqn);
        bool_or_function -> is_public(true);
        bool_or_function -> is_builtin(true);
        bool_or_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bool_or_scope = std::make_shared<scope>();
        bool_or_scope -> set_parent(l_scope);
        bool_or_function -> set_scope(bool_or_scope);
        bool_or_function -> add_param(param_one);
        bool_or_function -> add_param(param_two);
        bool_or_function -> set_return_type_instance(m_bool_instance);
        std::shared_ptr<decl> or_function_decl = bool_or_function;
        l_namespace -> add_declaration(or_function_decl);

        // boolean not
        std::shared_ptr<function> bool_not_function = std::make_shared<function>(not_function_tok);
        bool_not_function -> set_fqn(l_fqn);
        bool_not_function -> is_public(true);
        bool_not_function -> is_builtin(true);
        bool_not_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bool_not_scope = std::make_shared<scope>();
        bool_not_scope -> set_parent(l_scope);
        bool_not_function -> set_scope(bool_not_scope);
        bool_not_function -> add_param(param_one);
        bool_not_function -> set_return_type_instance(m_bool_instance);
        std::shared_ptr<decl> not_function_decl = bool_not_function;
        l_namespace -> add_declaration(not_function_decl);

        // bool cast to string
        // operator version
        token bool_cast_tok(IDENTIFIER, "__cast__", 0, 0, "__bif__");
        std::shared_ptr<function> bool_cast_function = std::make_shared<function>(bool_cast_tok);
        bool_cast_function -> set_fqn(l_fqn);
        bool_cast_function -> is_public(true);
        bool_cast_function -> is_builtin(true);
        bool_cast_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bool_cast_scope = std::make_shared<scope>();
        bool_cast_scope -> set_parent(l_scope);
        bool_cast_function -> set_scope(bool_cast_scope);
        bool_cast_function -> add_param(param_one);
        bool_cast_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> cast_function_decl = bool_cast_function;
        l_namespace -> add_declaration(cast_function_decl);
        // function version
        token bool_string_tok(IDENTIFIER, "string", 0, 0, "__bif__");
        std::shared_ptr<function> bool_string_function = std::make_shared<function>(bool_string_tok);
        bool_string_function -> set_fqn(l_fqn);
        bool_string_function -> is_public(true);
        bool_string_function -> is_builtin(true);
        bool_string_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bool_string_scope = std::make_shared<scope>();
        bool_string_scope -> set_parent(l_scope);
        bool_string_function -> set_scope(bool_string_scope);
        bool_string_function -> add_param(param_one);
        bool_string_function -> set_return_type_instance(string_instance);
        std::shared_ptr<decl> string_function_decl = bool_string_function;
        l_namespace -> add_declaration(string_function_decl);

        // __eq__ comparator
        token bool_eq_tok(IDENTIFIER, "__eq__", 0, 0, "__bif__");
        std::shared_ptr<function> bool_eq_function = std::make_shared<function>(bool_eq_tok);
        bool_eq_function -> set_fqn(l_fqn);
        bool_eq_function -> is_public(true);
        bool_eq_function -> is_builtin(true);
        bool_eq_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bool_eq_scope = std::make_shared<scope>();
        bool_eq_scope -> set_parent(l_scope);
        bool_eq_function -> set_scope(bool_eq_scope);
        bool_eq_function -> add_param(param_one);
        bool_eq_function -> add_param(param_two);
        bool_eq_function -> set_return_type_instance(m_bool_instance);
        std::shared_ptr<decl> eq_function_decl = bool_eq_function;
        l_namespace -> add_declaration(eq_function_decl);

        // __ne__ comparator
        token bool_ne_tok(IDENTIFIER, "__ne__", 0, 0, "__bif__");
        std::shared_ptr<function> bool_ne_function = std::make_shared<function>(bool_ne_tok);
        bool_ne_function -> set_fqn(l_fqn);
        bool_ne_function -> is_public(true);
        bool_ne_function -> is_builtin(true);
        bool_ne_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> bool_ne_scope = std::make_shared<scope>();
        bool_ne_scope -> set_parent(l_scope);
        bool_ne_function -> set_scope(bool_ne_scope);
        bool_ne_function -> add_param(param_one);
        bool_ne_function -> add_param(param_two);
        bool_ne_function -> set_return_type_instance(m_bool_instance);
        std::shared_ptr<decl> ne_function_decl = bool_ne_function;
        l_namespace -> add_declaration(ne_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_bool_prog.add_declaration(namespace_decl);

        return m_bool_prog;
    }
}
