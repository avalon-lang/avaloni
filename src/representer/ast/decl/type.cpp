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

#include <map>
#include <vector>
#include <cstddef>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

#include "lexer/token.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/exceptions/type_error.hpp"
#include "representer/exceptions/constructor_error.hpp"


namespace avalon {

/*
 * type
 */
/**
 * the constructor expects:
 * - the token with type information
 * - the validation state
 */
type::type(token& tok, validation_state is_valid) : m_name(tok.get_lexeme()), m_tok(tok), m_is_valid(is_valid), m_is_public(true), m_is_used(false), m_builder_instance(nullptr) {
}
    /**
     * set_name
     * updates the name of this type
     */
    void type::set_name(const std::string& name) {
        m_name = name;
    }

    /**
     * get_name
     * returns the name of the type
     */
    const std::string& type::get_name() const {
        return m_name;
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& type::get_token() const {
        return m_tok;
    }

    /**
     * set_fqn
     * sets the fqn where this declaration was found
     */
    void type::set_fqn(fqn& l_fqn) {
        m_fqn = l_fqn;
    }

    /**
     * get_fqn
     * returns the fqn where this declaration was found
     */
    fqn& type::get_fqn() {
        return m_fqn;
    }

    /**
     * set_namespace
     * set the namespace where this declaration was found
     */
    void type::set_namespace(const std::string& namespace_name) {
        m_namespace = namespace_name;
    }

    /**
     * get_namespace
     * returns the namespace where this declaration was found
     */
    const std::string& type::get_namespace() const {
        return m_namespace;
    }

    /**
     * add_constructor
     * add a default constructor to this type
     */
    void type::add_constructor(default_constructor& def_constructor) {        
        std::pair<std::string, std::size_t> cons_key(def_constructor.get_name(), def_constructor.get_params().size());
        if(m_def_constructors.count(cons_key) > 0)
            throw type_error("There already exists a default constructor with the given name and arity.");
        else
            m_def_constructors.emplace(cons_key, def_constructor);
    }

    /**
     * add_constructor
     * add a record constructor to this type
     */
    void type::add_constructor(record_constructor& rec_constructor) {
        std::pair<std::string, std::size_t> cons_key(rec_constructor.get_name(), rec_constructor.get_params().size());
        if(m_rec_constructors.count(cons_key) > 0)
            throw type_error("There already exists a record constructor with the given name and arity.");
        else
            m_rec_constructors.emplace(cons_key, rec_constructor);
    }

    /**
     * replace_constructor
     * replace a default constructor found on this type
     */
    void type::replace_constructor(default_constructor& def_constructor) {
        std::pair<std::string, std::size_t> cons_key(def_constructor.get_name(), def_constructor.get_params().size());
        if(m_def_constructors.count(cons_key) > 0)
            m_def_constructors.at(cons_key) = def_constructor;
        else
            throw type_error("No such default constructor belongs to this type.");
    }

    /**
     * replace_constructor
     * replace a record constructor found on this type
     */
    void type::replace_constructor(record_constructor& rec_constructor) {
        std::pair<std::string, std::size_t> cons_key(rec_constructor.get_name(), rec_constructor.get_params().size());
        if(m_rec_constructors.count(cons_key) > 0)
            m_rec_constructors.at(cons_key) = rec_constructor;
        else
            throw type_error("No such record constructor belongs to this type.");
    }

    /**
     * get_default_constructors
     * returns a vector of all default constructors that build this type
     */
    std::vector<default_constructor> type::get_default_constructors() {
        std::vector<default_constructor> def_constructors;
        for(auto& def_constructor : m_def_constructors)
            def_constructors.push_back(def_constructor.second);
        return def_constructors;
    }

    /**
     * get_record_constructors
     * returns a vector of all record constructors that build this type
     */
    std::vector<record_constructor> type::get_record_constructors() {
        std::vector<record_constructor> rec_constructors;
        for(auto& rec_constructor : m_rec_constructors)
            rec_constructors.push_back(rec_constructor.second);
        return rec_constructors;
    }

    /**
     * get_default_constructors
     * returns a vector of default constructors matching the name given
     */
    std::vector<default_constructor> type::get_default_constructors(const std::string& name) {
        std::vector<default_constructor> def_constructors;
        for(auto& def_constructor : m_def_constructors)
            // they key of the map that holds constructors is a pair itself which first element is the constructor name
            if(def_constructor.first.first == name)
                def_constructors.push_back(def_constructor.second);
        return def_constructors;
    }

    /**
     * get_record_constructors
     * returns a vector of record constructors matching the name given
     */
    std::vector<record_constructor> type::get_record_constructors(const std::string& name) {
        std::vector<record_constructor> rec_constructors;
        for(auto& rec_constructor : m_rec_constructors)
            // they key of the map that holds constructors is a pair itself which first element is the constructor name
            if(rec_constructor.first.first == name)
                rec_constructors.push_back(rec_constructor.second);
        return rec_constructors;
    }

    /**
     * get_default_constructor
     * given a default constructor name and arity, find it there is one that builds this type
     */
    default_constructor& type::get_default_constructor(const std::string& name, std::size_t arity) {
        std::pair<std::string, std::size_t> cons_key(name, arity);
        try {
            return m_def_constructors.at(cons_key);
        } catch(std::out_of_range err) {
            throw type_error("This type has no default constructor with the given name and arity.");
        }
    }

    /**
     * get_record_constructor
     * given a record constructor name and arity, find it there is one that builds this type
     */
    record_constructor& type::get_record_constructor(const std::string& name, std::size_t arity) {
        std::pair<std::string, std::size_t> cons_key(name, arity);
        try {
            return m_rec_constructors.at(cons_key);
        } catch(std::out_of_range err) {
            throw type_error("This type has no record constructor with the given name and arity.");
        }
    }

    /**
     * add_type_param
     * if this is a parametric type, this function allows us to add type parameters
     */
    void type::add_param(token& type_param) {
        m_params.push_back(type_param);
    }

    void type::add_param(const token& type_param) {
        m_params.push_back(type_param);
    }

    /**
     * get_type_params
     * returns a vector with all type parameters if any.
     */
    const std::vector<token>& type::get_params() const {
        return m_params;
    }

    /**
     * set_is_valid
     * updates the type current validation state.
     */
    void type::set_is_valid(validation_state state) {
        m_is_valid = state;
    }

    /**
     * is_valid
     * given a validation state,
     * this function returns true if the type has the given validation state.
     */
    bool type::is_valid(validation_state state) {
        return m_is_valid == state;
    }

    /**
     * is_used
     * sets and returns a boolean indicating whether this declaration was used anywhere
     * this is useful during code generation to emit messages about used declarations and to avoid generating dead code
     */
    void type::is_used(bool used) {
        m_is_used = used;
    }

    bool type::is_used() const {
        return m_is_used;
    }

    /**
     * add_specialization
     * add a type that was generated from a complete type instance
     */
    void type::add_specialization(std::shared_ptr<type>& specialization) {
        const std::string& name = specialization -> get_name();
        if(m_specializations.count(name) == 0)
            m_specializations.emplace(name, specialization);
    }

    /**
     * get_specialization
     * given a specialization name, return the corresponding object
     */
    std::shared_ptr<type>& type::get_specialization(const std::string& name) {
        try {
            return m_specializations.at(name);
        } catch(std::out_of_range err) {
            throw type_error("There no specialization by the name <" + name + "> for type <" + mangle_type(* this) + ">.");
        }
    }

    /**
     * get_specializations
     * returns a map of type specializations that were generated from this type
     */
    std::unordered_map<std::string, std::shared_ptr<type> >& type::get_specializations() {
        return m_specializations;
    }

    /**
     * set_builder_instance
     * set the type instance builder that built this type if it is complete
     */
    void type::set_builder_instance(type_instance& builder_instance) {
        m_builder_instance = std::make_shared<type_instance>(builder_instance);
    }

    /**
     * get_builder_instance
     * returns the type instance builder that built this type if it is complete
     */
    std::shared_ptr<type_instance>& type::get_builder_instance() {
        return m_builder_instance;
    }

    /**
     * mangle_type
     * give a type name and the number of type parameters accepts,
     * we create a string that represents the true type name as
     * the new name takes into account the parameters the type accepts.
     */
    std::string mangle_type(const type& type_decl) {
        const std::string& name = type_decl.get_name();
        const std::vector<token>& params =  type_decl.get_params();

        std::string mangled_name;
        if(name == "(") {
            mangled_name += "(";
        }
        else if(name == "[") {
            mangled_name += "[";
        }
        else if(name == "{") {
            mangled_name += "{";
        }
        else {
            mangled_name += name;
            mangled_name += "(";
        }

        for(auto it = params.begin(), end = params.end(); it != end; ++it) {
            mangled_name += it -> get_lexeme();

            if(it != end && it + 1 != end)
                mangled_name += ",";
        }
        
        if(name == "(") {
            mangled_name += ")";
        }
        else if(name == "[") {
            mangled_name += "]";
        }
        else if(name == "{") {
            mangled_name += "}";
        }
        else {
            mangled_name += ")";
        }

        return mangled_name;
    }

    std::string mangle_type(const std::shared_ptr<type>& type_decl) {
        return mangle_type(* type_decl);
    }

/**
 * the default constructor expects nothing
 */
type_instance::type_instance() : m_name(star_tok.get_lexeme()), m_tok(star_tok), m_old_tok(star_tok), m_tag(star_tok), m_category(USER), m_namespace("*"), m_type(nullptr), m_count(0), m_has_count(false), m_is_parametrized(false), m_is_reference(false) {
}

/*
 * type instance
 */
type_instance::type_instance(token& tok, const std::string& namespace_name) : m_name(tok.get_lexeme()), m_tok(tok), m_old_tok(tok), m_tag(star_tok), m_category(USER), m_namespace(namespace_name), m_type(nullptr), m_count(0), m_has_count(false), m_is_parametrized(false), m_is_reference(false) {
}

/**
 * this constructor expects the token with source code information, the type that buils this instance and the namespace where to find that type
 */
type_instance::type_instance(token& tok, std::shared_ptr<type>& ty, const std::string& namespace_name) : m_name(tok.get_lexeme()), m_tok(tok), m_old_tok(tok), m_tag(star_tok), m_category(USER), m_namespace(namespace_name), m_type(ty), m_count(0), m_has_count(false), m_is_parametrized(false), m_is_reference(false) {
}

    /**
     * copy assignment operator
     */
    type_instance& type_instance::copy(type_instance& instance) {
        m_name = instance.get_name();
        m_tok = instance.get_token();
        //m_old_tok = instance.get_old_token();
        m_tag = instance.get_tag();
        m_category = instance.get_category();
        m_namespace = instance.get_namespace();
        m_type = instance.get_type();
        m_is_parametrized = instance.is_parametrized();
        m_is_reference = instance.is_reference();
        /*
        // we avoid copying the count for now since we don't want to screw up generated functions
        // if we copied the count, we'd have to generate a new function to each count
        if(instance.has_count()) {
            m_count = instance.get_count();
            m_has_count = instance.has_count();
        }
        */
        std::vector<type_instance>& params = instance.get_params();
        m_params.clear();
        for(auto& param : params)
            m_params.push_back(param);
        return * this;
    }

    /**
     * set_name
     * updates the name of this type
     */
    void type_instance::set_name(const std::string& name) {
        m_name = name;
    }

    /**
     * get_name
     * returns the name of the type instance
     */
    const std::string& type_instance::get_name() const {
        return m_name;
    }

    /**
     * get_mangled_name
     * returns the mangled name of this type instance
     * that name reflect the types that the type instance depends on
     * throws a type error if this type instance is either abstract or parametrized
     * this is because callers of this function expect the same
     */
    std::string type_instance::get_mangled_name() const {
        // we make sure the type instance has a type constructor
        if(m_type == nullptr)
            throw type_error("Unable to return the mangled name of an abstract type instance.");

        // we make sure the type instance is complete
        if(m_is_parametrized)
            throw type_error("Unable to return the mangled name of an incomplete type instance.");

        return mangle_type_instance(* this);
    }

    /**
     * set_token
     * changes the token and the name of this type instance
     */
    void type_instance::set_token(token tok) {
        m_tok = tok;
        m_name = tok.get_lexeme();
    }

    /**
     * get_token
     * returns a token with type source information
     */
    const token& type_instance::get_token() const {
        return m_tok;
    }

    /**
     * set_old_token
     * When an instance is updated from an abstract one to a concrete one
     * this function is called to keep a record of the old token
     */
    void type_instance::set_old_token(token tok) {
        m_old_tok = tok;
    }

    /**
     * get_old_token
     * returns the old token
     */
    const token& type_instance::get_old_token() const {
        return m_old_tok;
    }

    /**
     * set_tag
     * set the tag associated with this type instance
     */
    void  type_instance::set_tag(token& tag) {
        m_tag = tag;
    }

    /**
     * get_tag
     * returns a token with a string representation of the tag associtated with this type instance
     */
    const token&  type_instance::get_tag() const {
        return m_tag;
    }

    /**
     * set_category
     * set the category to which this type instance belongs
     */
    void type_instance::set_category(type_instance_category category) {
        m_category = category;
    }

    /**
     * get_category
     * get the category to which this type instance belongs
     */
    const type_instance_category& type_instance::get_category() const {
        return m_category;
    }

    /**
     * set_namespace
     * set the namespace where to find the type that builds this type instance
     */
    void type_instance::set_namespace(const std::string& namespace_name) {
        m_namespace = namespace_name;
    }

    /**
     * get_namespace
     * returns the namespace where to find the type declaration that builds this type instance
     */
    const std::string& type_instance::get_namespace() const {
        return m_namespace;
    }

    /**
     * set_type
     * sets the type that constructs this type instance
     */
    void type_instance::set_type(std::shared_ptr<type>& ty) {
        m_type = ty;
    }

    /**
     * get_type
     * returns the type that corresponds to this type instance
     */
    std::shared_ptr<type>& type_instance::get_type() {
        return m_type;
    }

    const std::shared_ptr<type>& type_instance::get_type() const {
        return m_type;
    }

    /**
     * get_generated_type
     * returns the unique type declaration generated by this type instance
     * throws a type_error exception if this type instance is not complete (not abstract and not parametrized)
     * it also throws a type error if no such type has generated yet
     */
    std::shared_ptr<type>& type_instance::get_generated_type() {
        // we make sure the type instance has a type constructor
        if(m_type == nullptr)
            throw type_error("Unable to return the generated type of an abstract type instance.");

        // we make sure the type instance is complete
        if(m_is_parametrized)
            throw type_error("Unable to return the generated type of an incomplete type instance.");

        try {
            std::string mangled_name = get_mangled_name();
            return m_type -> get_specialization(mangled_name);
        } catch(type_error err) {
            throw err;
        }
    }

    /**
     * is_builtby
     * given a type declaration, return true if that type builds this type instance
     */
    bool type_instance::is_builtby(std::shared_ptr<type>& ty) {
        if(ty -> get_name() == m_name && ty -> get_params().size() == m_params.size())
            return true;
        else
            return false;
    }

    /**
     * set_count
     * set the number of elements of this type instances in case of lists and maps
     */
    void type_instance::set_count(std::size_t count) {
        m_count = count;
    }

    /**
     * set_count
     * returns the number of elements allowed for this type instance
     */
    std::size_t type_instance::get_count() {
        return m_count;
    }
    std::size_t type_instance::get_count() const {
        return m_count;
    }

    /**
     * has_count
     * sets and returns a boolean indicating whether an element count was specified for lists and maps
     */
    void type_instance::has_count(bool has_count_) {
        m_has_count = has_count_;
    }

    bool type_instance::has_count() {
        return m_has_count;
    }

    /**
     * add_param
     * a type instance depends on other type instances if
     * the type constructor that builds it is parametric.
     */
    void type_instance::add_param(type_instance& param) {
        m_params.push_back(param);
    }

    /**
     * get_params
     * returns a vector of type instances this depends on
     */
    std::vector<type_instance>& type_instance::get_params() {
        return m_params;
    }

    const std::vector<type_instance>& type_instance::get_params() const {
        return m_params;
    }

    /**
     * set_is_parametrized
     * if the type instance was declared within a type that depends on parametric types,
     * this function marks it as such.
     */
    void type_instance::is_parametrized(bool is_parametrized) {
        m_is_parametrized = is_parametrized;
    }

    bool type_instance::is_parametrized() const {
        return m_is_parametrized;
    }

    bool type_instance::is_parametrized() {
        return m_is_parametrized;
    }

    /**
     * is_reference
     * if the current type instance is a reference to another type, this function marks it as such.
     */
    void type_instance::is_reference(bool is_reference) {
        m_is_reference = is_reference;
    }

    bool type_instance::is_reference() const {
        return m_is_reference;
    }

    bool type_instance::is_reference() {
        return m_is_reference;
    }

    /**
     * has_reference
     * if this type instance is a reference or at least one of its parameter depends on a reference, this function returns true
     */
    bool type_instance::has_reference() const {
        if(m_is_reference)
            return true;

        for(auto& param : m_params)
            if(param.has_reference())
                return true;

        return false;
    }

    bool type_instance::has_reference() {
        if(m_is_reference)
            return true;

        for(auto& param : m_params)
            if(param.has_reference())
                return true;

        return false;
    }

    /**
     * is_abstract
     * returns true if this type instance is a parametric type, false otherwise
     */
    bool type_instance::is_abstract() {
        if(m_type == nullptr)
            return true;
        else
            return false;
    }

    bool type_instance::is_abstract() const {
        if(m_type == nullptr)
            return true;
        else
            return false;
    }

    /**
     * is_complete
     * returns true if the type instance is neither parametric nor abstract
     */
    bool type_instance::is_complete() {
        if(m_is_reference) {
            return !m_is_parametrized;
        }
        else {
            if(m_type == nullptr || m_is_parametrized)
                return false;
            else
                return true;
        }
    }

    bool type_instance::is_complete() const {
        return is_complete();
    }

    /**
     * depends_on
     * returns true if this type instance is dependent on the given set of constraints
     */
    bool type_instance::depends_on(std::vector<token>& constraints) {
        // if the type instance admits no parameter, we check if it matches the constraint
        if(m_params.size() == 0)
            return std::find(constraints.begin(), constraints.end(), m_tok) != constraints.end();

        // if it admits parameters, then one of its parameter might match the constraint
        for(auto& param : m_params) {
            if(std::find(constraints.begin(), constraints.end(), param.get_token()) != constraints.end()) {
                return true;
            }
            else {
                if(param.depends_on(constraints))
                    return true;
            }
        }
        return false;
    }

    /**
     * is_star
     * returns true if this type instance is the dummy type instance
     */
    bool type_instance::is_star() {
        if(m_name == "*")
            return true;
        else
            return false;
    }
    bool type_instance::is_star() const {
        if(m_name == "*")
            return true;
        else
            return false;
    }

    /**
     * mangle_type_constructor
     * returns a string representation of a type instance
     */
    std::string mangle_type_instance(const type_instance& instance) {
        const std::vector<type_instance>& params = instance.get_params();
        std::string mangled_name = "";

        // reference type instances are special cases that need special handling
        if(instance.is_reference()) {
            mangled_name += "ref'";
            mangled_name += mangle_type_instance(params[0]);
            return mangled_name;
        }

        if(instance.is_abstract()) {
            if(instance.is_star())
                mangled_name += "*";
            else
                mangled_name += instance.get_name() + "*";
        }
        else {
            if(instance.get_category() == USER) {
                mangled_name += instance.get_name();                
                if(!params.empty()) {
                    mangled_name += "(";
                    for(auto it = params.begin(), end = params.end(); it != end; ++it) {
                        mangled_name += mangle_type_instance(* it);

                        if(it != end && it + 1 != end)
                            mangled_name += ",";
                    }
                    mangled_name += ")";
                }
            }
            else if(instance.get_category() == TUPLE) {
                mangled_name += "(";
                if(!params.empty()) {
                    for(auto it = params.begin(), end = params.end(); it != end; ++it) {
                        mangled_name += mangle_type_instance(* it);

                        if(it != end && it + 1 != end)
                            mangled_name += ",";
                    }
                }
                mangled_name += ")";
            }
            else if(instance.get_category() == LIST) {
                //const std::size_t count = instance.get_count();
                mangled_name += "[";
                mangled_name += mangle_type_instance(params[0]);
                mangled_name += "]";
            }
            else if(instance.get_category() == MAP) {
                //const std::size_t count = instance.get_count();
                mangled_name += "{";
                mangled_name += mangle_type_instance(params[0]);
                mangled_name += ":";
                mangled_name += mangle_type_instance(params[1]);
                mangled_name += "}";
            }
        }

        return mangled_name;
    }

    /**
     * operator<<
     * print the type instance to the current stream
     */
    std::ostream& operator<<(std::ostream& os, const type_instance& instance) {
        std::string mangled_name = mangle_type_instance(instance);
        os << mangled_name;
        return os;
    }

    /**
     * type_instance_strong_compare
     * does a strong comparison of type instances
     * this means that a concrete type instance cannot be equal to an abstract type instance
     */
    bool type_instance_strong_compare(type_instance& this_instance, type_instance& that_instance) {
        const std::vector<type_instance>& this_params = this_instance.get_params();
        const std::vector<type_instance>& that_params = that_instance.get_params();

        // if the type instances belong to different categories, they are not equal
        if(this_instance.get_category() != that_instance.get_category())
            return false;

        // if both type instances are reference type instances, we compare their only parameters
        if(this_instance.is_reference() == true && that_instance.is_reference() == true) {
            return type_instance_strong_compare(this_params[0], that_params[0]);
        }
        // if one is a reference type instance and the other is not, they are not equal
        else if(this_instance.is_reference() != that_instance.is_reference()) {
            return false;
        }
        
        // get this type instance type and parameters
        std::shared_ptr<type> this_type = nullptr;
        if(this_instance.is_abstract() == false)
            this_type = this_instance.get_type();

        // get the other type instance type and parameters
        std::shared_ptr<type> that_type = nullptr;
        if(that_instance.is_abstract() == false)
            that_type = that_instance.get_type();
        
        // if both type instances are parametric types, then they are equal
        if(this_type == nullptr && that_type == nullptr)
            return true;

        // if this type instance has no type (it is parametric) but the other has, they are not equal
        else if(this_type == nullptr && that_type != nullptr)
            return false;

        // if this type instance has a type but the other doesn't (it is parametric), they are not equal
        else if(this_type != nullptr && that_type == nullptr)
            return false;

        // if both have types, we look at parameters, if any
        else {
            // if both don't admit parameters, we compare types for equality and return that as a result
            if(this_params.size() == 0 && that_params.size() == 0)
                return (* this_type) == (* that_type);

            // if they don't admit they same number of parameters,
            // they were not built from the same type even though they may share the same name
            if(this_params.size() != that_params.size())
                return false;

            // if they both admit they same number of parameters, we compare their types and their parameters
            // if they don't have the same type, they are not equal
            if((* this_type) != (* that_type)) {
                return false;
            }
            // if both type instances are built from the same type, we make sure their parameters are also equal
            else {
                for(auto this_it = this_params.begin(), that_it = that_params.begin(); this_it != this_params.end() && that_it != that_params.end(); ++this_it, ++that_it)
                    // if any of the parameters they depend on are not equal, then both type instances are not equal
                    if(type_instance_strong_compare(const_cast<type_instance&>(* this_it), const_cast<type_instance&>(* that_it)) == false)
                        return false;

                // if we are here, then all parameters are equal
                return true;
            }
        }
    }

    bool type_instance_strong_compare(const type_instance& this_instance, const type_instance& that_instance) {
        return type_instance_strong_compare(const_cast<type_instance&>(this_instance), const_cast<type_instance&>(that_instance));
    }

    /**
     * type_instance_weak_compare
     * does a weak comparison of type instances
     * this means that a concrete type instance cann be equal to an abstract type instance
     */
    bool type_instance_weak_compare(type_instance& this_instance, type_instance& that_instance) {
        const std::vector<type_instance>& this_params = this_instance.get_params();
        const std::vector<type_instance>& that_params = that_instance.get_params();

        // if the type instances belong to different categories, they are not equal
        if(this_instance.get_category() != that_instance.get_category())
            return false;

        // if both type instances are reference type instances, we compare their only parameters
        if(this_instance.is_reference() == true && that_instance.is_reference() == true) {
            return type_instance_weak_compare(this_params[0], that_params[0]);
        }
        // if one is a reference type instance and the other is not, they are not equal
        else if(this_instance.is_reference() != that_instance.is_reference()) {
            return false;
        }
        
        // get this type instance type and parameters
        std::shared_ptr<type> this_type = nullptr;
        if(this_instance.is_abstract() == false)
            this_type = this_instance.get_type();

        // get the other type instance type and parameters
        std::shared_ptr<type> that_type = nullptr;
        if(that_instance.is_abstract() == false)
            that_type = that_instance.get_type();
        
        // if one of the type instances if abstract, then they are equal
        if(this_type == nullptr || that_type == nullptr)
            return true;

        // if both have types, we look at parameters, if any
        else {
            // if both don't admit parameters, we compare types for equality and return that as a result
            if(this_params.size() == 0 && that_params.size() == 0)
                return (* this_type) == (* that_type);

            // if they don't admit they same number of parameters,
            // they were not built from the same type even though they may share the same name
            if(this_params.size() != that_params.size())
                return false;

            // if they both admit they same number of parameters, we compare their types and their parameters
            // if they don't have the same type, they are not equal
            if((* this_type) != (* that_type)) {
                return false;
            }
            // if both type instances are built from the same type, we make sure their parameters are also equal
            else {
                for(auto this_it = this_params.begin(), that_it = that_params.begin(); this_it != this_params.end() && that_it != that_params.end(); ++this_it, ++that_it)
                    // if any of the parameters they depend on are not equal, then both type instances are not equal
                    if(type_instance_weak_compare(const_cast<type_instance&>(* this_it), const_cast<type_instance&>(* that_it)) == false)
                        return false;

                // if we are here, then all parameters are equal
                return true;
            }
        }
    }

    bool type_instance_weak_compare(const type_instance& this_instance, const type_instance& that_instance) {
        return type_instance_weak_compare(const_cast<type_instance&>(this_instance), const_cast<type_instance&>(that_instance));
    }

    /**
     * weight
     * calculates the "weight" of one type instance relative to another.
     */
    std::size_t type_instance_weight(type_instance& this_instance, type_instance& that_instance) {
        const std::vector<type_instance>& this_params = this_instance.get_params();
        const std::vector<type_instance>& that_params = that_instance.get_params();
        std::size_t weight = 0;

        // if both types don't even compare weakly, then their relative weight is zero
        if(type_instance_weak_compare(this_instance, that_instance) == false)
            return weight;

        // if they don't admit parameters, then either they are exactly both the same types or one of them is abstract
        if(this_params.size() == 0 && that_params.size() == 0) {
            if(this_instance.is_abstract() || that_instance.is_abstract())
                weight = 1;
            else
                weight = 2;
        }        
        // if they admit parameters, we take them into account
        else {
            // if they both admit parameters, we recurse on parameters
            if(this_params.size() != 0 && that_params.size() != 0) {
                for(auto this_it = this_params.begin(), that_it = that_params.begin(); this_it != this_params.end() && that_it != that_params.end(); ++this_it, ++that_it)
                    weight = 1 + weight + type_instance_weight(const_cast<type_instance&>(* this_it), const_cast<type_instance&>(* that_it));
            }
            // if one of them admits parameters and the other doesn't, then one of them is either abstract or complete
            // but we do know that both type instances compare weakly so we are sure that one of them is abstract
            else {
                weight += 1;
            }            
        }

        return weight;
    }

    std::size_t type_instance_weight(const type_instance& this_instance, const type_instance& that_instance) {
        return type_instance_weight(const_cast<type_instance&>(this_instance), const_cast<type_instance&>(that_instance));
    }


/**
 * the constructor expects:
 * - the token with the constructor name
 * - the type that this constructor contructs
 */
default_constructor::default_constructor(token& tok, std::shared_ptr<type>& ty) : m_name(tok.get_lexeme()), m_tok(tok), m_type(ty), m_is_parametrized(false) {
}

    /**
     * set_type
     * changes the name of the constructor.
     */
    void default_constructor::set_name(const std::string& name) {
        m_name = name;
    }

    /**
     * get_name
     * returns a string with the name of the constructor
     */
    const std::string& default_constructor::get_name() const {
        return m_name;
    }

    /**
     * get_mangled_name
     * returns the mangled name of this constructor
     */
    std::string default_constructor::get_mangled_name() {
        return mangle_constructor(* this);
    }

    /**
     * get_token
     * returns the token that contains the name of this constructor.
     */
    const token& default_constructor::get_token() const {
        return m_tok;
    }

    /**
     * get_type
     * returns the type that this constructor creates
     */
    std::shared_ptr<type>& default_constructor::get_type() {
        return m_type;
    }

    /**
     * add_param
     * if the contructor is not an nullary constructor,
     * this function allows us to save type instances that the constructor relies on.
     */
    void default_constructor::add_param(type_instance& param) {
        m_params.push_back(param);
    }

    /**
     * get_params
     * returns a vector of type instances that make up the constructor parameters.
     */
    std::vector<type_instance>& default_constructor::get_params() {
        return m_params;
    }

    /**
     * set_is_parametrized
     * if the constructor relies on an unknown type,
     * we use this function to set this flag on.
     */
    void default_constructor::set_is_parametrized(bool is_parametrized) {
        m_is_parametrized = is_parametrized;
    }

    /**
     * is_parametrized
     * returns true if the constructor relies on an abstract type.
     */
    bool default_constructor::is_parametrized() {
        return m_is_parametrized;
    }


/**
 * the constructor expects:
 * - the token with the constructor name
 * - the type that this constructor contructs
 */
record_constructor::record_constructor(token& tok, std::shared_ptr<type>& ty) : m_name(tok.get_lexeme()), m_tok(tok), m_type(ty), m_is_parametrized(false) {
}

    /**
     * set_type
     * changes the name of the constructor.
     */
    void record_constructor::set_name(const std::string& name) {
        m_name = name;
    }

    /**
     * get_name
     * returns a string with the name of the constructor
     */
    const std::string& record_constructor::get_name() const {
        return m_name;
    }

    /**
     * get_token
     * returns the token that contains the name of this constructor.
     */
    const token& record_constructor::get_token() const {
        return m_tok;
    }

    /**
     * get_mangled_name
     * returns the mangled name of this constructor
     */
    std::string record_constructor::get_mangled_name() {
        return mangle_constructor(* this);
    }

    /**
     * get_type
     * returns the type that this constructor creates
     */
    std::shared_ptr<type>& record_constructor::get_type() {
        return m_type;
    }

    /**
     * add_param
     * given the parameter name (as a token) and a type instance,
     * add both to the map of parameters.
     * we use the token as parameter name instead of a string for error reporting purposes.
     * when showing error messages, we need the information the token holds.
     */
    void record_constructor::add_param(token param_name, type_instance& param_type) {
        m_params.emplace(param_name, param_type);
    }

    /**
     * get_params
     * returns a map of parameter names and type instances.
     */
    std::map<token,type_instance>& record_constructor::get_params() {
        return m_params;
    }

    /**
     * get_params_as_vector
     * returns a vector of type instances,
     * essentially transforming a record constructor into a default constructor
     */
    std::vector<type_instance>& record_constructor::get_params_as_vector() {
        if(m_params_vector.empty()) {
            for(auto it = m_params.begin(); it != m_params.end(); ++it) {
                m_params_vector.push_back(it -> second);
            } 
        }

        return m_params_vector;
    }

    /**
     * set_is_parametrized
     * if the constructor relies on an unknown type,
     * we use this function to set this flag on.
     */
    void record_constructor::set_is_parametrized(bool is_parametrized) {
        m_is_parametrized = is_parametrized;
    }

    /**
     * is_parametrized
     * returns true if the constructor relies on an abstract type.
     */
    bool record_constructor::is_parametrized() {
        return m_is_parametrized;
    }


/**
 * mangle_constructor
 * produces a string version of a constructor for use within maps, usually
 */
std::string mangle_constructor(default_constructor& def_cons) {
    const std::string& name = def_cons.get_name();
    std::vector<type_instance>& params = def_cons.get_params();
    std::shared_ptr<type>& type_decl = def_cons.get_type();

    std::string mangled_name = mangle_constructor(name, params);

    if(type_decl != nullptr) {
        std::shared_ptr<type_instance>& builder_instance = type_decl -> get_builder_instance();
        if(builder_instance != nullptr) {
            mangled_name += ":";
            mangled_name += mangle_type_instance(* builder_instance);
        }
    }

    return mangled_name;
}

std::string mangle_constructor(record_constructor& rec_cons) {
    const std::string& name = rec_cons.get_name();
    std::vector<type_instance>& params = rec_cons.get_params_as_vector();
    std::shared_ptr<type>& type_decl = rec_cons.get_type();    

    std::string mangled_name = mangle_constructor(name, params);

    if(type_decl != nullptr) {
        std::shared_ptr<type_instance>& builder_instance = type_decl -> get_builder_instance();
        if(builder_instance != nullptr) {
            mangled_name += ":";
            mangled_name += mangle_type_instance(* builder_instance);
        }
    }

    return mangled_name;
}

std::string mangle_constructor(const std::string& name, std::vector<type_instance>& params) {
    std::string mangled_name = "";
    mangled_name += name;    

    auto it = params.begin(), end = params.end();
    for(; it != end; ++it) {
        if(it == params.begin())
            mangled_name += "(";

        mangled_name += mangle_type_instance(* it);

        if(it != end && it + 1 != end)
            mangled_name += std::string(", ");

        if(it == end || it + 1 == end)
            mangled_name += ")";
    }
    
    return mangled_name;
}

std::string mangle_constructor(const std::string& name, std::vector<type_instance>& params, type_instance& builder_instance) {
    std::string mangled_name = mangle_constructor(name, params);
    mangled_name += ":";
    mangled_name += mangle_type_instance(builder_instance);
    return mangled_name;
}
}
