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
#include <vector>
#include <string>
#include <tuple>

#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects the token with type source information
     */
    call_expression::call_expression(token& tok) : m_tok(tok), m_namespace("*"), m_name(tok.get_lexeme()), m_expr_type(FUNCTION_CALL_EXPR), m_type_instance_from_parser(false) {
    }

    /**
     * copy constructor
     */
    call_expression::call_expression(const std::shared_ptr<call_expression>& call_expr) : m_tok(call_expr -> get_token()), m_namespace(call_expr -> get_namespace()), m_name(call_expr -> get_name()), m_instance(call_expr -> get_type_instance()), m_expr_type(call_expr -> get_expression_type()), m_type_instance_from_parser(call_expr -> type_instance_from_parser()), m_return_type(call_expr -> get_return_type_instance()), m_callee(call_expr -> get_callee()) {
        m_specializations = call_expr -> get_specializations();
        std::vector<std::pair<token, std::shared_ptr<expr> > >& arguments = call_expr -> get_arguments();
        for(const auto& argument : arguments)
            m_arguments.emplace_back(argument.first, argument.second -> copy());
    }

    /**
     * assignment copy operator
     */
    call_expression& call_expression::operator=(const std::shared_ptr<call_expression>& call_expr) {
        m_tok = call_expr -> get_token();
        m_namespace = call_expr -> get_namespace();
        m_name = call_expr -> get_name();
        m_instance = call_expr -> get_type_instance();
        m_specializations = call_expr -> get_specializations();
        m_expr_type = call_expr -> get_expression_type();
        m_type_instance_from_parser = call_expr -> type_instance_from_parser();
        std::vector<std::pair<token, std::shared_ptr<expr> > >& arguments = call_expr -> get_arguments();
        for(const auto& argument : arguments)
            m_arguments.emplace_back(argument.first, argument.second -> copy());
        m_return_type = call_expr -> get_return_type_instance();
        m_callee = call_expr -> get_callee();
        return * this;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& call_expression::get_token() const {
        return m_tok;
    }

    /**
     * set_namespace
     * set the namespace where this call expression was found
     */
    void call_expression::set_namespace(const std::string& namespace_name) {
        m_namespace = namespace_name;
    }

    /**
     * get_namespace
     * returns the namespace where this call expression was found
     */
    const std::string& call_expression::get_namespace() const {
        return m_namespace;
    }

    /**
     * get_name
     * returns the name of the function to call
     */
    const std::string& call_expression::get_name() const {
        return m_name;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression
     */
    void call_expression::set_type_instance(type_instance& instance) {
        m_instance = instance;
    }

    /**
     * set_type_instance
     * sets the type instance of this expression with the possibility of specifying
     * if the parser (true case) set the type instance or the inference engine (false case)
     */
    void call_expression::set_type_instance(type_instance& instance, bool from_parser) {
        m_instance = instance;
        m_type_instance_from_parser = from_parser;
    }

    /**
     * get_type_instance
     * returns the type instance of this expression
     */
    type_instance& call_expression::get_type_instance() {
        return m_instance;
    }

    /**
     * has_type_instance
     * returns true if the type instance is not star
     */
    bool call_expression::has_type_instance() {
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
    bool call_expression::type_instance_from_parser() const {
        return m_type_instance_from_parser;
    }

    /*
     * set_expression_type
     * sets the type of expression this is
     */
    void call_expression::set_expression_type(call_expression_type expr_type) {
        m_expr_type = expr_type;
    }

    /**
     * get_expression_type
     * returns the type of expression we are dealing
     */
    const call_expression_type& call_expression::get_expression_type() const {
        return m_expr_type;
    }

    /**
     * add_specialization
     * add a type a type instance that corresponds to a constraint
     */
    void call_expression::add_specialization(type_instance& specialization) {
        m_specializations.push_back(specialization);
    }

    /**
     * get_specializations
     * returns a vector of type instances that will be used to specialize a function against constraints
     */
    std::vector<type_instance>& call_expression::get_specializations() {
        return m_specializations;
    }

    /**
     * add_argument
     * adds an argument to the function call
     */
    void call_expression::add_argument(token& arg_name, std::shared_ptr<expr>& arg_value) {
        m_arguments.emplace_back(arg_name, arg_value);
    }

    /**
     * get_arguments
     * returns a vector of all the arguments of the function call
     */
    std::vector<std::pair<token, std::shared_ptr<expr> > >& call_expression::get_arguments() {
        return m_arguments;
    }

    /**
     * set_return_type_instance
     * set the expected return type instance of the function that corresponds to this function call
     */
    void call_expression::set_return_type_instance(type_instance& return_type) {
        m_return_type = return_type;
    }

    /**
     * get_return_type_instance
     * returns the expected return type instance of the function that corresponds to this function call
     */
    type_instance& call_expression::get_return_type_instance() {
        return m_return_type;
    }

    /**
     * has_return_type_instance
     * returns true if this call expression has a return type instance specified
     */
    bool call_expression::has_return_type_instance() {
        if(m_return_type.get_name() == "*")
            return false;
        else
            return true;
    }

    /**
     * has_record_syntax
     * returns true if arguments named, false otherwise
     */
    bool call_expression::has_record_syntax() {
        if(m_arguments.size() == 0) {
            return false;
        }
        else {
            std::pair<token, std::shared_ptr<expr> >& first_argument = m_arguments[0];
            // we use the star token as a placeholder in case no argument was given
            // so its presence as argument token indicates that the user didn't provide any
            if(first_argument.first.get_lexeme() == "*")
                return false;
            else
                return true;
        }        
    }

    /**
     * has_underscore
     * returns true if this call expression was passed an underscore expression
     */
    bool call_expression::has_underscore() {
        for(auto& argument : m_arguments) {
            if(argument.second -> is_underscore_expression()) {
                return true;
            }
            else if(argument.second -> is_call_expression()) {
                std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(argument.second);
                if(call_expr -> has_underscore()) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * set_callee
     * sets the name of the constructor or function that this call expression resolves to
     */
    void call_expression::set_callee(const std::string& ns_name, const std::string& name, const std::size_t arity) {
        m_callee = std::make_tuple(ns_name, name, arity);
    }

    /**
     * get_callee
     * returns the name of the constructor or function that this call expression resolves to
     */
    const std::tuple<std::string, std::string, std::size_t>& call_expression::get_callee() {
        return m_callee;
    }
}
