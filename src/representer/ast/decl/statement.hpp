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

#ifndef AVALON_AST_DECL_STATEMENT_HPP_
#define AVALON_AST_DECL_STATEMENT_HPP_

#include <memory>

#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/stmt/stmt.hpp"

#include "lexer/token.hpp"


namespace avalon {
    class statement: public decl {
    public:
        /**
         * the constructor expects nothing
         */
        statement();

        /**
         * copy constructor
         */
        statement(const std::shared_ptr<statement>& a_statement, std::shared_ptr<scope>& parent_scope);

        /**
         * set_statement
         * since statements can vary but the AST can only contain declarations
         * we invoke this function to store the actual statement in this declaration.
         */
        void set_statement(std::shared_ptr<stmt>& statement);

        /**
         * get_statement
         * return the statement stored in this declaration
         */
        std::shared_ptr<stmt>& get_statement();

        /**
         * token
         * returns this declaration token
         */
        virtual const token& decl_token() const {
            return star_tok;
        }

        /**
         * is_statement
         * a virtual function needed for casting a declaration into a statement
         */
        virtual bool is_statement() {
            return true;
        }

        /**
         * is_public
         * sets and returns a boolean indicating whether this declaration can be imported
         */
        virtual bool is_public() {
            return false;
        }
        virtual void is_public(bool public_) {
        }

        /**
         * is_reachable
         * sets and returns a boolean indicating whether this statement declaration will be executed
         */
        virtual bool is_reachable() {
            return m_reachable;
        }
        virtual void is_reachable(bool reachable) {
            m_reachable = reachable;
        }
        
        /**
         * terminates
         * sets and returns a boolean that indicates whether this statement declaration can end the normal execution of a program
         */
        virtual bool terminates() {
            return m_terminates;
        }
        virtual void terminates(bool terminates) {
            m_terminates = terminates;
        }

        /**
         * passes
         * sets and returns a boolean indicating whether this statement declaration allows the next declaration to be exeuted
         */
        virtual bool passes() {
            return m_passes;
        }
        virtual void passes(bool passes) {
            m_passes = passes;
        }

    private:
        /**
         * a pointer to the statement stored in this declaration
         */
        std::shared_ptr<stmt> m_statement;

        /**
         * this variable is true if this statement declaration will be executable
         */
        bool m_reachable;

        /**
         * this variable is true if this statement declaration can end the execution of a function
         */
        bool m_terminates;

        /**
         * this variable is true if this statement declaration will pass control to the next declaration
         */
        bool m_passes;
    };
}

#endif
