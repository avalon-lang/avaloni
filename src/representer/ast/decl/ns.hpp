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

#ifndef AVALON_AST_DECL_NAMESPACE_HPP_
#define AVALON_AST_DECL_NAMESPACE_HPP_

#include <memory>
#include <vector>

#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/symtable/fqn.hpp"
#include "lexer/token.hpp"


namespace avalon {
    class ns: public decl {
    public:
        /**
         * the constructor expects the token with source code information
         */
        ns(token& tok);

        /**
         * copy constructor
         */
        ns(ns& ns_decl);

        /**
         * copy assignment operator
         */
        ns& operator=(ns& ns_decl);

        /**
         * set_name
         * sets the name of the namespace
         */
        void set_name(const std::string& name);

        /**
         * get_name
         * returns the name of the namespace
         */
        const std::string& get_name() const;

        /**
         * set_token
         * sets a token with the namespace source information
         */
        void set_token(const token& tok);

        /**
         * get_token
         * returns a token with the namespace source information
         */
        const token& get_token() const;

        /**
         * set_fqn
         * sets the fqn where this declaration was found
         */
        void set_fqn(fqn& l_fqn);

        /**
         * get_fqn
         * returns the fqn where this declaration was found
         */
        fqn& get_fqn();

        /**
         * set_scope
         * specify the scope of all declarations found in this namespace
         */
        void set_scope(std::shared_ptr<scope>& l_scope);

        /**
         * add_declaration
         * a namespace can contain the following declarations:
         * - type declarations
         * - function declarations
         * - variable declarations
         */
        void add_declaration(std::shared_ptr<decl>& declaration);

        /**
         * get_declarations
         * for futher processing, this function is called to get all the declarations that can be found in the namespace
         */
        std::vector<std::shared_ptr<decl> >& get_declarations();

        /**
         * token
         * returns this declaration token
         */
        virtual const token& decl_token() const {
            return m_tok;
        }

        /*
         * is_namespace
         * returns true as this declaration is indeed a namespace
         */
        virtual bool is_namespace() {
            return true;
        }

        /**
         * is_public
         * sets and returns a boolean indicating whether this declaration can be imported
         */
        virtual bool is_public() {
            return true;
        }
        virtual void is_public(bool public_) {
        }

        /**
         * is_reachable
         * sets and returns a boolean indicating whether this type declaration will be executed
         */
        virtual bool is_reachable() {
            return true;
        }
        virtual void is_reachable(bool reachable) {
        }
        
        /**
         * sets and returns a boolean that indicates whether this type declaration will cause a function to return
         */
        virtual bool terminates() {
            return false;
        }
        virtual void terminates(bool terminates) {
        }

        /**
         * passes
         * sets and returns a boolean indicating whether this type declaration allows the next declaration to be exeuted
         */
        virtual bool passes() {
            return true;
        }
        virtual void passes(bool passes) {            
        }

    private:
        /*
         * the name of the namespace
         */
        std::string m_name;

        /*
         * the token with source file information about the type
         */
        token m_tok;

        /*
         * the fqn of the compilation unit where this declaration was found
         */
        fqn m_fqn;

        /*
         * a vector of declarations found in this namespace
         */
        std::vector<std::shared_ptr<decl> > m_declarations;
    };
}

#endif
