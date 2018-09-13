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

#ifndef AVALON_AST_DECL_HPP_
#define AVALON_AST_DECL_HPP_

#include <memory>

#include "lexer/token.hpp"


namespace avalon {
    /* Forward declarations */
    class scope;

    /*
     * checking data
     * valid    : the object is in a consistent state
     * invalid  : the object failed type checking
     * unknown  : the object has not been type checked yet
     */
    enum validation_state {
        VALID,
        INVALID,
        VALIDATING,
        UNKNOWN
    };


    class decl : public std::enable_shared_from_this<decl> {
    public:
        /**
         * copy
         * copies a declaration and returns the copied one
         */
        std::shared_ptr<decl> copy(std::shared_ptr<scope>& parent_scope);

        /**
         * token
         * returns this declaration token
         */
        virtual const token& decl_token() const = 0;

        virtual bool is_import() {
            return false;
        }
        virtual bool is_namespace() {
            return false;
        }
        virtual bool is_type() {
            return false;
        }
        virtual bool is_function() {
            return false;
        }
        virtual bool is_variable() {
            return false;
        }
        virtual bool is_statement() {
            return false;
        }

        /**
         * is_public
         * sets and returns a boolean indicating whether this declaration can be imported
         */
        virtual bool is_public() = 0;
        virtual void is_public(bool public_) = 0;

        /**
         * is_reachable
         * sets and returns a boolean indicating whether this declaration will be executed
         */
        virtual bool is_reachable() = 0;
        virtual void is_reachable(bool reachable) = 0;

        /**
         * terminates
         * sets and returns a boolean indicating whether this declaration will cause a function to return
         */
        virtual bool terminates() = 0;
        virtual void terminates(bool terminates) = 0;

        /**
         * passes
         * sets and returns a boolean indicating whether this declaration allows the next declaration to be exeuted
         */
        virtual bool passes() = 0;
        virtual void passes(bool passes) = 0;
    };
}

#endif
