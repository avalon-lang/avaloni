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

#ifndef AVALON_AST_EXPR_IDENTIFIER_EXPRESSION_HPP_
#define AVALON_AST_EXPR_IDENTIFIER_EXPRESSION_HPP_

#include <memory>
#include <string>

#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    enum identifier_expression_type {
        VAR_EXPR,
        CONSTRUCTOR_EXPR
    };

    class identifier_expression : public expr {
    public:
        /**
         * the constructor expects the token with source code information
         */
        identifier_expression(token& tok);

        /**
         * copy constructor
         */
        identifier_expression(const std::shared_ptr<identifier_expression>& id_expr);

        /**
         * assignment copy operator
         */
        identifier_expression& operator=(const std::shared_ptr<identifier_expression>& id_expr);

        /**
         * get_token
         * returns the name of the identifier
         */
        const std::string& get_name() const;

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * set_namespace
         * set the namespace where this identifier expression was found
         */
        void set_namespace(const std::string& namespace_name);

        /**
         * get_namespace
         * returns the namespace where this identifier expression was found
         */
        const std::string& get_namespace() const;

        /**
         * set_type_instance
         * sets the type instance of this expression
         */
        void set_type_instance(type_instance& instance);

        /**
         * set_type_instance
         * sets the type instance of this expression with the possibility of specifying
         * if the parser (true case) set the type instance or the inference engine (false case)
         */
        void set_type_instance(type_instance& instance, bool from_parser);

        /**
         * get_type_instance
         * returns the type instance of this expression
         */
        type_instance& get_type_instance();

        /**
         * has_type_instance
         * returns true if the type instance is not star
         */
        bool has_type_instance();

        /**
         * type_instance_from_parser
         * returns true if the type instance was set by the parser
         * this will happen when the user specifies a type directly on an expression
         */
        bool type_instance_from_parser() const;

        /*
         * set_expression_type
         * sets the type of expression this is
         */
        void set_expression_type(identifier_expression_type expr_type);

        /**
         * get_expression_type
         * returns the type of expression we are dealing
         */
        const identifier_expression_type& get_expression_type() const;

        /**
         * set_callee
         * sets the name of the constructor or function that this identifier expression resolves to in case it is a default constructor
         */
        void set_callee(const std::string& callee);

        /**
         * get_callee
         * returns the name of the constructor or function that this identifier expression resolves to in case it is a default constructor
         */
        const std::string& get_callee();

        /**
         * token
         * returns this expression token
         * this is useful because it prevents us from casting each expression just to display error messages
         */
        virtual const token& expr_token() const {
            return m_tok;
        }

        /**
         * expr_type_instance
         * returns the type instance of this expression, bypassing casting
         */
        virtual type_instance& expr_type_instance() {
            return m_instance;
        }

        /**
         * has_match_expression
         * returns true if the current expression depends on a match expression
         * this is useful during checking to make sure that variables and function parameters are not initialized with expressions containing match expressions
         */
        virtual bool has_match_expression() {
            return false;
        }
        
        /**
         * is_identifier_expression
         * returns true as this is an identifier expression
         */
        virtual bool is_identifier_expression() {
            return true;
        }

    private:
        /*
         * the name of the identifier expression
         */
        std::string m_name;
        
        /*
         * the token with source code information
         */
        token m_tok;

        /*
         * the namespace where to find this identifier
         */
        std::string m_namespace;

        /*
         * whether the type instance is from the parser
         */
        bool m_type_instance_from_parser;

        /*
         * type instance of this expression
         */
        type_instance m_instance;

        /*
         * the type of call this expression is
         */
        identifier_expression_type m_expr_type;

        /*
         * the name of the constructor or function that this call resolves to
         */
        std::string m_callee;
    };
}

#endif
