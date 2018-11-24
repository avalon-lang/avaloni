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

#ifndef AVALON_AST_EXPR_MAP_EXPRESSION_HPP_
#define AVALON_AST_EXPR_MAP_EXPRESSION_HPP_

#include <utility>
#include <memory>
#include <vector>

#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    class map_expression : public expr {
    public:
        /**
         * the constructor expects the token with source code information
         */
        map_expression(token& tok);

        /**
         * copy constructor
         */
        map_expression(const std::shared_ptr<map_expression>& map_expr);

        /**
         * assignment copy operator
         */
        map_expression& operator=(const std::shared_ptr<map_expression>& map_expr);

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

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

        /**
         * add_element
         * add a new key/value pair to the map
         */
        void add_element(std::shared_ptr<expr>& key, std::shared_ptr<expr>& value);

        /**
         * get_elements
         * returns a vector of all the elements in the map
         */
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& get_elements();

        /**
         * set_callee
         * sets the function to hash the key
         */
        void set_callee(const std::string& callee);

        /**
         * get_callee
         * returns the function to hash the key
         */
        const std::string& get_callee();

        /**
         * set_comparator_callee
         * sets the function to compare the keys for equality
         */
        void set_comparator_callee(const std::string& callee);

        /**
         * get_comparator_callee
         * returns the function to compare the keys for equality
         */
        const std::string& get_comparator_callee();

        /**
         * get_length
         * returns the length of the map
         */
        std::size_t get_length() const;

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
            for(const auto& element : m_elements) {
                if(element.first -> has_match_expression() || element.second -> has_match_expression())
                    return true;
            }

            return false;
        }
        
        /**
         * is_map_expression
         * returns true as this is an map expression
         */
        virtual bool is_map_expression() {
            return true;
        }

    private:
        /*
         * the token with source code information
         */
        token m_tok;

        /*
         * type instance of this expression
         */
        type_instance m_instance;

        /*
         * whether the type instance is from the parser
         */
        bool m_type_instance_from_parser;

        /*
         * a vector of expressions that make up the map
         */
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > > m_elements;

        /*
         * the hash function responsible for hashing the key
         */        
        std::string m_callee;

        /*
         * the equality comparator function
         */
        std::string m_comp_callee;
    };
}

#endif
