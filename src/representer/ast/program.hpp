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

#ifndef AVALON_AST_PROG_H_
#define AVALON_AST_PROG_H_

#include <memory>
#include <vector>

/* Symtable */
#include "representer/symtable/scope.hpp"
#include "representer/symtable/fqn.hpp"

/* AST */
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/program.hpp"


namespace avalon {
    class program {
    public:
        /**
         * the default contructor expects the scope bound to the program
         */
        program();

        /**
         * set_fqn
         * sets the fqn within which this program lives
         */
        void set_fqn(fqn& l_fqn);

        /**
         * get_fqn
         * returns the fqn within which this program lives
         */
        fqn& get_fqn();

        /**
         * set_scope
         * specify the scope bound to this program
         */
        void set_scope(std::shared_ptr<scope>& l_scope);

        /**
         * get_scope
         * return the scope bound to this program
         */
        std::shared_ptr<scope>& get_scope();

        /**
         * add_declaration
         * a program is made of different types of declarations:
         * - import declarations
         * - namespace declarations
         * 
         * a program sits at the top of the AST and it holds a series of those declarations.
         */
        void add_declaration(std::shared_ptr<decl>& declaration);

        /**
         * get_declarations
         * for futher processing, this function is called to get all the declarations that
         * make up a program.
         */
        std::vector<std::shared_ptr<decl> >& get_declarations();

        /**
         * is_builtin
         * sets and returns a boolean indicating whether this program contains builtin declarations
         */
        void is_builtin(bool is_builtin_);
        bool is_builtin();

    private:
        /*
         * the fqn to this program
         */
        fqn m_fqn;

        /*
         * scope introduced by the program, i.e. global scope
         */
        std::shared_ptr<scope> m_scope;

        /*
         * a vector of declarations representing declarations as they were found
         */
        std::vector<std::shared_ptr<decl> > m_declarations;

        /*
         * a boolean flag indicating whether this program contains builtin declarations so we can import declarations from it directly
         */
        bool m_is_builtin;
    };
}

#endif
