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

#ifndef AVALON_AST_TYPE_HPP_
#define AVALON_AST_TYPE_HPP_

#include <unordered_map>
#include <utility>
#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <new>

#include "lexer/token.hpp"
#include "representer/symtable/fqn.hpp"
#include "representer/ast/decl/decl.hpp"


namespace avalon {
    class type_instance;
    class record_constructor;
    class default_constructor;

    /*
     * types of type instances
     * USER     : user defined type instance
     * TUPLE    : built-in tuple type instance
     * LIST     : built-in list type instance
     * MAP      : built-in map type instance
     */
    enum type_instance_category {
        USER,
        TUPLE,
        LIST,
        MAP
    };


    class type: public decl {
    public:
        /**
         * the constructor expects:
         * - the token with type information
         * - the validation state
         */
        type(token& tok, validation_state is_valid);

        /**
         * set_name
         * updates the name of this type
         */
        void set_name(const std::string& name);

        /**
         * get_name
         * returns the name of the type
         */
        const std::string& get_name() const;

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * set_fqn
         * sets the fqn where this declaration was found
         */
        void set_fqn(fqn& l_fqn);

        /**
         * get_fqn
         * returns the fqn where this declaration was found
         */
        fqn& get_fqn();

        /**
         * set_namespace
         * set the namespace where this declaration was found
         */
        void set_namespace(const std::string& namespace_name);

        /**
         * get_namespace
         * returns the namespace where this declaration was found
         */
        const std::string& get_namespace() const;

        /**
         * add_constructor
         * add a default constructor to this type
         */
        void add_constructor(default_constructor& def_constructor);

        /**
         * add_constructor
         * add a record constructor to this type
         */
        void add_constructor(record_constructor& rec_constructor);

        /**
         * replace_constructor
         * replace a default constructor found on this type
         */
        void replace_constructor(default_constructor& def_constructor);

        /**
         * replace_constructor
         * replace a record constructor found on this type
         */
        void replace_constructor(record_constructor& rec_constructor);

        /**
         * get_default_constructors
         * returns a vector of all default constructors that build this type
         */
        std::vector<default_constructor> get_default_constructors();

        /**
         * get_record_constructors
         * returns a vector of all record constructors that build this type
         */
        std::vector<record_constructor> get_record_constructors();

        /**
         * get_default_constructors
         * returns a vector of default constructors matching the name given
         */
        std::vector<default_constructor> get_default_constructors(const std::string& name);

        /**
         * get_record_constructors
         * returns a vector of record constructors matching the name given
         */
        std::vector<record_constructor> get_record_constructors(const std::string& name);

        /**
         * get_default_constructor
         * given a default constructor name and arity, find it there is one that builds this type
         */
        default_constructor& get_default_constructor(const std::string& name, std::size_t arity);

        /**
         * get_record_constructor
         * given a record constructor name and arity, find it there is one that builds this type
         */
        record_constructor& get_record_constructor(const std::string& name, std::size_t arity);

        /**
         * add_type_param
         * if this is a parametric type, this function allows us to add type parameters
         */
        void add_param(token& type_param);
        void add_param(const token& type_param);

        /**
         * get_params
         * returns a vector with all type parameters if any.
         */
        const std::vector<token>& get_params() const;

        /**
         * set_is_valid
         * updates the type current validation state.
         */
        void set_is_valid(validation_state state);

        /**
         * is_valid
         * given a validation state,
         * this function returns true if the type has the given validation state.
         */
        bool is_valid(validation_state state);

        /**
         * token
         * returns this declaration token
         */
        virtual const token& decl_token() const {
            return m_tok;
        }

        /*
         * is_type
         * returns true as this declaration is indeed a type
         */
        virtual bool is_type() {
            return true;
        }

        /**
         * is_public
         * sets and returns a boolean indicating whether this declaration can be imported
         */
        virtual bool is_public() {
            return m_is_public;
        }
        virtual void is_public(bool public_) {
            m_is_public = public_;
        }

        /**
         * is_used
         * sets and returns a boolean indicating whether this declaration was used anywhere
         * this is useful during code generation to emit messages about unused declarations and to avoid generating dead code
         */
        void is_used(bool used);
        bool is_used() const;

        /**
         * add_specialization
         * add a type that was generated from a complete type instance
         */
        void add_specialization(std::shared_ptr<type>& specialization);

        /**
         * get_specialization
         * given a specialization name, return the corresponding object
         */
        std::shared_ptr<type>& get_specialization(const std::string& name);

        /**
         * get_specializations
         * returns a map of type specializations that were generated from this type
         */
        std::unordered_map<std::string, std::shared_ptr<type> >& get_specializations();

        /**
         * set_builder_instance
         * set the type instance builder that built this type if it is complete
         */
        void set_builder_instance(type_instance& builder_instance);

