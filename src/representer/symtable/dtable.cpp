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
#include <cstdint>
#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "representer/exceptions/symbol_already_declared.hpp"
#include "representer/exceptions/symbol_can_collide.hpp"
#include "representer/exceptions/symbol_not_found.hpp"
#include "representer/exceptions/invalid_symbol.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/symtable/dtable.hpp"
#include "representer/ast/decl/type.hpp"


namespace avalon {
/**
 * the constructor expects nothing
 */
dsymbols::dsymbols() {
}

    /**
     * insert_type
     * given a type declaration, this function inserts it into the symbol table
     */
    void dsymbols::insert_type(std::shared_ptr<type>& type_decl) {
        const std::string& name = type_decl -> get_name();

        // we make sure no other type of declaration has the same name as this type declaration
        try {
            declaration_type decl_type = m_declarations.at(name);
            if(decl_type == VARIABLE_DECL)
                throw symbol_can_collide("There already exists another declaration with the name given to this type.");
        } catch(std::out_of_range err){
        }

        // no other declaration shares the given name, we leave to the type to handle the rest
        std::pair<std::string, std::size_t> type_key(name, type_decl -> get_params().size());
        if(m_types.count(type_key) > 0)
            throw symbol_already_declared("There already exist a locally defined type with the same name and arity.");
        
        m_types.insert(std::make_pair(type_key, type_decl));
        m_declarations.emplace(name, TYPE_DECL);
    }

    /**
     * get_types
     * given a type name, find all the types that match the give type irrespective of arity
     */
    std::vector<std::shared_ptr<type> > dsymbols::get_types(const std::string& name) {
        std::vector<std::shared_ptr<type> > type_decls;
        
        for(auto& ty : m_types) {
            if(ty.first.first == name)
                type_decls.push_back(ty.second);
        }

        return type_decls;
    }

    /**
     * get_type
     * given a type instance, we find the type that builds it
     */
    std::shared_ptr<type>& dsymbols::get_type(const type_instance& type_ins) {
        std::pair<std::string, std::size_t> type_key(type_ins.get_name(), type_ins.get_params().size());
        try {
            return m_types.at(type_key);
        } catch(std::out_of_range err) {
            throw symbol_not_found("There is no type with the given name <" + type_ins.get_name() + "> and arity <" + std::to_string(type_ins.get_params().size()) + ">.");
        }
    }

    /**
     * get_type
     * given a type name and its arity, return the type that match them
     */
    std::shared_ptr<type>& dsymbols::get_type(const std::string& name, std::size_t arity) {
        std::pair<std::string, std::size_t> type_key(name, arity);
        try {
            return m_types.at(type_key);
        } catch(std::out_of_range err) {
            throw symbol_not_found("There is no type with the given name <" + name + "> and arity <" + std::to_string(arity) + ">.");
        }
    }

    /**
     * type_exists
     * given a type declaration, find if an existing type declaration matches it
     */
    bool dsymbols::type_exists(std::shared_ptr<type>& type_decl) {
        std::pair<std::string, std::size_t> type_key(type_decl -> get_name(), type_decl -> get_params().size());
        if(m_types.count(type_key) > 0)
            return true;
        else
            return false;
    }

    /**
     * type_exists
     * given a type instance, we find if there is a corresponding type in the symbol table.
     */
    bool dsymbols::type_exists(const type_instance& type_ins) {
        std::pair<std::string, std::size_t> type_key(type_ins.get_name(), type_ins.get_params().size());
        if(m_types.count(type_key) > 0)
            return true;
        else
            return false;
    }

    /**
     * type_exists
     * given a type name and its arity, find if an existing type declaration match the same
     */
    bool dsymbols::type_exists(const std::string& name, std::size_t arity) {
        std::pair<std::string, std::size_t> type_key(name, arity);
        if(m_types.count(type_key) > 0)
            return true;
        else
            return false;
    }

