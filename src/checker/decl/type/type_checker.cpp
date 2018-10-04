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

#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>
#include <map>

/* AST */
#include "representer/ast/decl/type.hpp"

/* Symtable */
#include "representer/symtable/scope.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* Generator */
#include "checker/decl/type/type_generator.hpp"

/* Checker */
#include "checker/decl/type/type_checker.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_not_found.hpp"
#include "checker/exceptions/invalid_constructor.hpp"
#include "checker/exceptions/invalid_type.hpp"


namespace avalon {
    /**
     * simple_check
     * given a type instance, the scope were it was found, the namespace of the object that holds it and a list of possible standins,
     * this function checks to see if that type instance exists in that scope in the given namespace or among the standins.
     */
    std::pair<bool,bool> type_instance_checker::simple_check(type_instance& instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& standins) {
        std::vector<type_instance>& instance_params = instance.get_params();
        std::shared_ptr<type> instance_type = nullptr;
        std::pair<bool,bool> ret(false, false);
        instance.is_parametrized(false);

        // if we have the star type instance, we don't bother looking for it in the symbol table
        if(instance.is_star()) {
            ret.first = true;
            ret.second = true;
            return ret;
        }

        // if we have a reference type instance, we check its parameter instead
        if(instance.is_reference()) {
            return type_instance_checker::complex_check(instance_params[0], l_scope, ns_name, standins);
        }

        // we can only look for user defined type instances in the scope we have
        if(instance.get_category() == USER) {
            // we try to find if the type instance has an associated type that builds
            try {
                instance_type = l_scope -> get_type(ns_name, instance);

                if(instance_type -> is_valid(UNKNOWN)) {
                    type_checker t_checker;

                    // check the type
                    try {
                        t_checker.check(instance_type, l_scope, ns_name);
                    } catch(invalid_type err) {
                        throw err;
                    }
                }
                else if(instance_type -> is_valid(INVALID)) {
                    throw invalid_type(instance_type -> get_token(), "Type <" + mangle_type(instance_type) + "> is not valid. Please make sure all the type instances its constructors depend on are valid. This means they must have type constructors (also called type builders) that exist in the current scope.");
                }

                // the type instance has a type that builds it,
                // we make sure the parameters it depends on are also valid
                const std::vector<token>& type_params = instance_type -> get_params();
                auto ins_it = instance_params.begin(), ins_end = instance_params.end();
                auto type_it = type_params.begin(), type_end = type_params.end();
                for(; ins_it != ins_end && type_it != type_end; ++ins_it, ++type_it) {
                    try {
                        std::pair<bool,bool> res = type_instance_checker::complex_check(* ins_it, l_scope, ns_name, standins);
                        // if the parameter is parametrized, so is this type instance
                        if(res.first == true || res.second == true) {
                            instance.is_parametrized(true);
                            ret.second = true;
                        }
                        // we set the old token on the param
                        ins_it -> set_old_token(* type_it);
                    } catch(invalid_type err) {
                        throw err;
                    }
                }
                // the type instances this type instance depend on are valid
                // 1. we attach it to the type instance
                instance.set_type(instance_type);

                // 2. we generate the type that corresponds to the type instance
                if(instance.is_complete()) {
                    std::shared_ptr<type> generated_type = type_generator::generate(instance);
                    instance_type -> add_specialization(generated_type);
                    instance_type -> is_used(true);
                }                
            }
            catch(symbol_not_found err) {
                // the type was not found in the scope given the namespace,
                // we try to find if it might be among the stand in types
                // but we only bother checking if it is among the standins if it admit no parameters
                if(instance_params.size() > 0) {
                    throw invalid_type(instance.get_token(), "The type instance <" + mangle_type_instance(instance) + "> has no type that builds it in the namespace <" + ns_name + ">.");
                }
                else {
                    if(std::find(standins.begin(), standins.end(), instance.get_token()) != standins.end()) {
                        ret.first = true;
                    }
                    else {
                        throw invalid_type(instance.get_token(), "The type instance <" + mangle_type_instance(instance) + "> has no type that builds it in the namespace <" + ns_name + ">.");
                    }
                }
            }
        }
        // if we have a built-in type, we check the type instances it depends on if any and set the type builder on it
        else {
            if(instance.get_category() == LIST && instance_params.size() != 1)
                throw invalid_type(instance.get_token(), "The type instance <" + mangle_type_instance(instance) + " is a list but has more or less than 1 parameter.");

            if(instance.get_category() == MAP && instance_params.size() != 2)
                throw invalid_type(instance.get_token(), "The type instance <" + mangle_type_instance(instance) + " is a list but has more or less than 2 parameter.");

            // we check dependent type instances, if any
            for(auto& instance_param : instance_params) {
                try {
                    std::pair<bool,bool> res = type_instance_checker::complex_check(instance_param, l_scope, ns_name, standins);
                    if(res.first == true || res.second == true) {
                        instance.is_parametrized(true);
                        ret.second = true;
                    }
                } catch(invalid_type err) {
                    throw err;
                }
            }

            // set the type builder
            token tok = instance.get_token();
            std::shared_ptr<type> ty = std::make_shared<type>(tok, VALID);
            instance.set_type(ty);
        }

        return ret;
    }