        /**
         * get_builder_instance
         * returns the type instance builder that built this type if it is complete
         */
        std::shared_ptr<type_instance>& get_builder_instance();

        /**
         * is_reachable
         * sets and returns a boolean indicating whether this type declaration will be executed
         */
        virtual bool is_reachable() {
            return true;
        }
        virtual void is_reachable(bool reachable) {
        }
        
        /**
         * sets and returns a boolean that indicates whether this type declaration will cause a function to return
         */
        virtual bool terminates() {
            return false;
        }
        virtual void terminates(bool terminates) {
        }

        /**
         * passes
         * sets and returns a boolean indicating whether this type declaration allows the next declaration to be exeuted
         */
        virtual bool passes() {
            return true;
        }
        virtual void passes(bool passes) {            
        }

    private:
        /*
         * the name of the type
         */
        std::string m_name;

        /*
         * the token with source file information about the type
         */
        token m_tok;

        /*
         * the fqn of the compilation unit where this declaration was found
         */
        fqn m_fqn;

        /*
         * the name of the namespace where this declaration was found
         */
        std::string m_namespace;

        /*
         * map of all default constructors that build this type
         */
        std::map<std::pair<std::string,std::size_t>, default_constructor> m_def_constructors;

        /*
         * map of all record constructors that build this type
         */
        std::map<std::pair<std::string,std::size_t>, record_constructor> m_rec_constructors;

        /*
         * vector of type parameters if this type was parametrized
         */
        std::vector<token> m_params;

        /*
         * the current validation state of the type
         */
        validation_state m_is_valid;

        /*
         * whether this type is available for public use outside of the namespace where it was declared
         * true if it is, false otherwise
         */
        bool m_is_public;

        /*
         * whether this type has been used for any expression
         */
        bool m_is_used;

        /*
         * map of types generated from this one
         */
        std::unordered_map<std::string, std::shared_ptr<type> > m_specializations;

        /*
         * builder type instance
         */
        std::shared_ptr<type_instance> m_builder_instance;
    };
    
    /**
     * operator==
     * compares two types for equality
     */
    inline bool operator==(const type& lhs, const type& rhs) {
        if (lhs.get_name() == rhs.get_name() && lhs.get_params().size() == rhs.get_params().size())
            return true;
        else
            return false;
    }
    
    /**
     * operator!=
     * compares two types for inequality
     */
    inline bool operator!=(const type& lhs, const type& rhs) {
        return !(lhs == rhs);
    }

    /**
     * mangle_type
     * give a type name and the number of type parameters accepts,
     * we create a string that represents the true type name as
     * the new name takes into account the parameters the type accepts.
     */
    std::string mangle_type(const type& type_decl);
    std::string mangle_type(const std::shared_ptr<type>& type_decl);


    class type_instance {
    public:
        /**
         * the default constructor expects nothing
         */
        type_instance();
        
        /**
         * this constructor expects the token with source code information and the namespace where to find the type
         */
        type_instance(token& tok, const std::string& namespace_name);
        
        /**
         * this constructor expects the token with source code information and the type that buils this instance
         */
        type_instance(token& tok, std::shared_ptr<type>& ty, const std::string& namespace_name);

        /**
         * copy assignment operator
         */
        type_instance& copy(type_instance& instance);

        /**
         * set_name
         * updates the name of this type instance
         */
        void set_name(const std::string& name);

        /**
         * get_name
         * returns the name of the type instance
         */
        const std::string& get_name() const;

        /**
         * get_mangled_name
         * returns the mangled name of this type instance
         * that name reflect the types that the type instance depends on
         * throws a type error if this type instance is either abstract or parametrized
         * this is because callers of this function expect the same
         */
        std::string get_mangled_name() const;

        /**
         * set_token
         * changes the token and the name of this type instance
         */
        void set_token(token tok);

        /**
         * get_token
         * returns a token with type source information
         */
        const token& get_token() const;

        /**
         * set_old_token
         * When an instance is updated from an abstract one to a concrete one
         * this function is called to keep a record of the old token
         */
        void set_old_token(token tok);

        /**
         * get_old_token
         * returns the old token
         */
        const token& get_old_token() const;

        /**
         * set_tag
         * set the tag associated with this type instance
         */
        void set_tag(token& tag);

        /**
         * get_tag
         * returns a token with a string representation of the tag associtated with this type instance
         */
        const token& get_tag() const;

        /**
         * set_category
         * set the category to which this type instance belongs
         */
        void set_category(type_instance_category category);

        /**
         * get_category
         * get the category to which this type instance belongs
         */
        const type_instance_category& get_category() const;

        /**
         * set_namespace
         * set the namespace where to find the type that builds this type instance
         */
        void set_namespace(const std::string& namespace_name);

