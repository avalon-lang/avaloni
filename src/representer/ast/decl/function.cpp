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

#include <unordered_map>
#include <algorithm>
#include <cstddef>
#include <utility>
#include <memory>
#include <vector>
#include <map>

#include "representer/exceptions/type_error.hpp"
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    static bool instances_collide(std::vector<type_instance>& this_instances, std::vector<type_instance>& that_instances) {
        if(this_instances.size() != that_instances.size())
            return false;

        auto this_it = this_instances.begin(), this_end = this_instances.end();
        auto that_it = that_instances.begin(), that_end = that_instances.end();

        for(; this_it != this_end && that_it != that_end; ++this_it, ++that_it) {
            if(type_instance_strong_compare(* this_it, * that_it) == false) {
                if(type_instance_weak_compare(* this_it, * that_it) == false)
                    return false;

                std::vector<type_instance>& this_params = this_it -> get_params();
                std::vector<type_instance>& that_params = that_it -> get_params();
                if(instances_collide(this_params, that_params) == false)
                    return false;
            }
        }

        return true;
    }

    static bool params_collide(std::vector<std::pair<std::string, std::shared_ptr<variable> > >& this_params, std::vector<std::pair<std::string, std::shared_ptr<variable> > >& that_params) {
        std::vector<type_instance> this_instances;
        std::vector<type_instance> that_instances;

        auto this_it = this_params.begin(), this_end = this_params.end();
        auto that_it = that_params.begin(), that_end = that_params.end();
        for(; this_it != this_end && that_it != that_end; ++this_it, ++that_it) {
            this_instances.push_back(this_it -> second -> get_type_instance());
            that_instances.push_back(that_it -> second -> get_type_instance());
        }

        return instances_collide(this_instances, that_instances);
    }

    /**
     * the constructor expects the token with function information
     */
    function::function(token& tok) : m_name(tok.get_lexeme()), m_old_name(tok.get_lexeme()), m_tok(tok), m_is_valid(UNKNOWN), m_is_public(true), m_is_used(false), m_is_builtin(false), m_terminates(false) {
    }

    /**
     * the constructor expects the token with function information and the validation state
     */
    function::function(token& tok, validation_state is_valid) : m_name(tok.get_lexeme()), m_old_name(tok.get_lexeme()), m_tok(tok), m_is_valid(is_valid), m_is_public(true), m_is_used(false), m_is_builtin(false), m_terminates(false) {    
    }

    /**
     * shallow_copy
     * creates a shallow copy of the given function into this one
     */
    function& function::shallow_copy(function& a_function) {
        m_name = a_function.get_name();
        m_old_name = a_function.get_old_name();
        m_tok = a_function.get_token();
        m_fqn = a_function.get_fqn();
        m_namespace = a_function.get_namespace();
        m_scope = std::make_shared<scope>(*(a_function.get_scope()));
        m_constraints = a_function.get_constraints();        
        m_return_type_instance = a_function.get_return_type_instance();
        block_stmt new_body(a_function.get_body(), m_scope);
        m_body = new_body;
        m_is_valid = UNKNOWN;
        m_is_public = a_function.is_public();
        m_is_used = a_function.is_used();
        m_is_builtin = a_function.is_builtin();
        std::unordered_map<std::string, std::shared_ptr<function> > temp(0);
        m_specializations = temp;
        m_terminates = a_function.terminates();
        // we do a manual copy otherwise the pointer to the variable will be copied
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = a_function.get_params();
        for(auto& param : params)
            m_params.emplace_back(param.first, std::make_shared<variable>(* (param.second)));
        return * this;
    }

    /**
     * set_name
     * updates the name of this function
     */
    void function::set_name(const std::string& name) {
        m_name = name;
    }

    /**
     * get_old_name
     * returns the old name of the function
     */
    const std::string& function::get_old_name() const {
        return m_old_name;
    }

    /**
     * get_name
     * returns the name of the function
     */
    const std::string& function::get_name() const {
        return m_name;
    }

    /**
     * get_token
     * returns a token with function source information
     */
    const token& function::get_token() const {
        return m_tok;
    }

    /**
     * set_fqn
     * sets the fqn where this declaration was found
     */
    void function::set_fqn(fqn& l_fqn) {
        m_fqn = l_fqn;
    }

    /**
     * get_fqn
     * returns the fqn where this declaration was found
     */
    fqn& function::get_fqn() {
        return m_fqn;
    }

    /**
     * set_namespace
     * set the namespace where this declaration was found
     */
    void function::set_namespace(const std::string& namespace_name) {
        m_namespace = namespace_name;
    }

    /**
     * get_namespace
     * returns the namespace where this declaration was found
     */
    const std::string& function::get_namespace() const {
        return m_namespace;
    }

    /**
     * set_scope
     * specify the scope introduced by this function
     */
    void function::set_scope(std::shared_ptr<scope>& l_scope) {
        m_scope = l_scope;
    }

    /**
     * get_scope
     * return the scope introduced by this function
     */
    std::shared_ptr<scope>& function::get_scope() {
        return m_scope;
    }

    /**
     * add_constraint
     * add a constraint to the function
     */
    void function::add_constraint(token& constraint) {
        m_constraints.push_back(constraint);
    }

    /**
     * get_constraints
     * returns the vector of all constraints
     */
    std::vector<token>& function::get_constraints() {
        return m_constraints;
    }

    /**
     * add_param
     * adds a parameter to the function
     */
    void function::add_param(variable& param) {
        m_params.emplace_back(param.get_name(), std::make_shared<variable>(param));
    }

    /**
     * get_params
     * returns a map of parameters indexed by their names
     */
    std::vector<std::pair<std::string, std::shared_ptr<variable> > >& function::get_params() {
        return m_params;
    }

    const std::vector<std::pair<std::string, std::shared_ptr<variable> > >& function::get_params() const {
        return m_params;
    }

    /**
     * set_return_type_instance
     * sets the function return type instance
     */
    void function::set_return_type_instance(type_instance& return_type_instance) {
        m_return_type_instance = return_type_instance;
    }

    /**
     * get_return_type_instance
     * return a function return type instance
     */
    type_instance& function::get_return_type_instance() {
        return m_return_type_instance;
    }

    const type_instance& function::get_return_type_instance() const {
        return m_return_type_instance;
    }

    /**
     * set_body
     * set the body of the function
     */
    void function::set_body(block_stmt& body) {
        m_body = body;
    }

    /**
     * get_body
     * return the body of the function
     */
    block_stmt& function::get_body() {
        return m_body;
    }

    /**
     * collides_with
     * return true if this function can collide with the given function
     */
    bool function::collides_with(function& that) {
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& this_params = get_params();
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& that_params = that.get_params();
        if(this_params.size() != that_params.size())
            return false;

        if(params_collide(this_params, that_params)) {
            if(type_instance_strong_compare(m_return_type_instance, that.get_return_type_instance()))
                return true;
            return false;
        }
        
        return false;
    }

    /**
     * is_used
     * sets and returns a boolean indicating whether this declaration was used anywhere
     * this is useful during code generation to emit messages about unused declarations and to avoid generating dead code
     */
    void function::is_used(bool used) {
        m_is_used = used;
    }

    bool function::is_used() const {
        return m_is_used;
    }

    /**
     * is_builtin
     * sets and returns a boolean indicating whether this function is builtin
     */
    void function::is_builtin(bool builtin) {
        m_is_builtin = builtin;
    }

    bool function::is_builtin() const {
        return m_is_builtin;
    }

    /**
     * add_specialization
     * add a function that was generated from this function
     */
    void function::add_specialization(function& specialization) {
        const std::string& name = specialization.get_name();
        if(m_specializations.count(name) == 0)
            m_specializations.emplace(name, std::make_shared<function>(specialization));
    }

    /**
     * get_specializations
     * returns a map of function specializations that were generated from this function
     */
    std::unordered_map<std::string, std::shared_ptr<function> >& function::get_specializations() {
        return m_specializations;
    }

    /**
     * mangle_function
     * returns a string that's unique to the function being mangled
     */
    std::string mangle_function(const function& fun_decl) {
        const std::string& name = fun_decl.get_name();
        const std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = fun_decl.get_params();
        const type_instance& ret_instance = fun_decl.get_return_type_instance();

        std::string mangled_name = name;
        mangled_name += "(";

        // we work on parameters first
        auto it = params.begin(), end = params.end();
        for(; it != end; ++it) {
            mangled_name += mangle_type_instance(it -> second -> get_type_instance());

            if(it != end && it + 1 != end)
                mangled_name += ", ";
        }

        mangled_name += ")";

        // we then work on the return type
        mangled_name += " -> ";
        mangled_name += mangle_type_instance(ret_instance);
        
        return mangled_name;
    }
}
