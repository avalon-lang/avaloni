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

#include <cstddef>
#include <utility>
#include <memory>
#include <string>

/* AST */
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"

/* Symtable */
#include "representer/symtable/scope.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* Exceptions */
#include "representer/exceptions/type_error.hpp"


namespace avalon {
    /**
     * the constructor expects:
     * - the token with source code information, including the variable name
     * - whether the variable is mutable
     */
    variable::variable(token& tok, bool is_mutable) : m_name(tok.get_lexeme()), m_tok(tok), m_is_mutable(is_mutable), m_namespace("*"), m_parent_scope(nullptr), m_value(nullptr), m_is_valid(UNKNOWN), m_check_initializer(true), m_is_public(true), m_is_global(false), m_is_used(false), m_is_reference(false), m_is_temporary(false), m_reachable(false), m_terminates(false) {
    }

    /**
     * the constructor expects:
     * - the token with source code information, including the variable name
     * - whether the variable is mutable
     * - the validation state of the variable
     */
    variable::variable(token& tok, bool is_mutable, validation_state is_valid) : m_name(tok.get_lexeme()), m_tok(tok), m_is_mutable(is_mutable), m_namespace("*"), m_parent_scope(nullptr), m_value(nullptr), m_is_valid(is_valid), m_is_public(true), m_is_global(false), m_is_used(false), m_is_reference(false), m_is_temporary(false), m_reachable(false), m_terminates(false) {        
    }

    /**
     * copy constructor
     */
    variable::variable(const std::shared_ptr<variable>& a_variable) : m_name(a_variable -> get_name()), m_tok(a_variable -> get_token()), m_is_mutable(a_variable -> is_mutable()), m_fqn(a_variable -> get_fqn()), m_namespace(a_variable -> get_namespace()), m_parent_scope(a_variable -> get_scope()), m_type_instance(a_variable -> get_type_instance()), m_value(a_variable -> get_value() -> copy()), m_is_valid(a_variable -> is_valid()), m_check_initializer(a_variable -> check_initializer()), m_is_public(a_variable -> is_public()), m_is_global(a_variable -> is_global()), m_is_used(a_variable -> is_used()), m_is_reference(a_variable -> is_reference()), m_is_temporary(a_variable -> is_temporary()), m_reachable(a_variable -> is_reachable()), m_terminates(a_variable -> terminates()) {
        m_type_instance.copy(a_variable -> get_type_instance());
    }

    variable::variable(variable& a_variable) : m_name(a_variable.get_name()), m_tok(a_variable.get_token()), m_is_mutable(a_variable.is_mutable()), m_fqn(a_variable.get_fqn()), m_namespace(a_variable.get_namespace()), m_parent_scope(a_variable.get_scope()), m_type_instance(a_variable.get_type_instance()), m_value(a_variable.get_value() -> copy()), m_is_valid(a_variable.is_valid()), m_check_initializer(a_variable.check_initializer()), m_is_public(a_variable.is_public()), m_is_global(a_variable.is_global()), m_is_used(a_variable.is_used()), m_is_reference(a_variable.is_reference()), m_is_temporary(a_variable.is_temporary()), m_reachable(a_variable.is_reachable()), m_terminates(a_variable.terminates()) {
        m_type_instance.copy(a_variable.get_type_instance());
    }

    /**
     * copy assignment operator
     */
    variable& variable::operator=(variable& a_variable) {
        m_name = a_variable.get_name();
        m_tok = a_variable.get_token();
        m_is_mutable = a_variable.is_mutable();
        m_fqn = a_variable.get_fqn();
        m_namespace = a_variable.get_namespace();
        m_parent_scope = a_variable.get_scope();
        m_type_instance.copy(a_variable.get_type_instance());
        m_value = a_variable.get_value() -> copy();
        m_is_valid = a_variable.is_valid();
        m_check_initializer = a_variable.check_initializer();
        m_is_public = a_variable.is_public();
        m_is_global = a_variable.is_global();
        m_is_used = a_variable.is_used();
        m_is_reference = a_variable.is_reference();
        m_is_temporary = a_variable.is_temporary();
        m_reachable = a_variable.is_reachable();
        m_terminates = a_variable.terminates();
        return * this;
    }

    variable& variable::operator=(const std::shared_ptr<variable>& a_variable) {
        * this = * a_variable;
        return * this;
    }

    /**
     * set_name
     * updates the name of this variable
     */
    void variable::set_name(const std::string& name) {
        m_name = name;
    }

    /**
     * get_name
     * returns the name of the variable
     */
    const std::string& variable::get_name() const {
        return m_name;
    }