        /**
         * get_namespace
         * returns the namespace where to find the type declaration that builds this type instance
         */
        const std::string& get_namespace() const;

        /**
         * set_type
         * sets the type that constructs this type instance
         */
        void set_type(std::shared_ptr<type>& ty);

        /**
         * get_type
         * returns the type that corresponds to this type instance
         */
        std::shared_ptr<type>& get_type();
        const std::shared_ptr<type>& get_type() const;

        /**
         * get_generated_type
         * returns the unique type declaration generated by this type instance
         * throws a type_error exception if this type instance is not complete (not abstract and not parametrized)
         * it also throws a type error if no such type has generated yet
         */
        std::shared_ptr<type>& get_generated_type();

        /**
         * is_builtby
         * given a type declaration, return true if that type builds this type instance
         */
        bool is_builtby(std::shared_ptr<type>& ty);

        /**
         * set_count
         * set the number of elements of this type instances in case of lists and maps
         */
        void set_count(std::size_t count);

        /**
         * set_count
         * returns the number of elements allowed for this type instance
         */
        std::size_t get_count();
        std::size_t get_count() const;

        /**
         * has_count
         * sets and returns a boolean indicating whether an element count was specified for lists and maps
         */
        void has_count(bool has_count_);
        bool has_count();

        /**
         * add_param
         * a type instance depends on other type instances if
         * the type constructor that builds it is parametric.
         */
        void add_param(type_instance& param);

        /**
         * get_params
         * returns a vector of type instances this depends on
         */
        std::vector<type_instance>& get_params();
        const std::vector<type_instance>& get_params() const;

        /**
         * is_parametrized
         * if the type instance was declared within a type that depends on parametric types,
         * this function marks it as such.
         */
        void is_parametrized(bool is_parametrized);
        bool is_parametrized() const;
        bool is_parametrized();

        /**
         * is_abstract
         * returns true if this type instance is a parametric type, false otherwise
         */
        void is_abstract(bool is_abstract);
        bool is_abstract();
        bool is_abstract() const;

        /**
         * is_complete
         * returns true if the type instance is neither parametric nor abstract
         */
        bool is_complete();
        bool is_complete() const;

        /**
         * depends_on
         * returns true if this type instance is dependent on the given set of constraints
         */
        bool depends_on(std::vector<token>& constraints);

        /**
         * is_star
         * returns true if this type instance is the dummy type instance
         */
        bool is_star();
        bool is_star() const;

    private:
        /*
         * the name of the type
         */
        std::string m_name;

        /*
         * the token with source file information about the type
         */
        token m_tok;

        /*
         * the old token when this type instance was an abstract instance
         */
        token m_old_tok;

        /*
         * the tag for tagged types such as named tuples
         * e.g.: consider the tuple definitions <(measure : float, unit : string)> and (height : float, unit : string)
         * even though bought have might have type <(float, string)>, we will not consider them to be the same.
         * so we attach extra information to types in order to distinguish them. This allows comparision between 50Kg and 50meters to fail at the type level.
         */
        token m_tag;

        /*
         * the category to which this type instance belongs
         */
        type_instance_category m_category;

        /*
         * the name of the namespace where to find the type builder of this type instance
         */
        std::string m_namespace;

        /*
         * the type the constructs this type instance
         */
        std::shared_ptr<type> m_type;

        /*
         * the number of elements of this type instance
         * if we worked with dependent types, this would have made perfect sense.
         * anyways, this is only applicable to lists and maps since their sizes can carry at runtime
         */
        std::size_t m_count;

        /*
         * if the number of elements was specified by the user
         */
        bool m_has_count;

        /*
         * type instances this type instance depends on
         */
        std::vector<type_instance> m_params;
        
        /*
         * flag indicating whether this type instance depends on an abstract type instance
         */
        bool m_is_parametrized;

    /**
     * a friend function that helps us std::cout type instances
     */
    friend std::ostream& operator<<(std::ostream& os, const type_instance& instance);
    };

    /*
     * a dummy type instance that can be used as the we use the star token
     */
    extern type_instance star_instance;

    /**
     * mangle_type_constructor
     * returns a string representation of a type instance
     */
    std::string mangle_type_instance(const type_instance& instance);

    /**
     * operator<<
     * print the type instance to the current stream
     */
    std::ostream& operator<<(std::ostream& os, const type_instance& instance);

    /**
     * strong_compare
     * does a strong comparison of type instances
     * this means that a concrete type instance cannot be equal to an abstract type instance
     */
    bool type_instance_strong_compare(type_instance& this_instance, type_instance& that_instance);
    bool type_instance_strong_compare(const type_instance& this_instance, const type_instance& that_instance);

    /**
     * weak_compare
     * does a strong comparison of type instances
     * this means that a concrete type instance cann be equal to an abstract type instance
     */
    bool type_instance_weak_compare(type_instance& this_instance, type_instance& that_instance);
    bool type_instance_weak_compare(const type_instance& this_instance, const type_instance& that_instance);

