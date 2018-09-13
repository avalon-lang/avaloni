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
#include "representer/builtins/lang/avalon_list.hpp"


namespace avalon {
    /**
     * the default constructor expects the dependent type instance parameter
     */
    avalon_list::avalon_list(type_instance& param) : m_list_type(std::make_shared<type>(list_type_tok, VALID)), m_list_instance(list_type_tok, m_list_type, "*") {
        m_list_instance.set_category(LIST);
        m_list_instance.add_param(param);

        /*
            NOTE : even though list has two constructors,
            we are not going to bother having them here as they do not interact directly with user code.
            if we intend to later have something like ":info []" as found in GHCI, then we can look into it.
        */
    }

    /**
     * get_type
     * returns the list type
     */
    std::shared_ptr<type>& avalon_list::get_type() {
        return m_list_type;
    }

    /**
     * get_type_instance
     * returns the list type instance
     */
    type_instance avalon_list::get_type_instance() {
        return m_list_instance;
    }

    /**
     * get_program
     * returns a program that defines the list type and all functions that operate on lists
     */
    program& avalon_list::get_program() {
        /* the program FQN */
        fqn l_fqn("__bifqn_list__", "__bifqn_list__");
        m_list_prog.set_fqn(l_fqn);
        m_list_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_list_prog.set_scope(l_scope);

        /* create the namespace to the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add the type to the scope and the namespace */
        l_scope -> add_type(l_namespace -> get_name(), m_list_type);
        std::shared_ptr<decl> type_decl = m_list_type;
        l_namespace -> add_declaration(type_decl);

        /* add function declarations to the namespace */
        // functions to be added later        

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_list_prog.add_declaration(namespace_decl);

        return m_list_prog;
    }
}