    /**
     * get_token
     * returns a token with the variable source information
     */
    const token& variable::get_token() const {
        return m_tok;
    }

    /**
     * is_mutable
     * returns true if the variable is mutable, false otherwise
     */
    const bool variable::is_mutable() const {
        return m_is_mutable;
    }

    /**
     * set_fqn
     * sets the fqn where this declaration was found
     */
    void variable::set_fqn(fqn& l_fqn) {
        m_fqn = l_fqn;
    }

    /**
     * get_fqn
     * returns the fqn where this declaration was found
     */
    fqn& variable::get_fqn() {
        return m_fqn;
    }

    /**
     * set_namespace
     * set the namespace where this declaration was found
     */
    void variable::set_namespace(const std::string& namespace_name) {
        m_namespace = namespace_name;
    }

    /**
     * get_namespace
     * returns the namespace where this declaration was found
     */
    const std::string& variable::get_namespace() const {
        return m_namespace;
    }

    /**
     * set_scope
     * set the scope where this variable declaration was found
     */
    void variable::set_scope(std::shared_ptr<scope>& l_scope) {
        m_parent_scope = l_scope;
    }

    /**
     * get_scope
     * return the scope wheret his variable declaration was found
     */
    std::shared_ptr<scope>& variable::get_scope() {
        return m_parent_scope;
    }

    /**
     * set_type_instance
     * sets the type instance for this variable.
     * this function is called either directly while parsing
     * or when the variable is initialized
     */
    void variable::set_type_instance(type_instance& instance) {
        m_type_instance = instance;
    }

    /**
     * get_type_instance
     * returns the type instance of this variable
     * throws a "type_error" exception if no type instance has been set.
     */
    type_instance& variable::get_type_instance() {
        return m_type_instance;
    }

    const type_instance& variable::get_type_instance() const {
        return m_type_instance;
    }

    /**
     * has_type_instance
     * returns true if the variable declaration has a type instance, false otherwise
     */
    bool variable::has_type_instance() {
        if(m_type_instance.is_star())
            return false;
        else
            return true;
    }

    /**
     * set_value
     * called to set the value of the variable
     * throws a "mutabity_error" exception if the variable is not mutable
     * and its value has already been set.
     */
    void variable::set_value(std::shared_ptr<expr>& value) {
        m_value = value;
    }

    /**
     * get_value
     * returns the value contained within the variable
     */
    std::shared_ptr<expr>& variable::get_value() {
        return m_value;
    }

    /**
     * check_initializer
     * returns true if we can skip the variable initializer checking
     */
    void variable::check_initializer(bool check_initializer_) {
        m_check_initializer = check_initializer_;
    }

    bool variable::check_initializer() {
        return m_check_initializer;
    }

    /**
     * set_is_valid
     * updates the variable validation state.
     */
    void variable::set_is_valid(validation_state state) {
        m_is_valid = state;
    }

    /**
     * is_valid
     * given a validation state,
     * this function returns true if the variable has the given validation state.
     */
    bool variable::is_valid(validation_state state) {
        return m_is_valid == state;
    }

    validation_state variable::is_valid() {
        return m_is_valid;
    }

    /**
     * is_global
     * sets and returns a boolean indicating whether this variable is a global variable
     */
    bool variable::is_global() {
        return m_is_global;
    }
    void variable::is_global(bool is_global) {
        m_is_global = is_global;
    }

    /**
     * is_used
     * sets and returns a boolean indicating whether this declaration was used anywhere
     * this is useful during code generation to emit messages about unused declarations and to avoid generating dead code
     */
    void variable::is_used(bool used) {
        m_is_used = used;
    }

    bool variable::is_used() const {
        return m_is_used;
    }

    /**
     * is_reference
     * sets and returns a boolean indicating if this variable is a reference variable
     */
    void variable::is_reference(bool reference) {
        m_is_reference = reference;
    }

    bool variable::is_reference() const {
        return m_is_reference;
    }

    /**
     * is_temporary
     * sets and returns a boolean indicating if this variable is a reference variable pointing to temporary data (local variable)
     */
    void variable::is_temporary(bool temporary) {
        m_is_temporary = temporary;
    }

    bool variable::is_temporary() const {
        return m_is_temporary;
    }

    /**
     * mangle_variable
     * returns a string that represents the variable
     */
    std::string mangle_variable(const variable& var_decl) {
        std::string mangled_name = var_decl.get_name();
        mangled_name += ":";
        mangled_name += mangle_type_instance(var_decl.get_type_instance());
        return mangled_name;
    }
}
