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
/* Declarations */
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/import.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/program.hpp"
/* Expressions */
#include "representer/ast/expr/expr.hpp"

/* Symbol table */
#include "representer/symtable/scope.hpp"
#include "representer/symtable/fqn.hpp"

/* Builtin types */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_int.hpp"

/* Builtin OS interface programs */
#include "representer/builtins/io/avalon_io.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_io::avalon_io() {
    }

    /**
     * get_program
     * returns a program that defines IO functions
     */
    program& avalon_io::get_program() {
        // int type
        avalon_int avl_int;
        program& int_prog = avl_int.get_program();
        type_instance int_instance = avl_int.get_type_instance();

        // string type
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        type_instance string_instance = avl_string.get_type_instance();

        /* the program FQN */
        fqn l_fqn("__bifqn_io__", "__bifqn_io__");
        m_io_prog.set_fqn(l_fqn);
        m_io_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_io_prog.set_scope(l_scope);

        /* import int and string programs into io */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import int program
        std::shared_ptr<import> int_import = std::make_shared<import>(import_tok, int_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_int_import = int_import;
        m_io_prog.add_declaration(final_int_import);
        // import string program
        std::shared_ptr<import> string_import = std::make_shared<import>(import_tok, string_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_string_import = string_import;
        m_io_prog.add_declaration(final_string_import);

        /* create the namespace for the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add function declarations to the namespace */
        // variables
        variable param_one(var_one_tok, false);
        param_one.set_type_instance(string_instance);

        // print
        token print_tok(IDENTIFIER, "__print__", 0, 0, "__bif__");
        std::shared_ptr<function> print_function = std::make_shared<function>(print_tok);
        print_function -> set_fqn(l_fqn);
        print_function -> is_public(true);
        print_function -> is_builtin(true);
        print_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> print_scope = std::make_shared<scope>();
        print_scope -> set_parent(l_scope);
        print_function -> set_scope(print_scope);
        print_function -> add_param(param_one);
        print_function -> set_return_type_instance(int_instance);
        std::shared_ptr<decl> print_function_decl = print_function;
        l_namespace -> add_declaration(print_function_decl);

        // println
        token println_tok(IDENTIFIER, "__println__", 0, 0, "__bif__");
        std::shared_ptr<function> println_function = std::make_shared<function>(println_tok);
        println_function -> set_fqn(l_fqn);
        println_function -> is_public(true);
        println_function -> is_builtin(true);
        println_function -> set_namespace(l_namespace -> get_name());
        std::shared_ptr<scope> println_scope = std::make_shared<scope>();
        println_scope -> set_parent(l_scope);
        println_function -> set_scope(println_scope);
        println_function -> add_param(param_one);
        println_function -> set_return_type_instance(int_instance);
        std::shared_ptr<decl> println_function_decl = println_function;
        l_namespace -> add_declaration(println_function_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_io_prog.add_declaration(namespace_decl);

        return m_io_prog;
    }
}
