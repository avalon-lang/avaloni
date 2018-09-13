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

#ifndef AVALON_SYMTABLE_SCOPE_HPP_
#define AVALON_SYMTABLE_SCOPE_HPP_

#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <map>

/* Symbol table */
#include "representer/symtable/dtable.hpp"
#include "representer/symtable/ctable.hpp"
#include "representer/symtable/fqn.hpp"


namespace avalon {
    /* forward declarations */
    class type;
    class function;
    class variable;
    class type_instance;
    
    class scope {
    public:
        /**
         * the constructor expects nothing
         */
        scope();

        /**
         * copy constructor
         */
        scope(scope& l_scope);

        /**
         * assignment copy operator
         */
        scope& operator=(scope& l_scope);

        /**
         * add_namespace
         */
        void add_namespace(const std::string& namespace_name);

        /**
         * has_namespace
         * returns true if this scope contains the given namespace
         */
        bool has_namespace(const std::string& namespace_name);

        /**
         * get_namespaces
         * returns a map of all namespaces available in this scope
         */
        std::map<std::string, std::string>& get_namespaces();

        /**
         * get_dtable
         * returns the symbol table of declarations found in this scope
         */
        dtable& get_dtable();

        /**
         * get_ctable
         * returns the symbol table of type constructors found in this scope
         */
        ctable& get_ctable();

        /*** TYPE DECLARATIONS ***/

        /**
         * add_type
         * add a new type declaration into this scope
         */
        void add_type(const std::string& ns_name, std::shared_ptr<type>& type_decl);

        /**
         * get_types
         * given a type name, find all the types that match the give type irrespective of arity
         */
        std::vector<std::shared_ptr<type> > get_types(const std::string& ns_name, const std::string& type_name);

        /**
         * get_type
         * given a type instance, we find the type that builds it
         */
        std::shared_ptr<type>& get_type(const std::string& ns_name, const type_instance& type_ins);

        /**
         * get_type
         * given a type name and its arity, return the type that match them
         */
        std::shared_ptr<type>& get_type(const std::string& ns_name, const std::string& type_name, std::size_t arity);

        /**
         * type_exists
         * given a type declaration, find if an existing type declaration matches it
         */
        bool type_exists(const std::string& ns_name, std::shared_ptr<type>& type_decl);

        /**
         * type_exists
         * given a type instance, we find if there is a corresponding type in the symbol table
         */
        bool type_exists(const std::string& ns_name, const type_instance& type_ins);

        /**
         * type_exists
         * given a type name and its arity, find if an existing type declaration match the same
         */
        bool type_exists(const std::string& ns_name, const std::string& type_name, std::size_t arity);

        /**
         * type_exists
         * given a type name and its arity, find if an existing type declaration match the same in all registered namespaces
         */
        bool type_exists(const std::string& type_name, std::size_t arity);

        /**
         * add_default_constructor
         * add a new default constructor into this symbol table
         */
        void add_default_constructor(const std::string& ns_name, default_constructor& def_cons);

        /**
         * add_record_constructor
         * add a new record constructor into this symbol table
         */
        void add_record_constructor(const std::string& ns_name, record_constructor& rec_cons);

        /**
         * get_default_constructor
         * given a constructor name and arity, return the matching default constructor if it exists
         */
        default_constructor& get_default_constructor(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        /**
         * get_record_constructor
         * given a constructor name and arity, return the matching record constructor if it exists
         */
        record_constructor& get_record_constructor(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        /**
         * default_constructor_exists
         * given a constructor name and its arity, find if an existing default constructor matches the same
         */
        bool default_constructor_exists(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        /**
         * record_constructor_exists
         * given a constructor name and its arity, find if an existing record constructor matches the same
         */
        bool record_constructor_exists(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        
        /*** FUNCTION DECLARATIONS ***/

        /**
         * add_function
         * given a namespace and a function declaration, add the declaration to the scope
         */
        void add_function(const std::string& ns_name, std::shared_ptr<function>& function_decl);

        /**
         * get_function
         * given a function name and its arity, return the function that match them
         */
        std::shared_ptr<function>& get_function(const std::string& ns_name, const std::string& function_name, std::size_t arity);

        /**
         * get_functions
         * given a namespace, a function name and its arity, return all functions in that namespace with the same name and arity
         */
        std::vector<std::shared_ptr<function> > get_functions(const std::string& ns_name, const std::string& function_name, size_t arity);

        /**
         * function_exists
         * given a namespace and a function declaration, find if that function declaration already exists in that namespace
         */
        bool function_exists(const std::string& ns_name, std::shared_ptr<function>& function_decl);

        /**
         * function_exists
         * given a namespace, a function name and its arity, find if we have a function with that name and arity in the current namespace
         */
        bool function_exists(const std::string& ns_name, const std::string& function_name, size_t arity);

        /**
         * function_exists
         * given a namespace and a function name, find if the namespace contains a function with that name
         */
        bool function_exists(const std::string& ns_name, const std::string& function_name);


        /*** VARIABLE DECLARATIONS ***/

        /**
         * add_variable
         * given a namespace and a variable declaration, add the later to the former
         */
        void add_variable(const std::string& ns_name, std::shared_ptr<variable>& variable_decl);

        /**
         * get_variable
         * given a namespace and a variable name, return the corresponding variable declaration if it exists in that namespace
         */
        std::shared_ptr<variable>& get_variable(const std::string& ns_name, const std::string& variable_name);

        /**
         * variable_exists
         * given a namespace and a variable declaration, find if it already exists in this namespace
         */
        bool variable_exists(const std::string& ns_name, std::shared_ptr<variable>& variable_decl);

        /**
         * variable_exists
         * given a namespace and a variable name, find if there is a variabl declaration for it in this namespace
         */
        bool variable_exists(const std::string& ns_name, const std::string& variable_name);

        /**
         * set_parent
         * sets the parent of this scope
         */
        void set_parent(std::shared_ptr<scope>& parent);

        /**
         * get_parent
         * returns the parent of this scope
         */
        std::shared_ptr<scope>& get_parent();

        /**
         * has_parent
         * returns true if this scope has a parent scope
         */
        bool has_parent();

        /**
         * set_start_line
         * sets the line where the scope starts
         */
        void set_start_line(std::size_t start_line);

        /**
         * get_start_line
         * returns the line where the scope starts
         */
        std::size_t get_start_line();

        /**
         * set_end_line
         * sets the line where the scope ends
         */
        void set_end_line(std::size_t end_line);

        /**
         * get_end_line
         * returns the line where the scope ends
         */
        std::size_t get_end_line();

        /**
         * set_origin
         * sets a string representing the origin of the scope
         */
        void set_origin(const std::string& origin);

        /**
         * get_origin
         * returns the string representing the origin of this scope
         */
        const std::string& get_origin();

    private:
        /*
         * a map of namespaces available to this scope
         */
        std::map<std::string, std::string> m_namespaces;

        /*
         * declarations declared within this scope.
         */
        dtable m_dtable;

        /*
         * constructors declared within this scope
         */
        ctable m_ctable;

        /*
         * parent scope
         */
        std::shared_ptr<scope> m_parent;

        /*
         * the start line of the scope
         */
        std::size_t m_start_line;

        /*
         * the end line of the scope
         */
        std::size_t m_end_line;

        /*
         * a string representing the origin of this scope
         * if it is a global scope, this will the fqn of the program
         * if it is from a function, it will be the function name
         * if it is from <if> or <while>, it will be those names
         */
        std::string m_origin;
    };
}

#endif
