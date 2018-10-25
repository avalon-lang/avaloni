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
#include <stdexcept>
#include <utility>
#include <memory>
#include <string>

/* AST */
#include "representer/ast/decl/function.hpp"

/* Symtable */
#include "representer/symtable/scope.hpp"

/* Checker */
#include "checker/decl/function/termination_checker.hpp"
#include "checker/decl/function/function_checker.hpp"
#include "checker/decl/function/header_checker.hpp"
#include "checker/decl/function/body_checker.hpp"
#include "checker/decl/type/type_checker.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_can_collide.hpp"
#include "representer/exceptions/symbol_not_found.hpp"
#include "checker/exceptions/invalid_expression.hpp"
#include "checker/exceptions/invalid_statement.hpp"
#include "checker/exceptions/invalid_function.hpp"
#include "checker/exceptions/invalid_variable.hpp"
#include "checker/exceptions/invalid_block.hpp"
#include "checker/exceptions/invalid_type.hpp"


namespace avalon {
static void fill_duplicates(std::vector<std::pair<std::size_t, std::size_t> >& weights, std::vector<std::size_t>& duplicates) {
    std::size_t uber_weight = weights[0].second;
    std::size_t max_weight = weights[0].second;
    for(auto& weight : weights) {
        if(weight.second > max_weight) {
            if(duplicates.size() > 1) {
                duplicates.clear();
                duplicates.push_back(weight.first);
            }
            else if(duplicates.size() == 0) {
                duplicates.push_back(weight.first);
            }
            else {
                duplicates[0] = weight.first;
            }
            max_weight = weight.second;
            uber_weight = max_weight > uber_weight ? max_weight : uber_weight;
        }
        else if(weight.second == max_weight) {
            duplicates.push_back(weight.first);
        }
    }

    // if the "maxest" weight (whatever the hell that means), aka uber_weight is 0,
    // then issue an exception because it means none of the weigths given matched the type instance(s) passed to us
    if(uber_weight == 0)
        throw std::logic_error("No duplicates could be filled");
}

/*
 * NOTE: This function is too long, inefficient, contains duplicated code; basically everything against proper software engineering.
 * But it also works and I have not the time to refactor it and improve upon it right now. Will do that when it matters (and that is not now).
 */
static std::shared_ptr<function> internal_find_function(const std::string& name, std::vector<type_instance>& param_instances, type_instance& ret_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& param_standins) {
    std::vector<std::shared_ptr<function> > candidates = l_scope -> get_functions(ns_name, name, param_instances.size());
    std::unordered_map<std::size_t, std::vector<std::size_t> > weights;
    std::vector<std::pair<std::size_t, std::size_t> > total_weights;
    std::vector<std::size_t> duplicate_weights;

    // 0. we make sure we have at least one candidate function to work with
    if(candidates.size() == 0)
        throw symbol_not_found("No function by the name <" + name + "> and given arguments and return type was found");

    // 1. we calculate the weights of all candidate functions relative to the given parameters
    for(auto it = candidates.begin(), end = candidates.end(); it != end; ++it) {
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& fun_params = (* it) -> get_params();
        std::vector<token>& fun_standins = (* it) -> get_constraints();
        auto fun_it = fun_params.begin(), fun_end = fun_params.end();
        auto param_it = param_instances.begin(), param_end = param_instances.end();

        for(; fun_it != fun_end && param_it != param_end; ++fun_it, ++param_it) {
            type_instance& fun_instance = fun_it -> second -> get_type_instance();
            try {
                std::pair<bool,bool> res = type_instance_checker::complex_check(fun_instance, l_scope, ns_name, fun_standins);
                if(res.second == true)
                    fun_instance.is_parametrized(true);
                fun_it -> second -> set_type_instance(fun_instance);
            } catch(invalid_type err) {
                throw err;
            }

            // we then validate the type instance we are looking for in the function
            type_instance param_instance = * param_it;
            try {
                std::pair<bool,bool> res = type_instance_checker::complex_check(param_instance, l_scope, ns_name, param_standins);
                if(res.second == true)
                    param_instance.is_parametrized(true);
            } catch(invalid_type err) {
                throw err;
            }

            std::size_t weight = type_instance_weight(fun_instance, param_instance);
            std::size_t index = it - candidates.begin();
            try {
                std::vector<std::size_t>& fun_weights = weights.at(index);
                fun_weights.push_back(weight);
            } catch(std::out_of_range err) {
                std::vector<std::size_t> fun_weights;
                fun_weights.push_back(weight);
                weights.emplace(index, fun_weights);
            }
        }
    }

    // 2. if we have at least one weight, we work on them looking for possible matches
    if(weights.size() > 0) {
        // 2.1. we calculate the total weights for each function
        for(auto& weight : weights) {
            std::size_t fun_index = weight.first;
            std::vector<std::size_t>& fun_weights = weight.second;
            std::size_t total_weight = 0;
            for(auto& fun_weight : fun_weights) {
                total_weight += fun_weight;
            }
            total_weights.emplace_back(fun_index, total_weight);
        }
        // 2.2. find all the duplicates so we can look at their return types for discrimination
        try {
            fill_duplicates(total_weights, duplicate_weights);
        } catch(std::logic_error err) {
            throw symbol_not_found("No function by the name <" + name + "> and given arguments was found");
        }        

        // 2.3. if we got a valid return type instance, we match it against the possible duplicate candidates
        if(ret_instance.is_star() == false) {
            // we start by making sure that the return type instance is valid
            try {
                type_instance_checker::complex_check(ret_instance, l_scope, ns_name, param_standins);
            } catch(invalid_type err) {
                throw err;
            }

            // if there is at least one duplicate, then we look which one matches the given return type
            std::vector<std::pair<std::size_t, std::size_t> > ret_candidates;
            std::vector<std::size_t> ret_duplicates;
            for(auto& duplicate : duplicate_weights) {
                std::shared_ptr<function>& candidate = candidates[duplicate];
                type_instance& fun_ret_instance = candidate -> get_return_type_instance();                
                if(type_instance_weak_compare(ret_instance, fun_ret_instance) == true) {
                    std::size_t ret_weight = type_instance_weight(ret_instance, fun_ret_instance);
                    ret_candidates.emplace_back(duplicate, ret_weight);                    
                }
            }

            // if we got no weight, then no function matches
            if(ret_candidates.size() == 0)
                throw symbol_not_found("At least one function was found that matches the name <" + name + "> and arguments given but none that match the return type instance given.");

            // we find duplicates for the return type instances
            try {
                fill_duplicates(ret_candidates, ret_duplicates);
            } catch(std::logic_error err) {
                throw symbol_not_found("At least one function was found with the name <" + name + "> and given arguments but none that match the return type instance given.");
            }

            // if we got any duplicates, we scream
            if(ret_duplicates.size() > 1)
                throw symbol_can_collide("We found more than one function with the name <" + name + ">, arguments and return type.");

            // we got out function, we return it
            return candidates[ret_duplicates[0]];
        }
        // if on the other no valid return type instance was given, then we make sure only one duplicate exists and we return it if so
        else {
            if(duplicate_weights.size() > 1) {
                throw symbol_not_found("Multiple functions match the name <" + name + "> and arguments. Please specify the return type to discrimate.");
            }
            else {
                std::shared_ptr<function>& candidate = candidates[duplicate_weights[0]];
                return candidate;
            }
        }
    }
    // 3. if we have no weights, then the candidates functions don't accept arguments
    // so either only one function was found and if many, the user must specify the return type instance to help discriminate
    else {
        // a return type was provided
        if(ret_instance.is_star() == false) {
            // we start by making sure that the return type instance is valid
            try {
                type_instance_checker::complex_check(ret_instance, l_scope, ns_name, param_standins);
            } catch(invalid_type err) {
                throw err;
            }

            // using the return type
            std::vector<std::pair<std::size_t, std::size_t> > ret_candidates;
            std::vector<std::size_t> ret_duplicates;
            for(auto it = candidates.begin(), end = candidates.end(); it != end; ++it) {
                type_instance& fun_ret_instance = (* it) -> get_return_type_instance();
                std::size_t index = it - candidates.begin();
                if(type_instance_weak_compare(ret_instance, fun_ret_instance) == true) {
                    std::size_t ret_weight = type_instance_weight(ret_instance, fun_ret_instance);
                    ret_candidates.emplace_back(index, ret_weight);
                }
            }

            // if we got no weight, then no function matches
            if(ret_candidates.size() == 0)
                throw symbol_not_found("No function found that matches the name <" + name + ">, arguments and return type.");

            // we figure out if we have duplicates - basically meaning more than one return type instance can match what we were given
            try{
                fill_duplicates(ret_candidates, ret_duplicates);
            } catch(std::logic_error err) {
                throw symbol_not_found("At least one function was found that match the name <" + name + "> and given arguments but none that match the return type instance given.");
            }

            // if we got any duplicates, we scream (basically two functions can return the given type)
            if(ret_duplicates.size() > 1)
                throw symbol_not_found("Multiple functions with name <" + name + ">, with arguments and return type instances.");

            // we got out function, we return it
            return candidates[ret_duplicates[0]];
        }
        // no return type was specified, then only one function is amond candidates
        else {
            if(candidates.size() > 1) {
               throw symbol_not_found("Multiple functions with name <" + name + "> and no arguments was found. Please specify the return type to discrimate.");
            }
            else {
                std::shared_ptr<function>& candidate = candidates[0];
                return candidate;
            }
        }
    }

    // 4. if we are here and we didn't return, then no function was found
    throw symbol_not_found("No function by the name <" + name + ">,  arguments and return type was found.");
}

/**
 * find_function
 * given a function name, parameters' type instances and possibly a return type, get the function that matches all
 */     
std::shared_ptr<function> find_function(const std::string& name, std::vector<type_instance>& param_instances, type_instance& ret_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& param_standins) {
    return internal_find_function(name, param_instances, ret_instance, l_scope, ns_name, param_standins);
}

std::shared_ptr<function> find_function(const std::string& name, std::vector<type_instance>& param_instances, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& param_standins) {
    token gen_tok(MUL, "*", 0, 0, "__bit__");
    type_instance generic_instance(gen_tok, "*");
    return internal_find_function(name, param_instances, generic_instance, l_scope, ns_name, param_standins);
}

/**
 * a default constructor expects nothing
 */
function_checker::function_checker() {
}

