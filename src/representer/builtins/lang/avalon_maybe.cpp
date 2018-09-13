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
#include "representer/ast/decl/type.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/program.hpp"

/* Symbol table */
#include "representer/symtable/scope.hpp"
#include "representer/symtable/fqn.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_maybe.hpp"


namespace avalon {
    /**
     * the default constructor expects the dependent type instance parameter
     */
    avalon_maybe::avalon_maybe() : m_maybe_type(std::make_shared<type>(maybe_type_tok, VALID)), m_maybe_instance(maybe_type_tok, m_maybe_type, "*") {
        // add the type parameter
        m_maybe_type -> add_param(type_param_a_tok);

        // set constructors on the type
        // None constructor
        default_constructor none_cons(none_cons_tok, m_maybe_type);
        m_maybe_type -> add_constructor(none_cons);
        // Just constructor
        default_constructor just_cons(just_cons_tok, m_maybe_type);
        type_instance just_param = type_instance(type_param_a_tok, "*");
        just_cons.add_param(just_param);
        m_maybe_type -> add_constructor(just_cons);

        // set the category of the type instance
        m_maybe_instance.set_category(USER);
        m_maybe_instance.is_parametrized(true);
    }

    /**
     * get_type
     * returns the maybe type
     */
    std::shared_ptr<type>& avalon_maybe::get_type() {
        return m_maybe_type;
    }

    /**
     * get_type_instance
     * returns the maybe type instance
     */
    type_instance avalon_maybe::get_type_instance() {
        return m_maybe_instance;
    }

    type_instance avalon_maybe::get_type_instance(type_instance new_param) {
        type_instance maybe_instance(m_maybe_instance);
        maybe_instance.is_parametrized(new_param.is_parametrized());
        new_param.set_old_token(type_param_a_tok);
        maybe_instance.add_param(new_param);
        return maybe_instance;
    }

    /**
     * get_program
     * returns a program that defines the maybe type and all functions that operate on maybes
     */
    program& avalon_maybe::get_program() {
        /* the program FQN */
        fqn l_fqn("__bifqn_maybe__", "__bifqn_maybe__");
        m_maybe_prog.set_fqn(l_fqn);
        m_maybe_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_maybe_prog.set_scope(l_scope);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the scope and the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_maybe_type);
        std::shared_ptr<decl> type_decl = m_maybe_type;
        l_namespace -> add_declaration(type_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_maybe_prog.add_declaration(namespace_decl);

        return m_maybe_prog;
    }
}
