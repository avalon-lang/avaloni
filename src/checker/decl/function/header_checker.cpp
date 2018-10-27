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
#include <vector>
#include <string>

/* AST */
#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/type.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_string.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* Checker */
#include "checker/decl/function/header_checker.hpp"
#include "checker/decl/type/type_checker.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_already_declared.hpp"
#include "representer/exceptions/symbol_can_collide.hpp"
#include "checker/exceptions/invalid_function.hpp"
#include "checker/exceptions/invalid_type.hpp"


namespace avalon {
    /**
     * check_header
     * this function ensures that the function's header (parameters and return type) is of the correct form
     */
    void header_checker::check_header(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = function_decl.get_params();
        std::vector<token>& constraints = function_decl.get_constraints();

        // we make sure all the function's parameters type instances are valid
        for(auto& param : params) {
            type_instance param_type_instance = param.second -> get_type_instance();

            try {
                std::shared_ptr<type> param_instance_type = nullptr;
                std::pair<bool,bool> res = type_instance_checker::complex_check(param_type_instance, l_scope, ns_name, constraints);
                if(res.first == false)
                    param_instance_type = param_type_instance.get_type();

                // we make sure that if the function is public then all the type instances it depends are built by public types
                if(param_instance_type != nullptr && (param_instance_type -> is_public() == false && function_decl.is_public() == true)) {
                    throw invalid_function(function_decl.get_token(), "This function is public but has parameters that depend on private types. All parameters of a public function must be public as well.");
                }

                // we make sure that the parameter type instance is not a quantum type
                if(param_instance_type != nullptr && param_instance_type -> is_quantum())
                    throw invalid_function(param_type_instance.get_token(), "Quantum types cannot be used a type instances to function parameters.");

                // if the type instance checked out, we replace the one on the parameter with the updated on
                param.second -> set_type_instance(param_type_instance);

                // add the parameter to the function scope
                try {
                    l_scope -> add_variable("*", param.second);
                } catch(symbol_already_declared err) {
                    throw invalid_function(param.second -> get_token(), err.what());
                } catch(symbol_can_collide err) {                    
                    throw invalid_function(param.second -> get_token(), err.what());
                }
            } catch(invalid_type err) {
                throw invalid_function(err.get_token(), err.what());
            }

            // string, tuples, lists and maps must always be immutable
            if(param_type_instance.get_category() == USER) {
                avalon_string avl_string;
                type_instance string_type_instance =  avl_string.get_type_instance();
                type_instance_checker::complex_check(string_type_instance, l_scope, ns_name, constraints);
                if(type_instance_strong_compare(string_type_instance, param_type_instance) == true && param.second -> is_mutable())
                    throw invalid_function(param.second -> get_token(), "A function parameter with type <" + mangle_type_instance(param_type_instance) + "> cannot be mutable.");
            }
            else if((param_type_instance.get_category() == TUPLE || param_type_instance.get_category() == LIST || param_type_instance.get_category() == MAP) && param.second -> is_mutable() == true) {
                throw invalid_function(param.second -> get_token(), "A function parameter with type <" + mangle_type_instance(param_type_instance) + "> cannot be mutable.");
            }
        }

        // we make sure the function's return type instance is valid
        type_instance& return_type_instance = function_decl.get_return_type_instance();
        try {
            std::shared_ptr<type> return_instance_type = nullptr;
            std::pair<bool,bool> res = type_instance_checker::complex_check(return_type_instance, l_scope, ns_name, constraints);
            if(res.first == false)
                return_instance_type = return_type_instance.get_type();

            // we make sure that if the function is public then the return type instance must be public as well
            if(return_instance_type != nullptr && (return_instance_type -> is_public() == false && function_decl.is_public() == true)) {
                throw invalid_function(function_decl.get_token(), "This function is public but its return type depends on a private type. The return type must be public as well or the function must be private.");
            }

            // if the type instance checked out, we replace the previous return type instance
            function_decl.set_return_type_instance(return_type_instance);
        } catch(invalid_type err) {
            throw invalid_function(err.get_token(), err.what());
        }
    }

    /**
     * prepare_header
     * this functions validates all type instances found in the function's header
     */
    void header_checker::prepare_header(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = function_decl.get_params();
        std::vector<token>& constraints = function_decl.get_constraints();

        // we make sure all the function's parameters type instances are valid
        for(auto& param : params) {
            type_instance param_type_instance = param.second -> get_type_instance();

            try {
                std::shared_ptr<type> param_instance_type = nullptr;
                std::pair<bool,bool> res = type_instance_checker::complex_check(param_type_instance, l_scope, ns_name, constraints);
                if(res.first == false)
                    param_instance_type = param_type_instance.get_type();

                // we make sure that if the function is public then all the type instances it depends are built by public types
                if(param_instance_type != nullptr && (param_instance_type -> is_public() == false && function_decl.is_public() == true)) {
                    throw invalid_function(function_decl.get_token(), "This function is public but has parameters that depend on private types. All parameters of a public function must be public as well.");
                }

                // if the type instance checked out, we replace the one on the parameter with the updated on
                param.second -> set_type_instance(param_type_instance);
            } catch(invalid_type err) {
                throw invalid_function(err.get_token(), err.what());
            }
        }

        // we make sure the function's return type instance is valid
        type_instance& return_type_instance = function_decl.get_return_type_instance();
        try {
            std::shared_ptr<type> return_instance_type = nullptr;
            std::pair<bool,bool> res = type_instance_checker::complex_check(return_type_instance, l_scope, ns_name, constraints);
            if(res.first == false)
                return_instance_type = return_type_instance.get_type();

            // we make sure that if the function is public then the return type instance must be public as well
            if(return_instance_type != nullptr && (return_instance_type -> is_public() == false && function_decl.is_public() == true)) {
                throw invalid_function(function_decl.get_token(), "This function is public but its return type depends on a private type. The return type must be public as well or the function must be private.");
            }

            // if the type instance checked out, we replace the previous return type instance
            function_decl.set_return_type_instance(return_type_instance);
        } catch(invalid_type err) {
            throw invalid_function(err.get_token(), err.what());
        }
    }
}
