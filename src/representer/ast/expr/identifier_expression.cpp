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
#include <string>

#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the token with source code information
     */
    identifier_expression::identifier_expression(token& tok) : m_name(tok.get_lexeme()), m_tok(tok), m_namespace("*"), m_type_instance_from_parser(false), m_expr_type(VAR_EXPR) {
    }

    /**
     * copy constructor
     */
    identifier_expression::identifier_expression(const std::shared_ptr<identifier_expression>& id_expr) : m_name(id_expr -> get_name()), m_tok(id_expr -> get_token()), m_namespace(id_expr -> get_namespace()), m_type_instance_from_parser(id_expr -> type_instance_from_parser()), m_instance(id_expr -> get_type_instance()), m_expr_type(id_expr -> get_expression_type()) {        
    }

    /**
     * assignment copy operator
     */
    identifier_expression& identifier_expression::operator=(const std::shared_ptr<identifier_expression>& id_expr) {
        m_name = id_expr -> get_name();
        m_tok = id_expr -> get_token();
        m_namespace = id_expr -> get_namespace();
        m_type_instance_from_parser = id_expr -> type_instance_from_parser();
        m_instance = id_expr -> get_type_instance();
        m_expr_type = id_expr -> get_expression_type();
        return * this;
    }

    /**
     * get_token
     * returns the name of the identifier
     */
    const std::string& identifier_expression::get_name() const {
        return m_name;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& identifier_expression::get_token() const {
        return m_tok;
    }

    /**
     * set_namespace
     * set the namespace where this identifier expression was found
     */
    void identifier_expression::set_namespace(const std::string& namespace_name) {
        m_namespace = namespace_name;
    }

    /**
     * get_namespace
     * returns the namespace where this identifier expression was found
     */
    const std::string& identifier_expression::get_namespace() const {
        return m_namespace;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void identifier_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression with the possibility of specifying
     * if the parser (true case) set the type instance or the inference engine (false case)
     */
    void identifier_expression::set_type_instance(type_instance& instance, bool from_parser) {
        m_instance = instance;
        m_type_instance_from_parser = from_parser;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& identifier_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool identifier_expression::has_type_instance() {
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
    bool identifier_expression::type_instance_from_parser() const {
        return m_type_instance_from_parser;
    }

    /*
     * set_expression_type
     * sets the type of expression this is
     */
    void identifier_expression::set_expression_type(identifier_expression_type expr_type) {
        m_expr_type = expr_type;
    }

    /**
     * get_expression_type
     * returns the type of expression we are dealing
     */
    const identifier_expression_type& identifier_expression::get_expression_type() const {
        return m_expr_type;
    }

    /**
     * set_callee
     * sets the name of the constructor or function that this identifier expression resolves to in case it is a default constructor
     */
    void  identifier_expression::set_callee(const std::string& callee) {
        m_callee = callee;
    }

    /**
     * get_callee
     * returns the name of the constructor or function that this identifier expression resolves to in case it is a default constructor
     */
    const std::string& identifier_expression::get_callee() {
        return m_callee;
    }
}
