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

#ifndef AVALON_SYMTABLE_DTABLE_HPP_
#define AVALON_SYMTABLE_DTABLE_HPP_

#include <unordered_map>
#include <cstddef>
#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <map>


namespace avalon {
    /* forward declarations */
    class type;
    class function;
    class variable;
    class type_instance;

    enum declaration_type {
        TYPE_DECL,
        FUNCTION_DECL,
        VARIABLE_DECL
    };


    class dsymbols {
    public:
        /**
         * the constructor expects nothing
         */
        dsymbols();

        /*** TYPE DECLARATIONS ***/

        /**
         * insert_type
         * given a type declaration, this function adds it into the symbol table
         */
        void insert_type(std::shared_ptr<type>& type_decl);

        /**
         * get_types
         * given a type name, find all the types that match the give type irrespective of arity
         */
        std::vector<std::shared_ptr<type> > get_types(const std::string& name);

        /**
         * get_type
         * given a type instance, we find the type that builds it
         */
        std::shared_ptr<type>& get_type(const type_instance& type_ins);

        /**
         * get_type
         * given a type name and its arity, return the type that match them
         */
        std::shared_ptr<type>& get_type(const std::string& name, std::size_t arity);

        /**
         * type_exists
         * given a type declaration, find if an existing type declaration matches it
         */
        bool type_exists(std::shared_ptr<type>& type_decl);

        /**
         * type_exists
         * given a type instance, we find if there is a corresponding type in the symbol table
         */
        bool type_exists(const type_instance& type_ins);

        /**
         * type_exists
         * given a type name and its arity, find if an existing type declaration match the same
         */
        bool type_exists(const std::string& name, std::size_t arity);

        
        /*** FUNCTION DECLARATIONS ***/

        /**
         * insert_function
         * given a function,
         * this function checks if a similar function exists and throws the following exception:
         * throws "symbol_can_collide" if the new function doesn't exactly match an existing one
         * but we cannot choose at compile between this new function and an existing one.
         */
        void insert_function(std::shared_ptr<function>& function_decl);

        /**
         * get_function
         * if we know that we only have one function with the given name and arity, we call this function
         * if there are many, we raise an exception
         */
        std::shared_ptr<function>& get_function(const std::string& name, const std::size_t arity);

        /**
         * get_functions
         * given a function name and the arity, return a vector of possible functions
         */
        std::vector<std::shared_ptr<function> > get_functions(const std::string& name, size_t arity);

        /**
         * function_exists
         * given a function declaration, find if it already exists in the symbol table
         */
        bool function_exists(std::shared_ptr<function>& function_decl);

        /**
         * function_exists
         * given a function name and its arity, find if there is any function that fit those two criteria
         */
        bool function_exists(const std::string& name, size_t arity);

        /**
         * function_exists
         * given a name, find if there is any function by that in name in the symbol table
         */
        bool function_exists(const std::string& name);


        /*** VARIABLE DECLARATIONS ***/

        /**
         * insert_variable
         * add a new variable declaration to this table
         */
        void insert_variable(std::shared_ptr<variable>& variable_decl);

        /**
         * get_variable
         * given a variable name, return the corresponding variable declaration
         */
        std::shared_ptr<variable>& get_variable(const std::string& name);

        /**
         * variable_exists
         * given a variable declaration, find if it already exists in this table
         */
        bool variable_exists(std::shared_ptr<variable>& variable_decl);

        /**
         * variable_exists
         * given a variable name, find if there is a variabl declaration for it in this table
         */
        bool variable_exists(const std::string& name);

    private:
        /*
         * map of the names of all declarations in the current scope
         * this map is used to prevent types, functions and variables from sharing the same name
         * but also to find out where to get a specific symbol when imported
         */        
        std::map<std::string, declaration_type> m_declarations;

        /*
         * map of all the type declarations in the current scope
         */
        std::map<std::pair<std::string,std::size_t>, std::shared_ptr<type> > m_types;

        /*
         * a map of all functions declared in the program
         */        
        std::multimap<std::pair<std::string,std::size_t>, std::shared_ptr<function> > m_functions;

        /*
         * map of variable names and variable declarations stored in this table
         */
        std::map<std::string, std::shared_ptr<variable> > m_variables;
    };


    class dtable {
    public:
        /**
         * the constructor expects nothing
         */
        dtable();

        /**
         * insert_type
         * given a namespace and a type declaration, this function adds it into the namespace symbol table
         */
        void insert_type(const std::string& ns_name, std::shared_ptr<type>& type_decl);

        /**
         * get_types
         * given a namespace and a type name, find all the types that match the give type name; in the namespace; irrespective of arity
         */
        std::vector<std::shared_ptr<type> > get_types(const std::string& ns_name, const std::string& type_name);

        /**
         * get_type
         * given a namespace and a type instance, we find the type that builds it from the namespace
         */
        std::shared_ptr<type>& get_type(const std::string& ns_name, const type_instance& type_ins);

        /**
         * get_type
         * given a namespace and a type name and its arity, return the type that matches both from the namespace
         */
        std::shared_ptr<type>& get_type(const std::string& ns_name, const std::string& type_name, std::size_t arity);

        /**
         * type_exists
         * given a namespace and a type declaration, find if an existing type declaration matches it within the namespace
         */
        bool type_exists(const std::string& ns_name, std::shared_ptr<type>& type_decl);

        /**
         * type_exists
         * given a namespace and a type instance, we find if there is a corresponding type in the namespace symbol table
         */
        bool type_exists(const std::string& ns_name, const type_instance& type_ins);

        /**
         * type_exists
         * given a namespace and a type name and its arity, find if an existing type declaration matches them within the namespace
         */
        bool type_exists(const std::string& ns_name, const std::string& type_name, std::size_t arity);

        /**
         * type_exists
         * given a type name and its arity, find if an existing type declaration matches them within the namespace
         */
        bool type_exists(const std::string& type_name, std::size_t arity);

        
        /*** FUNCTION DECLARATIONS ***/

        /**
         * insert_function
         * given a namespace and a function declaration, insert the last in the symbol table
         */
        void insert_function(const std::string& ns_name, std::shared_ptr<function>& function_decl);

        /**
         * get_function
         * given a namespace and a function name and its arity, return the function that matches both from the namespace
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
         * insert_variable
         * given a namespace and a variable declaration, add the later to the former
         */
        void insert_variable(const std::string& ns_name, std::shared_ptr<variable>& variable_decl);

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
    
    private:
        /*
         * a map between namespaces and their respective symbol tables
         */
        std::unordered_map<std::string, dsymbols> m_symbols;
    };
}

#endif