    /**
     * insert_function
     * given a function,
     * this function checks if a similar function exists and throws the following exception:
     * throws "symbol_can_collide" if the new function doesn't exactly match an existing one
     * but we cannot choose at compile between this new function and an existing one.
     */
    void dsymbols::insert_function(std::shared_ptr<function>& function_decl) {
        const std::string& name = function_decl -> get_name();

        // we make sure no other type of declaration has the same name as this function declaration
        try {
            declaration_type decl_type = m_declarations.at(name);
            if(decl_type == VARIABLE_DECL)
                throw symbol_already_declared("There already exists a variable declaration with the name given to this function.");
        } catch(std::out_of_range err){
        }

        std::vector<std::shared_ptr<function> > fun_decls = get_functions(name, function_decl -> get_params().size());
        for(auto& fun_decl : fun_decls)
            if(function_decl -> collides_with(* fun_decl))
                throw symbol_can_collide("This function can collide with another function.");

        m_functions.emplace(std::make_pair(name, function_decl -> get_params().size()), function_decl);
        m_declarations.emplace(name, FUNCTION_DECL);
    }

    /**
     * get_function
     * if we know that we only have one function with the given name and arity, we call this function
     * if there are many, we raise an exception
     */
    std::shared_ptr<function>& dsymbols::get_function(const std::string& name, const std::size_t arity) {
        if(m_functions.count(std::make_pair(name, arity)) == 0)
            throw symbol_not_found("No function by the name <" + name + "> with arity <" + std::to_string(arity) + "> could be found.");

        if(m_functions.count(std::make_pair(name, arity)) > 1)
            throw symbol_can_collide("Multiple function by the name <" + name + "> with arity <" + std::to_string(arity) + "> were found when only one was expected.");

        auto search = m_functions.find(std::make_pair(name, arity));
        return search -> second;
    }

    /**
     * get_functions
     * given a function name and the arity, return a vector of possible functions
     */
    std::vector<std::shared_ptr<function> > dsymbols::get_functions(const std::string& name, size_t arity) {
        std::vector<std::shared_ptr<function> > functions;
        
        auto range = m_functions.equal_range(std::make_pair(name, arity));
        for(auto it = range.first; it != range.second; ++it)
                functions.push_back(it -> second);

        return functions;
    }

    /**
     * function_exists
     * given a function declaration, find if it already exists in the symbol table
     */
    bool dsymbols::function_exists(std::shared_ptr<function>& function_decl) {
        std::vector<std::shared_ptr<function> > fun_decls = get_functions(function_decl -> get_name(), function_decl -> get_params().size());
        
        for(auto& fun_decl : fun_decls) {
            if(* fun_decl == * function_decl)
                return true;
        }

        return false;
    }

    /**
     * function_exists
     * given a function name and its arity, find if there is any function that fit those two criteria
     */
    bool dsymbols::function_exists(const std::string& name, size_t arity) {
        if(m_functions.count(std::make_pair(name, arity)) > 0)
            return true;

        return false;
    }

    /**
     * function_exists
     * given a name, find if there is any function by that in name in the symbol table
     */
    bool dsymbols::function_exists(const std::string& name) {
        for(auto& fun : m_functions) {
            if(fun.first.first == name)
                return true;
        }

        return false;
    }

    /**
     * insert_variable
     * add a new variable declaration to this table
     */
    void dsymbols::insert_variable(std::shared_ptr<variable>& variable_decl) {
        const std::string& name = variable_decl -> get_name();

        // we make sure no other type of declaration has the same name as this variable declaration
        try {
            declaration_type decl_type = m_declarations.at(name);
            if(decl_type != VARIABLE_DECL)
                throw symbol_can_collide("There already exists another declaration with the name given to this variable.");
        } catch(std::out_of_range err){
        }

        if(m_variables.count(name) > 0)
            throw symbol_already_declared("This variable declaration already exists.");

        m_variables.emplace(name, variable_decl);
        m_declarations.emplace(name, FUNCTION_DECL);
    }

    /**
     * get_variable
     * given a variable name, return the corresponding variable declaration
     */
    std::shared_ptr<variable>& dsymbols::get_variable(const std::string& name) {
        try {
            return m_variables.at(name);
        } catch(std::out_of_range err) {
            throw symbol_not_found("No variable declaration with the given name was found.");
        }
    }

    /**
     * variable_exists
     * given a variable declaration, find if it already exists in this table
     */
    bool dsymbols::variable_exists(std::shared_ptr<variable>& variable_decl) {
        if(m_variables.count(variable_decl -> get_name()) > 0)
            return true;
        else
            return false;
    }

    /**
     * variable_exists
     * given a variable name, find if there is a variabl declaration for it in this table
     */
    bool dsymbols::variable_exists(const std::string& name) {
        if(m_variables.count(name) > 0)
            return true;
        else
            return false;
    }


/**
 * the constructor expects nothing
 */
dtable::dtable() {    
}