    /**
     * complex_check
     * given a type instance, the scope were it was found, the namespace of the object that holds it and a list of possible standins,
     * these functions check to see if that type instance has a type builder that live in that scope in one the possible namespaces:
     * - the holder object namespace
     * - the global namespace
     * - the type instance attached namespace
     */
    std::pair<bool,bool> type_instance_checker::complex_check(type_instance& instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& standins) {
        const std::string& l_ns_name = instance.get_namespace();
       std::pair<bool,bool> res;

        // if the type instance namespace is "*" then it means the type instance exists either:
        // - in the current namespace where the object that holds it is declared (this is given to us as a parameter)
        // - or in the global namespace
        if(l_ns_name == "*") {
            try {
                // first we search the type instance holder object namespace
                res = type_instance_checker::simple_check(instance, l_scope, ns_name, standins);
            } catch(invalid_type err) {
                // we could not find the type instance in the holder namespace, we try the global namespace
                try {
                    res = type_instance_checker::simple_check(instance, l_scope, l_ns_name, standins);
                } catch(invalid_type err) {
                    throw err;
                }
            }
        }
        else {
            try {
                res = type_instance_checker::simple_check(instance, l_scope, l_ns_name, standins);
                // if the type instance is abstract, we raise an error because abstract type instances
                // are not allowed to be constrainted to a namespace
                if(res.first) {
                    throw invalid_type(instance.get_token(), "An abstract type instance cannot be constrainted to a namespace.");
                }
            } catch(invalid_type err) {
                throw invalid_type(instance.get_token(), "The type instance <" + mangle_type_instance(instance) + "> has no type that builds it in the namespace <" + ns_name + ">.");
            }
        }

        return res;
    }

    std::pair<bool,bool> type_instance_checker::complex_check(type_instance& instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<token> standins;
        return complex_check(instance, l_scope, ns_name, standins);
    }


    /**
     * check
     * given a default or record constructor,
     * this function checks whether they are valid by making sure
     * that all type instances they depend on are built from valid types.
     */
    void constructor_checker::check(default_constructor& def_constructor, std::shared_ptr<type>& type_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<type_instance>& cons_params = def_constructor.get_params();
        const std::vector<token>& type_params = type_decl -> get_params();

        // a constructor is judged to be valid if all the types it depends on are valid
        for(auto& cons_param : cons_params) {
            const std::string& l_ns_name = cons_param.get_namespace();
            std::shared_ptr<type> instance_type = nullptr;

            // validate the constructor parameter
            try {
                std::pair<bool,bool> res = type_instance_checker::complex_check(cons_param, l_scope, ns_name, type_params);
                if(res.first == false)
                    instance_type = cons_param.get_type();
            } catch(invalid_type err) {
                // we check if the type instance in question is not the current type this constructor builds
                if(cons_param.is_builtby(type_decl)) {
                    instance_type = type_decl;
                    cons_param.set_type(type_decl);
                }
                else {
                    throw invalid_constructor("This constructor depends on a type instance that does not exist either in the attached namespace or the local namespace or the global namespace.");
                }
            }

            // if the type that builds the parameters this constructor depends on is private and the type this constructor is public
            // we issue an error as this constructor cannot be used
            if(instance_type != nullptr && (instance_type -> is_public() == false && type_decl -> is_public() == true)) {
                throw invalid_constructor("This constructor depends on a type instance that's private while the type it builds is public. Both must be public or both must be private.");
            }

            // we have the type instance type builder, we check if it is the same as that which this constructor builds
            // if they are the same, then we can judge this constructor to be temporarily valid
            if((ns_name == l_ns_name) & (instance_type != nullptr && * instance_type == * type_decl)) {
                continue;
            }
            else {
                // if the type instance type builder is invalid, so it is the constructor
                if(instance_type != nullptr && instance_type -> is_valid(INVALID))
                    throw invalid_constructor("Default constructor <" + def_constructor.get_name() + "> failed type checking because the type <" + instance_type -> get_name() + "> is not valid.");
            }
        }
    }

