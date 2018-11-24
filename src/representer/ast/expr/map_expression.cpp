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

#include <utility>
#include <memory>
#include <string>

#include "representer/ast/expr/map_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the token with source code information
     */
    map_expression::map_expression(token& tok) : m_tok(tok), m_type_instance_from_parser(false), m_callee(""), m_comp_callee("") {
    }

    /**
     * copy constructor
     */
    map_expression::map_expression(const std::shared_ptr<map_expression>& map_expr) : m_tok(map_expr -> get_token()), m_instance(map_expr -> get_type_instance()), m_type_instance_from_parser(map_expr -> type_instance_from_parser()), m_callee(map_expr -> get_callee()), m_comp_callee(map_expr -> get_comparator_callee()) {
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& elements = map_expr -> get_elements();
        for(const auto& element : elements)
            m_elements.emplace_back(element.first -> copy(), element.second -> copy());
    }

    /**
     * assignment copy operator
     */
    map_expression& map_expression::operator=(const std::shared_ptr<map_expression>& map_expr) {
        m_tok = map_expr -> get_token();
        m_instance = map_expr -> get_type_instance();
        m_type_instance_from_parser = map_expr -> type_instance_from_parser();
        m_callee = map_expr -> get_callee();
        m_comp_callee = map_expr -> get_comparator_callee();
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& elements = map_expr -> get_elements();
        for(const auto& element : elements)
            m_elements.emplace_back(element.first -> copy(), element.second -> copy());
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& map_expression::get_token() const {
        return m_tok;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void map_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression with the possibility of specifying
     * if the parser (true case) set the type instance or the inference engine (false case)
     */
    void map_expression::set_type_instance(type_instance& instance, bool from_parser) {
        m_instance = instance;
        m_type_instance_from_parser = from_parser;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& map_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool map_expression::has_type_instance() {
        if(m_instance.get_name() == "*")
            return false;
        else
            return true;
    }

    /**
     * type_instance_from_parser
     * returns true if the type instance was set by the parser
     * this will happen when the user specifies a type directly on an expression
     */
    bool map_expression::type_instance_from_parser() const {
        return m_type_instance_from_parser;
    }

    /**
     * add_element
     * add a new key/value pair to the map
     */
    void map_expression::add_element(std::shared_ptr<expr>& key, std::shared_ptr<expr>& value) {
        m_elements.emplace_back(key, value);
    }

    /**
     * get_elements
     * returns a vector of all the elements in the map
     */
    std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& map_expression::get_elements() {
        return m_elements;
    }

    /**
     * set_callee
     * sets the function to hash the key
     */
    void map_expression::set_callee(const std::string& callee) {
        m_callee = callee;
    }

    /**
     * get_callee
     * returns the function to hash the key
     */
    const std::string& map_expression::get_callee() {
        return m_callee;
    }

    /**
     * set_comparator_callee
     * sets the function to compare the keys for equality
     */
    void map_expression::set_comparator_callee(const std::string& callee) {
        m_comp_callee = callee;
    }

    /**
     * get_comparator_callee
     * returns the function to compare the keys for equality
     */
    const std::string& map_expression::get_comparator_callee() {
        return m_comp_callee;
    }

    /**
     * get_length
     * returns the length of the map
     */
    std::size_t map_expression::get_length() const {
        return m_elements.size();
    }
}