    /**
     * check
     * this function makes sure:
     * - all parameters' types and return type are valid
     * - the body of the function is valid
     * - all declarations that make up a function are reachable (conservatively)
     * - all branches do terminate normally (conservatively)
     */
    void function_checker::check(function& function_decl, const std::string& ns_name) {
        // if the function declaration is a builtin function, we don't run the checker
        if(function_decl.is_builtin())
            return;

        // make sure a scope was set on the function
        std::shared_ptr<scope>& l_scope = function_decl.get_scope();
        if(l_scope == nullptr) {
            throw invalid_function(function_decl.get_token(), "[compiler error] there is a function without a scope for it.");
        }

        try {
            // we check the header aka function signature
            check_header(function_decl, l_scope, ns_name);

            // check the body of the function
            check_body(function_decl, l_scope, ns_name);

            // we check for termination
            check_termination(function_decl, l_scope);
        } catch(invalid_function err) {
            throw err;
        }
    }

    /**
     * check_header
     * this function ensures that the function's header (parameters and return type) is of the correct form
     */
    void function_checker::check_header(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        try {
            header_checker::check_header(function_decl, l_scope, ns_name);
        } catch(invalid_function err) {
            throw err;
        }
    }

    /**
     * check_body
     * this function ensures that the body of the function is valid:
     * - makes sure that declarations that make up the function's body are either variable declarations or statement declarations
     * - make sure that all return statements return objects of the correct type
     * - make sure that all declarations that make up the function body are valid
     */
    void function_checker::check_body(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        try {
            body_checker::check_body(function_decl, l_scope, ns_name);
        } catch(invalid_variable err) {
            // if a variable declaration fails checking inside the block
            throw invalid_function(err.get_token(), err.what());
        } catch(invalid_block err) {
            // if the block contains anything other that variable and statement declarations
            throw invalid_function(function_decl.get_token(), err.what());
        } catch(invalid_expression err) {
            // if any expression inside the function sucks
            throw invalid_function(err.get_token(), err.what());
        } catch(invalid_type err) {
            // temporary
            throw invalid_function(err.get_token(), err.what());
        } catch(invalid_statement err) {
            throw invalid_function(err.get_token(), err.what());
        }
    }

    /**
     * check_termination
     * this function makes sure that all declarations in the function's body are reachable and that the function terminates
     */
    void function_checker::check_termination(function& function_decl, std::shared_ptr<scope>& l_scope) {
        termination_checker term_checker;
        try {
            term_checker.check_termination(function_decl, l_scope);
        } catch(invalid_function err) {
            throw err;
        }
    }
}