    void constructor_checker::check(record_constructor& rec_constructor, std::shared_ptr<type>& type_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::map<token, type_instance>& cons_params = rec_constructor.get_params();
        const std::vector<token>& type_params = type_decl -> get_params();

        // a constructor is judged to be valid if all the types it depends on are valid        
        for(auto& cons_param : cons_params) {
            const std::string& l_ns_name = cons_param.second.get_namespace();
            std::shared_ptr<type> instance_type = nullptr;

            // validate the constructor parameter
            try {
                std::pair<bool,bool> res = type_instance_checker::complex_check(cons_param.second, l_scope, ns_name, type_params);
                if(res.first == false)
                    instance_type = cons_param.second.get_type();
            } catch(invalid_type err) {
                // we check if the type instance in question is not the current type this constructor builds
                if(cons_param.second.is_builtby(type_decl)) {
                    instance_type = type_decl;
                    cons_param.second.set_type(type_decl);
                }
                else {
                    throw invalid_constructor("This constructor depends on a type instance that does not exist either in the attached namespace or the local namespace or the global namespace.");
                }
            }

            // if the type that builds the parameters this constructor depends on is private and the type this constructor is public
            // we issue an error as this constructor cannot be used
            if(instance_type != nullptr && (instance_type -> is_public() == false && type_decl -> is_public() == true)) {
                throw invalid_constructor("This constructor depends on a type instance that's private while the type it builds is public. Both must be public or both must be private.");
            }

            // we have the type instance type builder, we check if it is the same as that which this constructor builds
            // if they are the same, then we can judge this constructor to be temporarily valid
            if((ns_name == l_ns_name) & (instance_type != nullptr && * instance_type == * type_decl)) {
                continue;
            }
            else {
                // if the type instance type builder is invalid, so it is the constructor
                if(instance_type != nullptr && instance_type -> is_valid(INVALID))
                    throw invalid_constructor("Record constructor <" + rec_constructor.get_name() + "> failed type checking because the type <" + instance_type -> get_name() + "> is not valid.");
            }
        }
    }


/**
 * the default constructor expects nothing
 */
type_checker::type_checker() {
}
    
    /**
     * check
     * - makes sure all type parameters are valid
     * - make sure all constructors are valid
     */
    void type_checker::check(std::shared_ptr<type>& type_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        type_decl -> set_is_valid(VALIDATING);

        // check type parameters making sure no parameter appears twice
        // and no parameter correponds to any type in all the available namespaces
        const std::vector<token>& type_params = type_decl -> get_params();
        for(const auto& type_param : type_params) {
            // make sure no parameter is a duplicate of an existing parameter
            if(std::count(type_params.begin(), type_params.end(), type_param) > 1) {
                throw invalid_type(type_param, "The type parameter <" + type_param.get_lexeme() + "> cannot be used more than once inside type parameters.");
            }

            // make sure no parameter correponds to any concrete type available in the scope (all namespaces)
            if(l_scope -> type_exists(type_param.get_lexeme(), 0)) {
                throw invalid_type(type_param, "The type parameter <" + type_param.get_lexeme() + "> has the same name as a concrete type available in the current scope.");
            }
        }

        // check default constructors
        std::vector<default_constructor> def_constructors = type_decl -> get_default_constructors();
        for(auto& def_constructor : def_constructors) {
            try {
                constructor_checker::check(def_constructor, type_decl, l_scope, ns_name);
                type_decl -> replace_constructor(def_constructor);
            } catch(invalid_constructor err) {
                throw invalid_type(def_constructor.get_token(), err.what());
            }
        }

        // check record constructors
        std::vector<record_constructor> rec_constructors = type_decl -> get_record_constructors();
        for(auto& rec_constructor : rec_constructors) {
            try {
                constructor_checker::check(rec_constructor, type_decl, l_scope, ns_name);
                type_decl -> replace_constructor(rec_constructor);
            } catch(invalid_constructor err) {
                throw invalid_type(rec_constructor.get_token(), err.what());
            }
        }

        // all is well, the type declaration is valid
        type_decl -> set_is_valid(VALID);
    }
}