    /**
     * weight
     * calculates the "weight" of one type instance relative to another.
     */
    std::size_t type_instance_weight(type_instance& this_instance, type_instance& that_instance);
    std::size_t type_instance_weight(const type_instance& this_instance, const type_instance& that_instance);


    class default_constructor {
    public:
        /**
         * the constructor expects:
         * - the token with the constructor name
         * - the type that this constructor contructs
         */
        default_constructor(token& tok, std::shared_ptr<type>& ty);

        /**
         * set_type
         * changes the name of the constructor.
         */
        void set_name(const std::string& name);

        /**
         * get_name
         * returns a string with the name of the constructor
         */
        const std::string& get_name() const;

        /**
         * get_mangled_name
         * returns the mangled name of this constructor
         */
        std::string get_mangled_name();

        /**
         * get_token
         * returns the token that contains the name of this constructor.
         */
        const token& get_token() const;

        /**
         * get_type
         * returns the type that this constructor creates
         */
        std::shared_ptr<type>& get_type();

        /**
         * add_param
         * if the contructor is not an nullary constructor,
         * this function allows us to save type instances that the constructor relies on.
         */
        void add_param(type_instance& param);

        /**
         * get_params
         * returns a vector of type instances that make up the constructor parameters.
         */
        std::vector<type_instance>& get_params();

        /**
         * set_is_parametrized
         * if the constructor relies on an unknown type,
         * we use this function to set this flag on.
         */
        void set_is_parametrized(bool is_parametrized);

        /**
         * is_parametrized
         * returns true if the constructor relies on an abstract type.
         */
        bool is_parametrized();

    private:
        /*
         * name of the constructor
         */
        std::string m_name;

        /*
         * token that contains the following relevant information
         */
        token m_tok;

        /*
         * a vector of type instances for nary constructors
         */
        std::vector<type_instance> m_params;

        /*
         * the type that this contructor creates
         */
        std::shared_ptr<type> m_type;

        /*
         * flag set to true if the constructor depends on an abstract type
         */
        bool m_is_parametrized;
    };


    class record_constructor {
    public:
        /**
         * the constructor expects:
         * - the token with the constructor name
         * - the type that this constructor contructs
         */
        record_constructor(token& tok, std::shared_ptr<type>& ty);

        /**
         * set_type
         * changes the name of the constructor.
         */
        void set_name(const std::string& name);

        /**
         * get_name
         * returns a string with the name of the constructor
         */
        const std::string& get_name() const;

        /**
         * get_mangled_name
         * returns the mangled name of this constructor
         */
        std::string get_mangled_name();

        /**
         * get_token
         * returns the token that contains the name of this constructor.
         */
        const token& get_token() const;

        /**
         * get_type
         * returns the type that this constructor creates
         */
        std::shared_ptr<type>& get_type();
        
        /**
         * add_param
         * given the parameter name (as a token) and a type instance,
         * add both to the map of parameters.
         * we use the token as parameter name instead of a string for error reporting purposes.
         * when showing error messages, we need the information the token holds.
         */
        void add_param(token param_name, type_instance& param_type);

        /**
         * get_params
         * returns a map of parameter names and type instances.
         */
        std::map<token,type_instance>& get_params();

        /**
         * get_params_as_vector
         * returns a vector of type instances,
         * essentially transforming a record constructor into a default constructor
         */
        std::vector<type_instance>& get_params_as_vector();

        /**
         * set_is_parametrized
         * if the constructor relies on an unknown type,
         * we use this function to set this flag on.
         */
        void set_is_parametrized(bool is_parametrized);

        /**
         * is_parametrized
         * returns true if the constructor relies on an abstract type.
         */
        bool is_parametrized();

    private:
        /*
         * name of the constructor
         */
        std::string m_name;

        /*
         * token that contains the following relevant information
         */
        token m_tok;

        /*
         * a map between parameter names and their type[s] (instances)
         */
        std::map<token, type_instance> m_params;

        /*
         * a vector of types
         */
        std::vector<type_instance> m_params_vector;

        /*
         * the type that this contructor creates
         */
        std::shared_ptr<type> m_type;

        /*
         * flag set to true if the constructor depends on an abstract type
         */
        bool m_is_parametrized;
    };

    /**
     * mangle_constructor
     * produces a string version of a constructor for use within maps, usually
     */
    std::string mangle_constructor(default_constructor& def_cons);
    std::string mangle_constructor(record_constructor& rec_cons);
    std::string mangle_constructor(const std::string& name, std::vector<type_instance>& params);
    std::string mangle_constructor(const std::string& name, std::vector<type_instance>& params, type_instance& parent_type_instance);
}

#endif