    /**
     * insert_type
     * given a namespace and a type declaration, this function adds it into the namespace symbol table
     */
    void dtable::insert_type(const std::string& ns_name, std::shared_ptr<type>& type_decl) {
        // we try to find if there already exists a symbol table attached to the current namespace and use it
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            try {
                decl_symbols.insert_type(type_decl);
            } catch(symbol_already_declared err) {
                throw symbol_already_declared("There already exists another type with the same name and arity declared in this namespace.");
            } catch(symbol_can_collide err) {
                throw symbol_can_collide("There already exist another declaration (variable) with the same name in this namespace.");
            }
        } catch(std::out_of_range err) {
            // if we don't have a symbol table attached to the given namespace, we create an entry for it
            dsymbols decl_symbols;
            decl_symbols.insert_type(type_decl);            
            m_symbols.emplace(ns_name, decl_symbols);
        }
    }

    /**
     * get_types
     * given a namespace and a type name, find all the types that match the give type name; in the namespace; irrespective of arity
     */
    std::vector<std::shared_ptr<type> > dtable::get_types(const std::string& ns_name, const std::string& type_name) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.get_types(type_name);
        } catch(std::out_of_range err) {
            std::vector<std::shared_ptr<type> > type_decls;
            return type_decls;
        }
    }

    /**
     * get_type
     * given a namespace and a type instance, we find the type that builds it from the namespace
     */
    std::shared_ptr<type>& dtable::get_type(const std::string& ns_name, const type_instance& type_ins) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.get_type(type_ins);
        } catch(std::out_of_range err) {
            throw symbol_not_found("There is no type corresponding to the given type instance in this namespace.");
        } catch(symbol_not_found err) {
            throw symbol_not_found(std::string(err.what()) + " Namespace is <" + ns_name + ">.");
        }
    }

    /**
     * get_type
     * given a namespace and a type name and its arity, return the type that matches both from the namespace
     */
    std::shared_ptr<type>& dtable::get_type(const std::string& ns_name, const std::string& type_name, std::size_t arity) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.get_type(type_name, arity);
        } catch(std::out_of_range err) {
            throw symbol_not_found("There is no type corresponding to the given name and arity in this namespace.");
        } catch(symbol_not_found err) {
            throw symbol_not_found(std::string(err.what()) + " Namespace is <" + ns_name + ">.");
        }
    }

    /**
     * type_exists
     * given a namespace and a type declaration, find if an existing type declaration matches it within the namespace
     */
    bool dtable::type_exists(const std::string& ns_name, std::shared_ptr<type>& type_decl) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.type_exists(type_decl);
        } catch(std::out_of_range err) {
            return false;
        }
    }

    /**
     * type_exists
     * given a namespace and a type instance, we find if there is a corresponding type in the namespace symbol table
     */
    bool dtable::type_exists(const std::string& ns_name, const type_instance& type_ins) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.type_exists(type_ins);
        } catch(std::out_of_range err) {
            return false;
        }
    }

    /**
     * type_exists
     * given a namespace and a type name and its arity, find if an existing type declaration matches them within the namespace
     */
    bool dtable::type_exists(const std::string& ns_name, const std::string& type_name, std::size_t arity) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.type_exists(type_name, arity);
        } catch(std::out_of_range err) {
            return false;
        }
    }

    /**
     * type_exists
     * given a type name and its arity, find if an existing type declaration matches them within the namespace
     */
    bool dtable::type_exists(const std::string& type_name, std::size_t arity) {
        for(auto& decl_symbols : m_symbols) {
            if(decl_symbols.second.type_exists(type_name, arity))
                return true;
        }
        return false;
    }

    /**
     * insert_function
     * given a namespace and a function declaration, insert the last in the symbol table
     */
    void dtable::insert_function(const std::string& ns_name, std::shared_ptr<function>& function_decl) {
        // we try to find if there already exists a symbol table attached to the current namespace and use it
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            try {
                decl_symbols.insert_function(function_decl);
            } catch(symbol_can_collide err) {
                throw symbol_can_collide("There already exist another function <" + mangle_function(* function_decl) + "> with the same name, parameters and return type in the current namespace.");
            } catch(symbol_already_declared err) {
                throw symbol_already_declared("There already exist another declaration (type or variable) with the same name as this function.");
            }
        } catch(std::out_of_range err) {
            // if we don't have a symbol table attached to the given namespace, we create an entry for it
            dsymbols decl_symbols;
            decl_symbols.insert_function(function_decl);            
            m_symbols.emplace(ns_name, decl_symbols);
        }
    }

    /**
     * get_function
     * given a namespace and a function name and its arity, return the function that matches both from the namespace
     */
    std::shared_ptr<function>& dtable::get_function(const std::string& ns_name, const std::string& function_name, std::size_t arity) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.get_function(function_name, arity);
        } catch(std::out_of_range err) {
            throw symbol_not_found("There is no function corresponding to the given name and arity in this namespace.");
        } catch(symbol_not_found err) {
            throw symbol_not_found(std::string(err.what()) + " Namespace is <" + ns_name + ">.");
        }
    }

    /**
     * get_functions
     * given a namespace, a function name and its arity, return all functions in that namespace with the same name and arity
     */
    std::vector<std::shared_ptr<function> > dtable::get_functions(const std::string& ns_name, const std::string& function_name, size_t arity) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.get_functions(function_name, arity);
        } catch(std::out_of_range err) {
            std::vector<std::shared_ptr<function> > fun_decls;
            return fun_decls;
        }
    }

    /**
     * function_exists
     * given a namespace and a function declaration, find if that function declaration already exists in that namespace
     */
    bool dtable::function_exists(const std::string& ns_name, std::shared_ptr<function>& function_decl) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.function_exists(function_decl);
        } catch(std::out_of_range err) {
            return false;
        }
    }

    /**
     * function_exists
     * given a namespace, a function name and its arity, find if we have a function with that name and arity in the current namespace
     */
    bool dtable::function_exists(const std::string& ns_name, const std::string& function_name, size_t arity) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.function_exists(function_name, arity);
        } catch(std::out_of_range err) {
            return false;
        }
    }

    /**
     * function_exists
     * given a namespace and a function name, find if the namespace contains a function with that name
     */
    bool dtable::function_exists(const std::string& ns_name, const std::string& function_name) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.function_exists(function_name);
        } catch(std::out_of_range err) {
            return false;
        }
    }

    /**
     * insert_variable
     * given a namespace and a variable declaration, add the later to the former
     */
    void dtable::insert_variable(const std::string& ns_name, std::shared_ptr<variable>& variable_decl) {
        // we try to find if there already exists a symbol table attached to the current namespace and use it
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            try {
                decl_symbols.insert_variable(variable_decl);
            } catch(symbol_already_declared err) {
                throw symbol_already_declared("There already exists another variable with the same name <" + variable_decl -> get_name() + "> declared in the namespace <" + ns_name + ">.");
            } catch(symbol_can_collide err) {
                throw symbol_can_collide("There already exist another declaration (type or function) with the same name <" + variable_decl -> get_name() + "> in the namespace <" + ns_name + ">.");
            }
        } catch(std::out_of_range err) {
            // if we don't have a symbol table attached to the given namespace, we create an entry for it
            dsymbols decl_symbols;
            decl_symbols.insert_variable(variable_decl);
            m_symbols.emplace(ns_name, decl_symbols);
        }
    }

    /**
     * get_variable
     * given a namespace and a variable name, return the corresponding variable declaration if it exists in that namespace
     */
    std::shared_ptr<variable>& dtable::get_variable(const std::string& ns_name, const std::string& variable_name) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.get_variable(variable_name);
        } catch(std::out_of_range err) {
            throw symbol_not_found("No variable declaration with the given name was found in the given namespace because the namespace doesn't exist.");
        }
    }

    /**
     * variable_exists
     * given a namespace and a variable declaration, find if it already exists in this namespace
     */
    bool dtable::variable_exists(const std::string& ns_name, std::shared_ptr<variable>& variable_decl) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.variable_exists(variable_decl);
        } catch(std::out_of_range err) {
            return false;
        }
    }

    /**
     * variable_exists
     * given a namespace and a variable name, find if there is a variabl declaration for it in this namespace
     */
    bool dtable::variable_exists(const std::string& ns_name, const std::string& variable_name) {
        try {
            dsymbols& decl_symbols = m_symbols.at(ns_name);
            return decl_symbols.variable_exists(variable_name);
        } catch(std::out_of_range err) {
            return false;
        }
    }
}
