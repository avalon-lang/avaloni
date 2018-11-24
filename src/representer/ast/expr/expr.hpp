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

#ifndef AVALON_AST_EXPR_HPP_
#define AVALON_AST_EXPR_HPP_

#include <memory>

#include "representer/ast/decl/type.hpp"


namespace avalon {
    class expr : public std::enable_shared_from_this<expr> {
    public:
        /**
         * copy
         * creates a copy of an expression and returns it.
         * ideally, this should be the assignment copy operator but I suck at C++ so please educate me when you see this if you inclined to do so.
         */
        std::shared_ptr<expr> copy();

        /**
         * token
         * returns this expression token
         * this is useful because it prevents us from casting each expression just to display error messages
         */
        virtual const token& expr_token() const = 0;

        /**
         * expr_type_instance
         * returns the type instance of this expression, bypassing casting
         */
        virtual type_instance& expr_type_instance() = 0;

        /**
         * has_match_expression
         * returns true if the current expression depends on a match expression
         * this is useful during checking to make sure that variables and function parameters are not initialized with expressions containing match expressions
         */
        virtual bool has_match_expression() = 0;

        /** assignment expression **/
        virtual bool is_assignment_expression() {
            return false;
        }
        /** conditional expression **/
        virtual bool is_conditional_expression() {
            return false;
        }
        /** unary expression **/
        virtual bool is_unary_expression() {
            return false;
        }
        /** binary expression **/
        virtual bool is_binary_expression() {
            return false;
        }
        /** match expression **/
        virtual bool is_match_expression() {
            return false;
        }
        /** cast expression **/
        virtual bool is_cast_expression() {
            return false;
        }
        /** primary expressions **/
        virtual bool is_grouped_expression() {
            return false;
        }
        /* identifier expressions */
        virtual bool is_identifier_expression() {
            return false;
        }
        /* call expressions */
        virtual bool is_call_expression() {
            return false;
        }
        /* tuple expressions */
        virtual bool is_tuple_expression() {
            return false;
        }
        /* list expressions */
        virtual bool is_list_expression() {
            return false;
        }
        /* map expression */
        virtual bool is_map_expression() {
            return false;
        }
        /* literal expressions */
        virtual bool is_literal_expression() {
            return false;
        }
        /* reference expression */
        virtual bool is_reference_expression() {
            return false;
        }
        /* dereference expression */
        virtual bool is_dereference_expression() {
            return false;
        }
        /* undescore expression */
        virtual bool is_underscore_expression() {
            return false;
        }

        /*
         * NOTES:
         * UNARY AND BINARY EXPRESSIONS:
         * you will notice that even though we have many operators,
         * we do not have a one to one mapping between them and expression classes.
         * this is because many operators decay into function calls.
         * therefore, for many operators, we just need to convert them into function call expressions.
         *
         * CAST EXPRESSIONS:
         * cast expressions are a special case because even though they rely on a unary operator
         * and do decay to function calls, we still need to pass them the type to cast to and
         * this is not supposed by the unary expression class because type instances are not expressions.
         *
         * IDENTIFIER EXPRESSIONS:
         * default constructor expressions without arguments and variables expressions all are represented by
         * a single identifier. so during parsing, we have no way to know which is which.
         * we leave it to the checker to disambigue them.
         *
         * CALL EXPRESSIONS:
         * function calls and constructor expressions with arguments all have the same syntax; an identifier
         * followed by an argument list. hence when parsing, we can't dinstinguish them.
         * we group all of them into a single expression and leave it to the checker to disambigue them.
         */
    };
}

#endif
