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

#include <stdexcept>
#include <utility>
#include <cstddef>
#include <memory>
#include <string>
#include <cstdio>
#include <map>

/* Expressions */
#include "representer/ast/expr/dereference_expression.hpp"
#include "representer/ast/expr/assignment_expression.hpp"
#include "representer/ast/expr/underscore_expression.hpp"
#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/grouped_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/binary_expression.hpp"
#include "representer/ast/expr/unary_expression.hpp"
#include "representer/ast/expr/match_expression.hpp"
#include "representer/ast/expr/tuple_expression.hpp"
#include "representer/ast/expr/list_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/cast_expression.hpp"
#include "representer/ast/expr/map_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_qubit8.hpp"
#include "representer/builtins/lang/avalon_qubit4.hpp"
#include "representer/builtins/lang/avalon_qubit2.hpp"
#include "representer/builtins/lang/avalon_qubit.hpp"
#include "representer/builtins/lang/avalon_float.hpp"
#include "representer/builtins/lang/avalon_maybe.hpp"
#include "representer/builtins/lang/avalon_tuple.hpp"
#include "representer/builtins/lang/avalon_list.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_bit2.hpp"
#include "representer/builtins/lang/avalon_bit4.hpp"
#include "representer/builtins/lang/avalon_bit8.hpp"
#include "representer/builtins/lang/avalon_int.hpp"
#include "representer/builtins/lang/avalon_map.hpp"
#include "representer/builtins/lang/avalon_ref.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"

/* Checker */
#include "checker/decl/function/function_checker.hpp"
#include "checker/decl/type/type_checker.hpp"

/* Symbol table */
#include "representer/symtable/scope.hpp"

/* AST */
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/type.hpp"

/* Generator */
#include "checker/decl/function/function_generator.hpp"

/* Inferer */
#include "inferer/inferer.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_can_collide.hpp"
#include "representer/exceptions/symbol_not_found.hpp"
#include "checker/exceptions/invalid_expression.hpp"
#include "checker/exceptions/invalid_function.hpp"
#include "checker/exceptions/invalid_type.hpp"


namespace avalon {
/**
 * set_type_instance
 * given an expression, set the given type instance on it
 * possibly overriding the existing one if necessary
 */
static void set_type_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name);

static void set_tuple_instance(std::shared_ptr<tuple_expression> const & tup_expr, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    // we also make that new type instance is complete
    if(new_instance.is_complete() == false)
        throw invalid_type(new_instance.get_token(), "The given type instance cannot be on expressions unless it is complete.");

    // since the tuple inferred type instance is incomplete, we replace it with the new complete type instance
    tup_expr -> set_type_instance(new_instance, false);

    // we go over the elements in the tuple looking for expressions with incomplete type instances to replace them
    std::vector<std::pair<std::string, std::shared_ptr<expr> > >& elements = tup_expr -> get_elements();
    std::vector<type_instance>& new_instances = new_instance.get_params();
    auto elem_it = elements.begin(), elem_end = elements.end();
    auto ins_it = new_instances.begin(), ins_end = new_instances.end();
    for(; ins_it != ins_end && elem_it != elem_end; ++ins_it, ++elem_end)
        set_type_instance(elem_it -> second, * ins_it, l_scope, ns_name, sub_ns_name);
}

static void set_tuple_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    std::shared_ptr<tuple_expression> const & tup_expr = std::static_pointer_cast<tuple_expression>(an_expression);
    set_tuple_instance(tup_expr, new_instance, l_scope, ns_name, sub_ns_name);
}

static void set_list_instance(std::shared_ptr<list_expression> const & list_expr, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    // we also make that new type instance is complete
    if(new_instance.is_complete() == false)
        throw invalid_type(new_instance.get_token(), "The given type instance cannot be on expressions unless it is complete.");

    // since the list inferred type instance is incomplete, we replace it with the new complete type instance
    list_expr -> set_type_instance(new_instance, false);

    // we go over the elements in the list looking for expressions with incomplete type instances to replace them
    std::vector<std::shared_ptr<expr> >& elements = list_expr -> get_elements();
    std::vector<type_instance>& new_instances = new_instance.get_params();
    type_instance& elem_instance = new_instances[0];
    for(auto& elem : elements)
        set_type_instance(elem, elem_instance, l_scope, ns_name, sub_ns_name);
}

static void set_list_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    std::shared_ptr<list_expression> const & list_expr = std::static_pointer_cast<list_expression>(an_expression);
    set_list_instance(list_expr, new_instance, l_scope, ns_name, sub_ns_name);
}

static void set_map_instance(std::shared_ptr<map_expression> const & map_expr, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    // we also make that new type instance is complete
    if(new_instance.is_complete() == false)
        throw invalid_type(new_instance.get_token(), "The given type instance cannot be on expressions unless it is complete.");

    // since the map inferred type instance is incomplete, we replace it with the new complete type instance
    map_expr -> set_type_instance(new_instance, false);

    // we go over the elements in the map looking for expressions with incomplete type instances to replace them
    std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& elements = map_expr -> get_elements();
    std::vector<type_instance>& new_instances = new_instance.get_params();
    type_instance& key_instance = new_instances[0];
    type_instance& value_instance = new_instances[1];
    for(auto& elem : elements) {
        set_type_instance(elem.first, key_instance, l_scope, ns_name, sub_ns_name);
        set_type_instance(elem.second, value_instance, l_scope, ns_name, sub_ns_name);
    }
}

static void set_map_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    std::shared_ptr<map_expression> const & map_expr = std::static_pointer_cast<map_expression>(an_expression);
    set_map_instance(map_expr, new_instance, l_scope, ns_name, sub_ns_name);
}

static void replace_instance(type_instance& dest, type_instance& orig) {
    if(dest.is_abstract()) {
        std::vector<type_instance>& params = orig.get_params();
        for(auto& param : params) {
            if(dest.get_token() == param.get_old_token())
                dest = param;
        }
    }
    else if(dest.is_parametrized()) {
        std::vector<type_instance>& params = dest.get_params();
        for(auto& param : params)
            replace_instance(param, orig);
    }
}

static void set_call_instance(std::shared_ptr<call_expression> const & call_expr, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    // we quickly exist if the call expression is a function call instead of a default or record constructor expression
    if(call_expr -> get_expression_type() == FUNCTION_CALL_EXPR)
        return;

    // we also make that new type instance is complete
    if(new_instance.is_complete() == false)
        throw invalid_type(new_instance.get_token(), "The given type instance cannot be on expressions unless it is complete.");

    // since the call inferred type instance is incomplete, we replace it with the new complete type instance
    call_expr -> set_type_instance(new_instance, false);

    // assume we have a default constructor expression
    if(call_expr -> get_expression_type() == DEFAULT_CONSTRUCTOR_EXPR) {
        std::vector<std::pair<token, std::shared_ptr<expr> > >& cons_args = call_expr -> get_arguments();
        default_constructor& cons = l_scope -> get_default_constructor(sub_ns_name, call_expr -> get_name(), cons_args.size());
        std::vector<type_instance>& cons_params = cons.get_params();

        auto param_it = cons_params.begin(), param_end = cons_params.end();
        auto arg_it = cons_args.begin(), arg_end = cons_args.end();
        for(; param_it != param_end && arg_it != arg_end; ++param_it, ++arg_it) {
            type_instance final_instance = * param_it;
            replace_instance(final_instance, new_instance);
            try {
                type_instance_checker::complex_check(final_instance, l_scope, ns_name);
            } catch(invalid_type err) {
                throw invalid_expression(arg_it -> second -> expr_token(), "The given type instance for this expression is not valid. Details: " + std::string(err.what()));
            }
            type_instance& inferred_instance = arg_it -> second -> expr_type_instance();
            if(type_instance_weak_compare(inferred_instance, final_instance) == false) {
                throw invalid_expression(arg_it -> second -> expr_token(), "The given type instance for this expression <" + mangle_type_instance(final_instance) + "> is not the same as the one inferred <" + mangle_type_instance(inferred_instance) + ">.");
            }
            set_type_instance(arg_it -> second, final_instance, l_scope, ns_name, sub_ns_name);
        }
    }

    // assuming we have a record constructor expression
    if(call_expr -> get_expression_type() == RECORD_CONSTRUCTOR_EXPR) {
        std::vector<std::pair<token, std::shared_ptr<expr> > >& cons_args = call_expr -> get_arguments();
        record_constructor& cons = l_scope -> get_record_constructor(sub_ns_name, call_expr -> get_name(), cons_args.size());
        std::map<token, type_instance>& cons_params = cons.get_params();

        for(auto& cons_arg : cons_args) {
            type_instance final_instance = cons_params[cons_arg.first];
            replace_instance(final_instance, new_instance);
            try {
                type_instance_checker::complex_check(final_instance, l_scope, ns_name);
            } catch(invalid_type err) {
                throw invalid_expression(cons_arg.second -> expr_token(), "The given type instance for this expression is not valid. Details: " + std::string(err.what()));
            }
            type_instance& inferred_instance = cons_arg.second -> expr_type_instance();
            if(type_instance_weak_compare(inferred_instance, final_instance) == false) {
                throw invalid_expression(cons_arg.second -> expr_token(), "The given type instance for this expression <" + mangle_type_instance(final_instance) + "> is not the same as the one inferred <" + mangle_type_instance(inferred_instance) + ">.");
            }
            set_type_instance(cons_arg.second, final_instance, l_scope, ns_name, sub_ns_name);
        }
    }
}

static void set_call_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(an_expression);
    set_call_instance(call_expr, new_instance, l_scope, ns_name, sub_ns_name);
}

static void set_identifier_instance(std::shared_ptr<identifier_expression> const & id_expr, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    // variable expressions always have a complete type instances so we don't work on them
    if(id_expr -> get_expression_type() == VAR_EXPR)
        return;

    // we also make that new type instance is complete
    if(new_instance.is_complete() == false)
        throw invalid_type(new_instance.get_token(), "The given type instance cannot be on expressions unless it is complete.");

    // since the identifier inferred type instance is incomplete, we replace it with the new complete type instance
    id_expr -> set_type_instance(new_instance, false);
}

static void set_identifier_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(an_expression);
    set_identifier_instance(id_expr, new_instance, l_scope, ns_name, sub_ns_name);
}

static void set_grouped_instance(std::shared_ptr<grouped_expression> const & group_expr, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    // we also make that new type instance is complete
    if(new_instance.is_complete() == false)
        throw invalid_type(new_instance.get_token(), "The given type instance cannot be on expressions unless it is complete.");

    // since the grouped inferred type instance is incomplete, we replace it with the new complete type instance
    group_expr -> set_type_instance(new_instance);

    // work on the contained value
    std::shared_ptr<expr>& value = group_expr -> get_value();
    set_type_instance(value, new_instance, l_scope, ns_name, sub_ns_name);
}

static void set_grouped_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    std::shared_ptr<grouped_expression> const & id_expr = std::static_pointer_cast<grouped_expression>(an_expression);
    set_grouped_instance(id_expr, new_instance, l_scope, ns_name, sub_ns_name);
}

void set_type_instance(std::shared_ptr<expr>& an_expression, type_instance& new_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::string& sub_ns_name) {
    if(
       an_expression -> is_underscore_expression()  ||
       an_expression -> is_literal_expression()     ||
       an_expression -> is_cast_expression()        ||
       an_expression -> is_unary_expression()
    ) {
        return;
    }
    else if(an_expression -> is_tuple_expression()) {
        set_tuple_instance(an_expression, new_instance, l_scope, ns_name, sub_ns_name);
    }
    else if(an_expression -> is_list_expression()) {
        set_list_instance(an_expression, new_instance, l_scope, ns_name, sub_ns_name);
    }
    else if(an_expression -> is_map_expression()) {
        set_map_instance(an_expression, new_instance, l_scope, ns_name, sub_ns_name);
    }
    else if(an_expression -> is_call_expression()) {
        set_call_instance(an_expression, new_instance, l_scope, ns_name, sub_ns_name);
    }
    else if(an_expression -> is_identifier_expression()) {
        set_identifier_instance(an_expression, new_instance, l_scope, ns_name, sub_ns_name);
    }
    else if(an_expression -> is_grouped_expression()) {
        set_grouped_instance(an_expression, new_instance, l_scope, ns_name, sub_ns_name);
    }
    else {
        throw std::runtime_error("[compiler error] unexpected expression type in inference engine.");
    }
}

static void build_type_instance(type_instance& dest_instance, type_instance cons_instance, type_instance expr_instance, token err_tok) {
    // if the type instance from the constructor is abstract,
    // then it is a type instance built from a type parameter (done in the inferer below)
    // we search for it on the destination type instance to perform a replacement with the type instance from the expression
    if(cons_instance.is_abstract() == true) {
        std::vector<type_instance>& dest_params = dest_instance.get_params();
        // we try to find the constructor instance amond the type parameters given to us on the destination instance
        for(auto& dest_param : dest_params) {
            // if we find the constructor type instance, we perform the replacement
            if(dest_param.get_token() == cons_instance.get_token()) {
                dest_param = expr_instance;
            }
            // if we cannot find the constructor type instance, we try to see if the replacement was not done already                
            else {
                // if it was already done, we make sure the new type instance (it exists on the expression) is equal to the existing one
                if(cons_instance.get_token() == dest_param.get_old_token()) {
                    if(type_instance_weak_compare(dest_param, expr_instance) == false)
                        throw invalid_expression(err_tok, "This expression is not of the expected type instance");
                }
            }
        }
    }
    // if it has a type, then we compare it with the type instance from the expression
    else {
        // if the type instance of the expression and that of the contructor parameter are different, no point continuing
        if(type_instance_weak_compare(cons_instance, expr_instance) == false) {
            throw invalid_expression(err_tok, "This expression has type instance <" + mangle_type_instance(expr_instance) + "> while the expected type instance is <" + mangle_type_instance(cons_instance) + ">.");
        }
        // if the type instance from the constructor compares weakly from the type instance from the expression,
        // we check to see if the type instance from the expression is a concrete type before proceeding
        else {
            std::vector<type_instance>& cons_params = cons_instance.get_params();
            std::vector<type_instance>& expr_params = expr_instance.get_params();

            auto cons_it = cons_params.begin(), cons_end = cons_params.end();
            auto expr_it = expr_params.begin(), expr_end = expr_params.end();
            for(; cons_it != cons_end && expr_it != expr_end; ++cons_it, ++expr_it)
                build_type_instance(dest_instance, * cons_it, * expr_it, err_tok);
        }
    }
}    

static type_instance build_function(function& new_fun, const token& error_tok, const std::string& name, std::vector<type_instance>& args_instances, type_instance& ret_instance, std::vector<type_instance>& constraint_instances, std::vector<token>& standins, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
    std::shared_ptr<function> fun = nullptr;

    // we try to find the function        
    try {
        fun = find_function(name, args_instances, ret_instance, l_scope, ns_name, standins);
    } catch(symbol_not_found err) {
        throw invalid_expression(error_tok, err.what());
    } catch(symbol_can_collide err) {
        throw invalid_expression(error_tok, err.what());
    } catch(invalid_type err) {
        throw invalid_expression(error_tok, "No function declaration that corresponds to this function call was found. Reason: " + std::string(err.what()));
    }

    // we generate a new function from the function we found
    new_fun.shallow_copy(* fun);
    function_generator generator(new_fun, constraint_instances, l_scope, ns_name);
    try {
        generator.generate(args_instances, ret_instance);
    } catch(invalid_type err) {
        throw invalid_expression(err.get_token(), err.what());
    } catch(invalid_function err) {
        throw invalid_expression(err.get_token(), err.what());
    }

    // set a new name that reflects the arguments instances
    new_fun.set_name(mangle_function(new_fun));
    std::shared_ptr<scope>& f_scope = new_fun.get_scope();
    f_scope -> set_origin(new_fun.get_name());

    // check the new function
    function_checker f_checker;
    try {
        const std::string& fun_ns_name = new_fun.get_namespace();
        f_checker.check(new_fun, fun_ns_name);
    } catch(invalid_function err) {
        throw invalid_expression(err.get_token(), err.what());
    }

    // add the specialization to the root function
    fun -> add_specialization(new_fun);
    fun -> is_used(true);

    // the return type of the function is the type instance of the expression
    type_instance& fun_instance = new_fun.get_return_type_instance();

    // typecheck the inferred type instance
    try {
        type_instance_checker::complex_check(fun_instance, l_scope, ns_name);
    } catch(invalid_type err) {
        throw invalid_expression(err.get_token(), err.what());
    }

    return fun_instance;
}


/**
 * the default constructor expects nothing
 */
inferer::inferer() {    
}

    /**
     * infer
     * given an expression, this function infers the type instance of said expression and returns it
     */
    type_instance inferer::infer(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        if(an_expression -> is_underscore_expression()) {
            return infer_underscore(an_expression);
        }
        else if(an_expression -> is_reference_expression()) {
            return infer_reference(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_dereference_expression()) {
            return infer_dereference(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_literal_expression()) {
            return infer_literal(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_tuple_expression()) {
            return infer_tuple(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_list_expression()) {
            return infer_list(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_map_expression()) {
            return infer_map(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_call_expression()) {
            return infer_call(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_identifier_expression()) {
            return infer_identifier(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_grouped_expression()) {
            return infer_grouping(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_cast_expression()) {
            return infer_cast(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_unary_expression()) {
            return infer_unary(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_binary_expression()) {
            return infer_binary(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_match_expression()) {
            return infer_match(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_assignment_expression()) {
            return infer_assignment(an_expression, l_scope, ns_name);
        }
        else {
            throw std::runtime_error("[compiler error] unexpected expression type in inference engine.");
        }
    }

    /**
     * infer_underscore
     * infers the type instance of an underscopre expression, which is just a type instance without any type
     */
    type_instance inferer::infer_underscore(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<underscore_expression> const & und_expr = std::static_pointer_cast<underscore_expression>(an_expression);
        token gen_tok(MUL, "*", 0, 0, "__bit__");
        type_instance gen_instance(gen_tok, "*");
        und_expr -> set_type_instance(gen_instance);
        return gen_instance;
    }

    /**
     * infer_reference
     * infers the type instance of a reference
     */
    type_instance inferer::infer_reference(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<reference_expression> const & ref_expr = std::static_pointer_cast<reference_expression>(an_expression);
        std::shared_ptr<expr>& val = ref_expr -> get_val();

        // we infer the type instance that we will refer to
        type_instance val_instance = infer(val, l_scope, ns_name);

        // create a new type instance that refers to the type instance of the expression referenced
        avalon_ref avl_ref;
        type_instance ref_instance =  avl_ref.get_type_instance(val_instance);

        return ref_instance;
    }

    /**
     * infer_reference
     * infers the type instance of a dereference
     */
    type_instance inferer::infer_dereference(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<dereference_expression> const & dref_expr = std::static_pointer_cast<dereference_expression>(an_expression);
        std::shared_ptr<variable>& var = dref_expr -> get_variable();
        type_instance var_instance = var -> get_type_instance();
        type_instance dref_instance = var_instance.get_params()[0];
        return dref_instance;
    }

    /**
     * infer_literal
     * infers the type instance of a literal
     */
    type_instance inferer::infer_literal(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<literal_expression> const & lit_expr = std::static_pointer_cast<literal_expression>(an_expression);

        // if the expression already has a type instance set, we return that
        if(lit_expr -> type_instance_from_parser() == false && lit_expr -> has_type_instance() == true) {
            return lit_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(lit_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = lit_expr -> get_type_instance();

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(err.get_token(), err.what());
            }
        }
        
        type_instance inferred_type_instance;
        if(lit_expr -> get_expression_type() == INTEGER_EXPR) {
            avalon_int avl_int;
            inferred_type_instance =  avl_int.get_type_instance();
        }
        else if(lit_expr -> get_expression_type() == FLOATING_POINT_EXPR) {
            avalon_float avl_float;
            inferred_type_instance =  avl_float.get_type_instance();
        }
        else if(lit_expr -> get_expression_type() == STRING_EXPR) {
            avalon_string avl_string;
            inferred_type_instance =  avl_string.get_type_instance();
        }
        else if(lit_expr -> get_expression_type() == BIT_EXPR) {
            const std::string& val = lit_expr -> get_value();
            if(val.length() == 1) {
                avalon_bit avl_bit;
                inferred_type_instance = avl_bit.get_type_instance();
            }
            else if(val.length() == 2) {
                avalon_bit2 avl_bit2;
                inferred_type_instance = avl_bit2.get_type_instance();
            }
            else if(val.length() == 4) {
                avalon_bit4 avl_bit4;
                inferred_type_instance = avl_bit4.get_type_instance();
            }
            else if(val.length() == 8) {
                avalon_bit8 avl_bit8;
                inferred_type_instance = avl_bit8.get_type_instance();
            }
            else {
                throw invalid_expression(lit_expr -> get_token(), "Only bitsets of length 1, 2, 4 and 8 are currently supported.");
            }
        }
        else if(lit_expr -> get_expression_type() == QUBIT_EXPR) {
            const std::string& val = lit_expr -> get_value();
            if(val.length() == 1) {
                avalon_qubit avl_qubit;
                inferred_type_instance = avl_qubit.get_type_instance();
            }
            else if(val.length() == 2) {
                avalon_qubit2 avl_qubit2;
                inferred_type_instance = avl_qubit2.get_type_instance();
            }
            else if(val.length() == 4) {
                avalon_qubit4 avl_qubit4;
                inferred_type_instance = avl_qubit4.get_type_instance();
            }
            else if(val.length() == 8) {
                avalon_qubit8 avl_qubit8;
                inferred_type_instance = avl_qubit8.get_type_instance();
            }
            else {
                throw invalid_expression(lit_expr -> get_token(), "Only qubits of length 1, 2, 4 and 8 are currently supported.");
            }
        }
        else {
            throw std::runtime_error("[compiler error] unexpected literal expression in inference engine.");
        }

        // typecheck the inferred type instance
        try {
            type_instance_checker::complex_check(inferred_type_instance, l_scope, ns_name);
        } catch(invalid_type err) {
            throw invalid_expression(err.get_token(), err.what());
        }

        // if we have type instance from the parser we compare it with the inferred type
        // and if they equal, we keep the parser type instance
        if(has_parser_type_instance) {
            if(type_instance_weak_compare(parser_type_instance, inferred_type_instance) == false) {
                throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied along the expression: <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one deduced by the inference engine: <" + mangle_type_instance(inferred_type_instance) + ">.");
            }
            else {
                return parser_type_instance;
            }
        }
        else {
            lit_expr -> set_type_instance(inferred_type_instance, false);
            return inferred_type_instance;
        }
    }

    /**
     * infer_tuple
     * infers the type instance of a tuple
     */
    type_instance inferer::infer_tuple(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<tuple_expression> const & tup_expr = std::static_pointer_cast<tuple_expression>(an_expression);
        
        // if the expression already has a type instance set, we return that
        if(tup_expr -> type_instance_from_parser() == false && tup_expr -> has_type_instance() == true) {
            return tup_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(tup_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = tup_expr -> get_type_instance();

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(err.get_token(), err.what());
            }
        }

        // we create a type and type instance out of the expression dynamically
        avalon_tuple avl_tuple;
        type_instance inferred_type_instance =  avl_tuple.get_type_instance();
        inferred_type_instance.is_parametrized(false);

        // we fill in the type instance parameters
        std::vector<std::pair<std::string, std::shared_ptr<expr> > >& elements = tup_expr -> get_elements();
        for(auto& element : elements) {
            type_instance el_instance = infer(element.second, l_scope, ns_name);
            inferred_type_instance.add_param(el_instance);
            if(el_instance.is_parametrized())
                inferred_type_instance.is_parametrized(true);
        }

        // typecheck the inferred type instance
        try {
            type_instance_checker::complex_check(inferred_type_instance, l_scope, ns_name);
        } catch(invalid_type err) {
            throw invalid_expression(err.get_token(), err.what());
        }

        // if we have type instance from the parser we compare it with the inferred type
        // and if they equal, we keep the parser type instance
        if(has_parser_type_instance) {
            if(type_instance_weak_compare(parser_type_instance, inferred_type_instance) == false) {
                throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied along the expression: <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one deduced by the inference engine: <" + mangle_type_instance(inferred_type_instance) + ">.");
            }
            else {
                set_tuple_instance(tup_expr, parser_type_instance, l_scope, ns_name, ns_name);
                return parser_type_instance;
            }
        }
        else {
            tup_expr -> set_type_instance(inferred_type_instance, false);
            return inferred_type_instance;
        }
    }

    /**
     * infer_list
     * infers the type instance of a list
     */
    type_instance inferer::infer_list(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<list_expression> const & list_expr = std::static_pointer_cast<list_expression>(an_expression);
        
        // if the expression already has a type instance set, we return that
        if(list_expr -> type_instance_from_parser() == false && list_expr -> has_type_instance() == true) {
            return list_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(list_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = list_expr -> get_type_instance();

            // make sure the type instance is of type LIST
            if(parser_type_instance.get_category() != LIST) {
                throw invalid_expression(parser_type_instance.get_token(), "Excepted a list type instance to be attached to a list expression.");
            }

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(err.get_token(), err.what());
            }
        }

        // if the list is empty, we return the type instance on the list
        // it can be a parser provided one or the generic type instance (signaling a lack of user provided type instance)
        std::vector<std::shared_ptr<expr> >& elements = list_expr -> get_elements();
        if(elements.size() == 0) {
            return list_expr -> get_type_instance();
        }

        // if we do have at least one element in the list, then its type instance is that of the first element
        // we leave it to the check to make sure other elements have the same type instance as the first one
        std::shared_ptr<expr>& first_element = elements[0];
        type_instance first_element_instance = infer(first_element, l_scope, ns_name);
        const std::vector<token>& standins = first_element_instance.get_type() -> get_params();

        // we build the list type instance
        avalon_list avl_list(first_element_instance);
        type_instance inferred_type_instance =  avl_list.get_type_instance();
        inferred_type_instance.is_parametrized(false);
        for(auto& element : elements) {
            type_instance elem_instance = infer(element, l_scope, ns_name);
            if(elem_instance.is_parametrized())
                inferred_type_instance.is_parametrized(true);
        }

        // typecheck the inferred type instance        
        try {
            type_instance_checker::complex_check(inferred_type_instance, l_scope, ns_name, standins);
        } catch(invalid_type err) {
            throw invalid_expression(err.get_token(), err.what());
        }

        // if we have type instance from the parser we compare it with the inferred type
        // and if they equal, we keep the parser type instance
        if(has_parser_type_instance) {
            if(type_instance_weak_compare(parser_type_instance, inferred_type_instance) == false) {
                throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied along the expression: <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one deduced by the inference engine: <" + mangle_type_instance(inferred_type_instance) + ">.");
            }
            else {
                set_list_instance(list_expr, parser_type_instance, l_scope, ns_name, ns_name);
                return parser_type_instance;
            }
        }
        else {
            list_expr -> set_type_instance(inferred_type_instance, false);
            return inferred_type_instance;
        }
    }

    /**
     * infer_map
     * infers the type instance of a map
     */
    type_instance inferer::infer_map(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<map_expression> const & map_expr = std::static_pointer_cast<map_expression>(an_expression);
        
        // if the expression already has a type instance set, we return that
        if(map_expr -> type_instance_from_parser() == false && map_expr -> has_type_instance() == true) {
            return map_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(map_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = map_expr -> get_type_instance();

            // make sure the type instance is of type MAP
            if(parser_type_instance.get_category() != MAP) {
                throw invalid_expression(parser_type_instance.get_token(), "Excepted a map type instance to be attached to a map expression.");
            }

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(err.get_token(), err.what());
            }
        }

        // if the map is empty, we return the type instance on the map
        // it can be a parser provided one or the generic type instance (signaling a lack of user provided type instance)
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& elements = map_expr -> get_elements();
        if(elements.size() == 0) {
            return map_expr -> get_type_instance();
        }

        // if we do have at least one element in the map, then its type instance is that of the first element
        // we leave it to the check to make sure other elements have the same type instance as the first one
        std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> >& first_element = elements[0];
        type_instance first_element_key_instance = infer(first_element.first, l_scope, ns_name);
        type_instance first_element_value_instance = infer(first_element.second, l_scope, ns_name);

        // we build the map type instance
        avalon_map avl_map(first_element_key_instance, first_element_value_instance);
        type_instance inferred_type_instance =  avl_map.get_type_instance();
        if(first_element_key_instance.is_parametrized() || first_element_value_instance.is_parametrized())
            inferred_type_instance.is_parametrized(true);

        // typecheck the inferred type instance
        try {
            type_instance_checker::complex_check(inferred_type_instance, l_scope, ns_name);
        } catch(invalid_type err) {
            throw invalid_expression(err.get_token(), err.what());
        }

        // if we have type instance from the parser we compare it with the inferred type
        // and if they equal, we keep the parser type instance
        type_instance final_instance = inferred_type_instance;
        if(has_parser_type_instance) {
            if(type_instance_weak_compare(parser_type_instance, inferred_type_instance) == false) {
                throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied along the expression: <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one deduced by the inference engine: <" + mangle_type_instance(inferred_type_instance) + ">.");
            }
            else {
                set_map_instance(map_expr, parser_type_instance, l_scope, ns_name, ns_name);
                final_instance = parser_type_instance;
            }
        }
        else {
            map_expr -> set_type_instance(inferred_type_instance, false);
        }

        // the int type instance is the base type instance for hashing
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // the equality comparator function returns a boolean
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // deduce the hashing function
        std::string hash_name = "__hash__";
        const token& first_element_tok = first_element.first -> expr_token();
        token hash_tok(first_element_tok.get_type(), hash_name, first_element_tok.get_line(), first_element_tok.get_column(), first_element_tok.get_source_path());

        std::shared_ptr<call_expression> hash_expr = std::make_shared<call_expression>(hash_tok);
        hash_expr -> set_namespace(first_element_key_instance.get_namespace());
        hash_expr -> set_return_type_instance(int_instance);
        hash_expr -> add_argument(star_tok, first_element.first);
        
        try {
            function hash_fun(star_tok);
            infer_function_call(hash_fun, hash_expr, l_scope, ns_name);
            map_expr -> set_callee(hash_fun.get_name());
        } catch(invalid_expression err) {
            throw invalid_expression(err.get_token(), "There needs to be exactly one __hash__ function for <" + mangle_type_instance(first_element_key_instance) + ">. Details : " + std::string(err.what()));
        }

        // deduce the hash comparator function (which is the int equality comparator function)
        function eq_fun(star_tok);
        std::string eq_name = "__eq__";
        token eq_tok(first_element_tok.get_type(), eq_name, first_element_tok.get_line(), first_element_tok.get_column(), first_element_tok.get_source_path());
        std::vector<type_instance> eq_instances { int_instance, int_instance };
        std::vector<type_instance> eq_constraint_instances;
        std::vector<token> eq_standins;
        build_function(eq_fun, eq_tok, eq_name, eq_instances, bool_instance, eq_constraint_instances, eq_standins, l_scope, "*");
        map_expr -> set_comparator_callee(eq_fun.get_name());

        return final_instance;
    }

    /**
     * infer_call
     * infers the type instance of a call expression
     */
    type_instance inferer::infer_call(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(an_expression);
        
        if(call_expr -> get_expression_type() == DEFAULT_CONSTRUCTOR_EXPR) {
            return infer_default_constructor(call_expr, l_scope, ns_name);
        }
        else if(call_expr -> get_expression_type() == RECORD_CONSTRUCTOR_EXPR) {
            return infer_record_constructor(call_expr, l_scope, ns_name);
        }
        else {
            function new_fun(star_tok);
            return infer_function_call(new_fun, call_expr, l_scope, ns_name);
        }
    }

    /**
     * infer_default_constructor
     * infers the type instance of a default constructor expression
     */
    type_instance inferer::infer_default_constructor(std::shared_ptr<call_expression> const & call_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        const std::string& sub_ns_name = call_expr -> get_namespace();

        // if the expression already has a type instance, we return it
        if(call_expr -> type_instance_from_parser() == false && call_expr -> has_type_instance() == true) {
            return call_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(call_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = call_expr -> get_type_instance();

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(parser_type_instance.get_token(), err.what());
            }
        }

        std::vector<std::pair<token, std::shared_ptr<expr> > >& cons_args = call_expr -> get_arguments();
        default_constructor& cons = l_scope -> get_default_constructor(sub_ns_name, call_expr -> get_name(), cons_args.size());
        std::vector<type_instance>& cons_params = cons.get_params();
        std::shared_ptr<type>& cons_type = cons.get_type();
        const std::vector<token>& type_params = cons_type -> get_params();
        const token& type_token = cons_type -> get_token();

        // we create the type instance for which we are going to fill parameters
        type_instance inferred_type_instance(const_cast<token&>(type_token), cons_type, sub_ns_name);
        // at this point, all of this type instance parameters will be abstract types so the instance should be parametrized
        // but since we are going to be replacing those parameters (or some anyway) with other types, it is easier to go from not parametrized to parametrized
        // the logic is this: we start with not parametrized. the moment we encounter a parametrized type, we set this type instance to parametrized
        // doing this like this is O(1) while doing the reverse would be at best O(n)
        inferred_type_instance.is_parametrized(false);
        for(auto& type_param : type_params) {
            type_instance instance_param(const_cast<token&>(type_param), "*");
            instance_param.is_parametrized(true);
            inferred_type_instance.add_param(instance_param);
        }

        // we have a type instance with abstract types from the type builder
        // now we need to replace them with actual types inferred from the constructor expression
        auto param_it = cons_params.begin(), param_end = cons_params.end();
        auto arg_it = cons_args.begin(), arg_end = cons_args.end();
        for(; param_it != param_end && arg_it != arg_end; ++param_it, ++arg_it) {
            // validate the constructor parameter
            try {
                type_instance_checker::complex_check(* param_it, l_scope, ns_name, type_params);
            } catch(invalid_type err) {
                throw invalid_expression(param_it -> get_token(), err.what());
            }
            // infer the argument type instance
            type_instance arg_type_instance = infer(arg_it -> second, l_scope, ns_name);
            build_type_instance(inferred_type_instance, * param_it, arg_type_instance, call_expr -> get_token());
        }

        // typecheck the inferred type instance
        try {
            type_instance_checker::complex_check(inferred_type_instance, l_scope, ns_name, type_params);
        } catch(invalid_type err) {
            throw invalid_expression(call_expr -> get_token(), err.what());
        }

        // if we have type instance from the parser we compare it with the inferred type
        // and if they equal, we keep the parser type instance
        if(has_parser_type_instance) {
            if(type_instance_weak_compare(parser_type_instance, inferred_type_instance) == false) {
                throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied along the expression: <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one deduced by the inference engine: <" + mangle_type_instance(inferred_type_instance) + ">.");
            }
            else {
                set_call_instance(call_expr, parser_type_instance, l_scope, ns_name, sub_ns_name);
                return parser_type_instance;
            }
        }
        else {
            call_expr -> set_type_instance(inferred_type_instance, false);
            return inferred_type_instance;
        }
    }

    /**
     * infer_record_constructor
     * infers the type instance of a record constructor expression
     */
    type_instance inferer::infer_record_constructor(std::shared_ptr<call_expression> const & call_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        const std::string& sub_ns_name = call_expr -> get_namespace();

        // if the expression already has a type instance, we return it
        if(call_expr -> type_instance_from_parser() == false && call_expr -> has_type_instance() == true) {
            return call_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(call_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = call_expr -> get_type_instance();

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(parser_type_instance.get_token(), err.what());
            }
        }

        std::vector<std::pair<token, std::shared_ptr<expr> > >& cons_args = call_expr -> get_arguments();
        record_constructor& cons = l_scope -> get_record_constructor(sub_ns_name, call_expr -> get_name(), cons_args.size());
        std::map<token, type_instance>& cons_params = cons.get_params();
        std::shared_ptr<type>& cons_type = cons.get_type();
        const std::vector<token>& type_params = cons_type -> get_params();
        const token& type_token = cons_type -> get_token();

        // we create the type instance for which we are going to fill parameters
        type_instance inferred_type_instance(const_cast<token&>(type_token), cons_type, sub_ns_name);
        // at this point, all of this type instance parameters will be abstract types so the instance should be parametrized
        // but since we are going to be replacing those parameters (or some anyway) with other types, it is easier to go from not parametrized to parametrized
        // the logic is this: we start with not parametrized. the moment we encounter a parametrized type, we set this type instance to parametrized
        // doing this like this is O(1) while doing the reverse would be at best O(n)
        inferred_type_instance.is_parametrized(false);
        for(auto& type_param : type_params) {
            type_instance instance_param(const_cast<token&>(type_param), "*");
            instance_param.is_parametrized(true);
            inferred_type_instance.add_param(instance_param);
        }

        // we have a type instance with abstract types from the type builder
        // now we need to replace them with actual types inferred from the constructor expression
        for(auto& cons_arg : cons_args) {
            type_instance& cons_param = cons_params[cons_arg.first];
            try {
                type_instance_checker::complex_check(cons_param, l_scope, ns_name, type_params);
            } catch(invalid_type err) {
                throw invalid_expression(cons_param.get_token(), err.what());
            }
            // infer the argument type instance
            type_instance arg_type_instance = infer(cons_arg.second, l_scope, ns_name);
            if(arg_type_instance.is_parametrized())
                inferred_type_instance.is_parametrized(true);            
            build_type_instance(inferred_type_instance, cons_param, arg_type_instance, call_expr -> get_token());
        }

        // typecheck the inferred type instance
        try {
            type_instance_checker::complex_check(inferred_type_instance, l_scope, ns_name, type_params);
        } catch(invalid_type err) {
            throw invalid_expression(err.get_token(), err.what());
        }

        // if we have type instance from the parser we compare it with the inferred type
        // and if they equal, we keep the parser type instance
        if(has_parser_type_instance) {
            if(type_instance_weak_compare(parser_type_instance, inferred_type_instance) == false) {
                throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied along the expression: <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one deduced by the inference engine: <" + mangle_type_instance(inferred_type_instance) + ">.");
            }
            else {
                set_call_instance(call_expr, parser_type_instance, l_scope, ns_name, sub_ns_name);
                return parser_type_instance;
            }
        }
        else {
            call_expr -> set_type_instance(inferred_type_instance, false);
            return inferred_type_instance;
        }
    }

    /**
     * infer_function_call
     * infers the type instance of a function call expression
     */
    type_instance inferer::infer_function_call(function& new_fun, std::shared_ptr<call_expression> const & call_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        const std::string& sub_ns_name = call_expr -> get_namespace();

        // we get all the data we can get from the call expression
        std::vector<std::pair<token, std::shared_ptr<expr> > >& call_args = call_expr -> get_arguments();
        type_instance& ret_instance = call_expr -> get_return_type_instance();
        std::vector<type_instance>& constraint_instances = call_expr -> get_specializations();

        // we build the data we need from the call expression data
        std::vector<type_instance> args_instances;
        for(auto& arg : call_args)
            args_instances.push_back(infer(arg.second, l_scope, ns_name));
        std::vector<token> standins;
        
        // get the type instance
        type_instance fun_instance = build_function(new_fun, call_expr -> get_token(), call_expr -> get_name(), args_instances, ret_instance, constraint_instances, standins, l_scope, sub_ns_name);
        // set the type instance on the expression
        call_expr -> set_type_instance(fun_instance, false);

        // we got the function, its return type is the type instance of a function call expression
        return fun_instance;
    }

    /**
     * infer_identifier
     * infers the type instance of an identifier expression
     */
    type_instance inferer::infer_identifier(std::shared_ptr<expr> & an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(an_expression);
        
        if(id_expr -> get_expression_type() == VAR_EXPR) {
            return infer_variable(id_expr, l_scope, ns_name);
        }
        else {
            return infer_constructor(id_expr, l_scope, ns_name);
        }
    }

    /**
     * infer_variable
     * infers the type instance of a variable expression
     */
    type_instance inferer::infer_variable(std::shared_ptr<identifier_expression> const & id_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        const std::string& sub_ns_name = id_expr -> get_namespace();

        // if the expression already has a type instance, we return it
        if(id_expr -> type_instance_from_parser() == false && id_expr -> has_type_instance() == true) {
            return id_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(id_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = id_expr -> get_type_instance();

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(parser_type_instance.get_token(), err.what());
            }
        }

        try {
            std::shared_ptr<variable>& var_decl = l_scope -> get_variable(sub_ns_name, id_expr -> get_name());
            type_instance& var_instance = var_decl -> get_type_instance();
            std::shared_ptr<scope>& var_scope = (var_decl -> is_global() == true) ? var_decl -> get_scope() : l_scope;

            // typecheck the inferred type instance
            try {
                type_instance_checker::complex_check(var_instance, var_scope, ns_name);
            } catch(invalid_type err) {
                throw invalid_expression(var_decl -> get_token(), std::string(err.what()) + " Did you import the module that contains the type declaration? Do not rely on implicit imports of declarations.");
            }

            // if we have a parser type instance, we make sure it is the same as the one we inferred
            if(has_parser_type_instance) {
                if(type_instance_weak_compare(parser_type_instance, var_instance) == false)
                    throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one inferred <" + mangle_type_instance(var_instance) + ">.");
            }

            // set the type instance on the expression
            id_expr -> set_type_instance(var_instance, false);

            return var_instance;
        } catch(symbol_not_found err) {
            throw invalid_expression(id_expr -> get_token(), err.what());
        }
    }

    /**
     * infer_constructor
     * infers the type instance of an identifier constructor expression
     */
    type_instance inferer::infer_constructor(std::shared_ptr<identifier_expression> const & id_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        const std::string& sub_ns_name = id_expr -> get_namespace();

        // if the expression already has a type instance, we return it
        if(id_expr -> type_instance_from_parser() == false && id_expr -> has_type_instance() == true) {
            return id_expr -> get_type_instance();
        }

        // if on the other hand a type instance from the parser was posted, we type check check it
        type_instance parser_type_instance;
        bool has_parser_type_instance = false;
        if(id_expr -> type_instance_from_parser() == true) {
            has_parser_type_instance = true;
            parser_type_instance = id_expr -> get_type_instance();

            try {
                // we typecheck the parser type instance
                std::pair<bool,bool> res = type_instance_checker::complex_check(parser_type_instance, l_scope, ns_name);
                // we don't allow parametrized type instances to be bound to expressions
                if (res.second == true) {
                    throw invalid_expression(parser_type_instance.get_token(), "Parametrized types cannot be used on expressions.");
                }
            } catch(invalid_type err) {
                throw invalid_expression(parser_type_instance.get_token(), err.what());
            }
        }

        // we build the type instance from the default constructor we got
        // cons data
        default_constructor& cons = l_scope -> get_default_constructor(sub_ns_name, id_expr -> get_name(), 0);
        // type data
        std::shared_ptr<type>& cons_type = cons.get_type();
        const token& type_token = cons_type -> get_token();
        const std::vector<token>& type_params = cons_type -> get_params();
        // instance data
        type_instance cons_instance(const_cast<token&>(type_token), cons_type, sub_ns_name);
        cons_instance.is_parametrized(false);
        for(auto& type_param : type_params) {
            type_instance instance_param(const_cast<token&>(type_param), "*");
            instance_param.is_parametrized(true);
            cons_instance.add_param(instance_param);
        }

        // typecheck the inferred type instance
        try {
            type_instance_checker::complex_check(cons_instance, l_scope, ns_name, type_params);
        } catch(invalid_type err) {
            throw invalid_expression(id_expr -> get_token(), err.what());
        }

        // make sure that if the user set their own type instance, it is equal to the one we inferred
        if(has_parser_type_instance) {
            if(type_instance_weak_compare(parser_type_instance, cons_instance) == false) {                
                throw invalid_expression(parser_type_instance.get_token(), "The type instance supplied <" + mangle_type_instance(parser_type_instance) + "> is not the same as the one inferred <" + mangle_type_instance(cons_instance) + ">.");
            }
            else {
                set_identifier_instance(id_expr, parser_type_instance, l_scope, ns_name, sub_ns_name);
                return parser_type_instance;
            }
        }

        // set the type instance on the expression
        id_expr -> set_type_instance(cons_instance, false);
        return cons_instance;
    }

    /**
     * infer_grouping
     * infers the type instance of a grouped expressino
     */
    type_instance inferer::infer_grouping(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<grouped_expression> const & group_expr = std::static_pointer_cast<grouped_expression>(an_expression);

        // if the expression already has a type instance, we return it
        if(group_expr -> has_type_instance())  {
            return group_expr -> get_type_instance();
        }

        std::shared_ptr<expr>& value = group_expr -> get_value();
        type_instance group_instance = infer(value, l_scope, ns_name);
        group_expr -> set_type_instance(group_instance);
        return group_instance;
    }

    /**
     * infer_cast
     * infers the type instance of a cast expression
     */
    type_instance inferer::infer_cast(std::shared_ptr<expr> & an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<cast_expression> const & cast_expr = std::static_pointer_cast<cast_expression>(an_expression);
        function cast_fun(star_tok);
        return infer_cast(cast_fun, cast_expr, l_scope, ns_name);
    }

    type_instance inferer::infer_cast(function& cast_fun, std::shared_ptr<cast_expression> const & cast_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        type_instance& cast_instance = cast_expr -> get_cast_type_instance();
        std::shared_ptr<expr>& value = cast_expr -> get_val();
        type_instance value_instance = infer(value, l_scope, ns_name);

        const std::string& sub_ns_name = value_instance.get_namespace();        
        std::vector<type_instance> args_instances = {
            value_instance
        };
        
        std::string fun_name = "__cast__";
        std::vector<token> standins;
        std::vector<type_instance> constraint_instances;

        // infering the type instance of a cast expression amounts to finding the function that performs the cast
        return build_function(cast_fun, cast_expr -> get_token(), fun_name, args_instances, cast_instance, constraint_instances, standins, l_scope, sub_ns_name);
    }

    /**
     * infer_unary
     * infers the type instance of a unary expression
     */
    type_instance inferer::infer_unary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<unary_expression> const & unary_expr = std::static_pointer_cast<unary_expression>(an_expression);
        function unary_fun(star_tok);
        type_instance instance = infer_unary(unary_fun, unary_expr, l_scope, ns_name);
        unary_expr -> set_type_instance(instance);
        return instance;
    }

    type_instance inferer::infer_unary(function& unary_fun, std::shared_ptr<unary_expression> const & unary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::string fun_name = "";
        std::shared_ptr<expr>& value = unary_expr -> get_val();

        // construct the name
        if(unary_expr -> get_expression_type() == PLUS_SIGN_EXPR) {
            fun_name = "__pos__";
        }
        else if(unary_expr -> get_expression_type() == MINUS_SIGN_EXPR) {
            fun_name = "__neg__";
        }
        else if(unary_expr -> get_expression_type() == BITWISE_NOT_EXPR) {
            fun_name = "__bnot__";
        }
        else if(unary_expr -> get_expression_type() == LOGICAL_NOT_EXPR) {
            fun_name = "__not__";
        }
        else {
            throw std::runtime_error("[compiler error] unexpected unary operator.");
        }

        // infer the type of the operand
        type_instance op_instance = infer(value, l_scope, ns_name);

        // construct the rest of parameters required to find the function
        const std::string& sub_ns_name = op_instance.get_namespace();
        std::vector<type_instance> args_instances = {
            op_instance
        };        
        std::vector<token> standins;
        std::vector<type_instance> constraint_instances;

        // find the unary function that corresponds to the given parameters
        return build_function(unary_fun, unary_expr -> get_token(), fun_name, args_instances, op_instance, constraint_instances, standins, l_scope, sub_ns_name);
    }

    /**
     * infer_binary
     * infers the type instance of a binary expression
     */
    type_instance inferer::infer_binary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<binary_expression> const & binary_expr = std::static_pointer_cast<binary_expression>(an_expression);
        function binary_fun(star_tok);
        type_instance instance = infer_binary(binary_fun, binary_expr, l_scope, ns_name);
        binary_expr -> set_type_instance(instance);
        return instance;
    }

    /**
     * infer_binary
     * infers the type instance of a binary expression
     */
    type_instance inferer::infer_binary(function& binary_fun, std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        binary_expression_type expr_type = binary_expr -> get_expression_type();

        // work on operations that decay into functions
        if(
           expr_type == PLUS_EXPR           ||
           expr_type == MINUS_EXPR          ||
           expr_type == MUL_EXPR            ||
           expr_type == DIV_EXPR            ||
           expr_type == MOD_EXPR            ||
           expr_type == POW_EXPR            ||
           expr_type == LEFT_SHIFT_EXPR     ||
           expr_type == RIGHT_SHIFT_EXPR    ||
           expr_type == LOGICAL_AND_EXPR    ||
           expr_type == LOGICAL_OR_EXPR     ||
           expr_type == BITWISE_AND_EXPR    ||
           expr_type == BITWISE_OR_EXPR     ||
           expr_type == BITWISE_XOR_EXPR    ||
           expr_type == EQUAL_EQUAL_EXPR    ||
           expr_type == NOT_EQUAL_EXPR      ||
           expr_type == LESS_EXPR           ||
           expr_type == LESS_EQUAL_EXPR     ||
           expr_type == GREATER_EXPR        ||
           expr_type == GREATER_EQUAL_EXPR

        ) {
            return infer_functional_binary(expr_type, binary_fun, binary_expr, l_scope, ns_name);
        }
        else if(expr_type == DOT_EXPR) {
            return infer_dot_binary(binary_expr, l_scope, ns_name);
        }
        else if(expr_type == SUBSCRIPT_EXPR) {
            return infer_subscript_binary(binary_expr, l_scope, ns_name);
        }
        else {
            throw std::runtime_error("[compiler error] unexpected binary operator.");
        }
    }

    /**
     * infer_functional_binary
     * infers the type instance of a binary expression that immediately decays into a function call
     */
    type_instance inferer::infer_functional_binary(binary_expression_type& expr_type, function& binary_fun, std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::string fun_name = "";
        std::shared_ptr<expr>& lval = binary_expr -> get_lval();
        std::shared_ptr<expr>& rval = binary_expr -> get_rval();

        if(expr_type == PLUS_EXPR) {
            fun_name = "__add__";
        }
        else if(expr_type == MINUS_EXPR) {
            fun_name = "__sub__";
        }
        else if(expr_type == MUL_EXPR) {
            fun_name = "__mul__";
        }
        else if(expr_type == DIV_EXPR) {
            fun_name = "__div__";
        }
        else if(expr_type == MOD_EXPR) {
            fun_name = "__mod__";
        }
        else if(expr_type == POW_EXPR) {
            fun_name = "__pow__";
        }
        else if(expr_type == LEFT_SHIFT_EXPR) {
            fun_name = "__lshift__";
        }
        else if(expr_type == RIGHT_SHIFT_EXPR) {
            fun_name = "__rshift__";
        }
        else if(expr_type == LOGICAL_AND_EXPR) {
            fun_name = "__and__";
        }
        else if(expr_type == LOGICAL_OR_EXPR) {
            fun_name = "__or__";
        }
        else if(expr_type == BITWISE_AND_EXPR) {
            fun_name = "__band__";
        }
        else if(expr_type == BITWISE_OR_EXPR) {
            fun_name = "__bor__";
        }
        else if(expr_type == BITWISE_XOR_EXPR) {
            fun_name = "__xor__";
        }
        else if(expr_type == EQUAL_EQUAL_EXPR) {
            fun_name = "__eq__";
        }
        else if(expr_type == NOT_EQUAL_EXPR) {
            fun_name = "__ne__";
        }
        else if(expr_type == LESS_EXPR) {
            fun_name = "__lt__";
        }
        else if(expr_type == LESS_EQUAL_EXPR) {
            fun_name = "__le__";
        }
        else if(expr_type == GREATER_EXPR) {
            fun_name = "__gt__";
        }
        else if(expr_type == GREATER_EQUAL_EXPR) {
            fun_name = "__ge__";
        }
        else {
            throw std::runtime_error("[compiler error] unexpected functional binary operator.");
        }

        // infer the type of the operands
        type_instance lval_instance = infer(lval, l_scope, ns_name);
        type_instance rval_instance = infer(rval, l_scope, ns_name);

        // construct the rest of parameters required to find the function
        const std::string& sub_ns_name = lval_instance.get_namespace();
        std::vector<type_instance> args_instances = {
            lval_instance,
            rval_instance
        };        
        std::vector<token> standins;
        std::vector<type_instance> constraint_instances;

        // find the binary function that corresponds to the given parameters
        token gen_tok(MUL, "*", 0, 0, "__bit__");
        type_instance gen_instance(gen_tok, "*");
        return build_function(binary_fun, binary_expr -> get_token(), fun_name, args_instances, gen_instance, constraint_instances, standins, l_scope, sub_ns_name);
    }

    /**
     * infer_getattr_binary
     * infers the type instance of a binary expression arising from the dot operator
     */
    type_instance inferer::infer_dot_binary(std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = binary_expr -> get_lval();
        std::shared_ptr<expr>& rval = binary_expr -> get_rval();
        const std::string& name = lval -> expr_token().get_lexeme();

        // if the lval is a namespace name
        if(l_scope -> has_namespace(name)) {
            return infer_namespace_binary(name, rval, l_scope, ns_name);
        }
        // if the lval is a variable
        else if(l_scope -> variable_exists(ns_name, name)) {
            return infer_variable_binary(lval, rval, l_scope, ns_name);
        }
        // anything else if an error
        else {
            throw invalid_expression(lval -> expr_token(), "Expected a namespace name or a variable name as lval to a dot expression.");
        }
    }

    /**
     * infer_subscript_binary
     * infers the type instance of a binary expression arising from the subscript operator
     */
    type_instance inferer::infer_subscript_binary(std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = binary_expr -> get_lval();
        std::shared_ptr<expr>& rval = binary_expr -> get_rval();
        const std::string& name = lval -> expr_token().get_lexeme();        

        // if the lval is an identifier expression, then it is a variable expression
        if(l_scope -> variable_exists(ns_name, name)) {
            return infer_variable_subscript(lval, rval, l_scope, ns_name);
        }
        // anything else if an error
        else {
            throw invalid_expression(lval -> expr_token(), "Expected a variable name as lval to a subscript expression.");
        }
    }

    type_instance inferer::infer_namespace_binary(const std::string& sub_ns_name, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        // if the rval is an identifier expression, then it must be either a variable expression or a default constructor expression
        if(rval -> is_identifier_expression()) {
            std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(rval);
            if(l_scope -> variable_exists(sub_ns_name, id_expr -> get_name())) {
               id_expr -> set_namespace(sub_ns_name);
               return infer_variable(id_expr, l_scope, ns_name);
            }
            else if(l_scope -> default_constructor_exists(sub_ns_name, id_expr -> get_name(), 0)) {
                id_expr -> set_namespace(sub_ns_name);
                return infer_constructor(id_expr, l_scope, ns_name);
            }
            else {
                throw invalid_expression(id_expr -> get_token(), "Expected a variable expression or a default constructor expression after the namespace name.");
            }        
        }
        // if it is a call expression, we expect it to be either a function call or a default constructor or a record constructor
        else if(rval -> is_call_expression()) {
            std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(rval);           

            if(l_scope -> function_exists(sub_ns_name, call_expr -> get_name())) {
                call_expr -> set_namespace(sub_ns_name);
                function binary_fun(star_tok);
                return infer_function_call(binary_fun, call_expr, l_scope, ns_name);
            }
            else if(l_scope -> default_constructor_exists(sub_ns_name, call_expr -> get_name(), call_expr -> get_arguments().size())) {
                call_expr -> set_namespace(sub_ns_name);
                return infer_default_constructor(call_expr, l_scope, ns_name);
            }
            else if(l_scope -> record_constructor_exists(sub_ns_name, call_expr -> get_name(), call_expr -> get_arguments().size())) {
                call_expr -> set_namespace(sub_ns_name);
                return infer_record_constructor(call_expr, l_scope, ns_name);
            }
            else {
                throw invalid_expression(call_expr -> get_token(), "Expected a function call or a record constructor.");
            }
        }
        // if the rval is a binary expression then it is either a dot expression or a subscript expression and the lval of that binary expression is a variable expression
        else if(rval -> is_binary_expression()) {
            std::shared_ptr<binary_expression> const & binary_expr = std::static_pointer_cast<binary_expression>(rval);
            binary_expression_type expr_type = binary_expr -> get_expression_type();
            std::shared_ptr<expr>& sub_lval = binary_expr -> get_lval();
            std::shared_ptr<expr>& sub_rval = binary_expr -> get_rval();

            if(sub_lval -> is_identifier_expression()) {
                std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(sub_lval);
                if(l_scope -> variable_exists(sub_ns_name, id_expr -> get_name())) {
                    id_expr -> set_namespace(sub_ns_name);
                    if(expr_type == DOT_EXPR) {
                        return infer_variable_attribute(sub_lval, sub_rval, l_scope, ns_name);
                    }
                    else if(expr_type == SUBSCRIPT_EXPR) {
                        return infer_variable_subscript(sub_lval, sub_rval, l_scope, ns_name);
                    }
                    else {
                        throw invalid_expression(binary_expr -> get_token(), "Excepted either a dot expression or a subscript expression.");
                    }
                }
                else {
                    throw invalid_expression(id_expr -> get_token(), "Excepted the name of a variable.");
                }
            }
            else {
                throw invalid_expression(binary_expr -> get_token(), "The lval of this binary expression must be a variable name.");
            }
        }
        else {
            throw invalid_expression(rval -> expr_token(), "The rval of a dot expression after an lval namespace must be either a variable expression, a function call or a default constructor.");
        }
    }

    type_instance inferer::infer_variable_binary(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        // if the rval is an identifier expression, then it must be an attribute from either a record constructor expression or a named tuple expression
        if(rval -> is_identifier_expression()) {
            return infer_variable_attribute(lval, rval, l_scope, ns_name);
        }
        else {
            throw invalid_expression(rval -> expr_token(), "Expected a record constructor attribute or a named tuple attribute as rval to a dot expression.");
        }
    }

    type_instance inferer::infer_variable_attribute(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
        std::shared_ptr<expr>& var_value = var_decl -> get_value();

        // if the lval contains a named tuple
        if(var_value -> is_tuple_expression()) {
            return infer_tuple_attribute(var_value, rval, l_scope, ns_name);
        }
        else {
            return infer_custom_attribute(lval, rval, l_scope, ns_name);
        }
    }

    type_instance inferer::infer_tuple_attribute(std::shared_ptr<expr>& lval_val, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<tuple_expression> const & tuple_expr = std::static_pointer_cast<tuple_expression>(lval_val);
        std::vector<std::pair<std::string, std::shared_ptr<expr> > >& elements = tuple_expr -> get_elements();
        token rval_tok = rval -> expr_token();        

        for(auto& element : elements) {
            if(element.first == rval_tok.get_lexeme())
                return infer(element.second, l_scope, ns_name);
        }

        throw invalid_expression(rval_tok, "This attribute does not exist in the named tuple contained in the variable being accessed.");
    }

    type_instance inferer::infer_custom_attribute(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        type_instance lval_instance = infer(lval, l_scope, ns_name);

        // we build the function call expression for __getattr__ from which the type instance will be infered
        token rval_tok = rval -> expr_token();
        std::string call_name = "__getattr_" + rval_tok.get_lexeme() + "__";
        token call_tok(rval_tok.get_type(), call_name, rval_tok.get_line(), rval_tok.get_column(), rval_tok.get_source_path());
        std::shared_ptr<call_expression> getattr_expr = std::make_shared<call_expression>(call_tok);
        getattr_expr -> set_namespace(lval_instance.get_namespace());
        // argument is a variable expression
        getattr_expr -> add_argument(star_tok, lval);

        function binary_fun(star_tok);
        return infer_function_call(binary_fun, getattr_expr, l_scope, ns_name);
    }

    type_instance inferer::infer_variable_subscript(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
        type_instance& var_instance = var_decl -> get_type_instance();

        if(var_instance.get_category() == TUPLE) {
            return infer_tuple_subscript(var_instance, rval, l_scope, ns_name);
        }
        else if(var_instance.get_category() == LIST) {
            return infer_list_subscript(var_instance, rval, l_scope, ns_name);
        }
        else if(var_instance.get_category() == MAP) {
            return infer_map_subscript(var_instance, rval, l_scope, ns_name);
        }
        else {
            return infer_custom_subscript(lval, rval, l_scope, ns_name);
        }
    }

    type_instance inferer::infer_tuple_subscript(type_instance& var_instance, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        const token& rval_tok = rval -> expr_token();
        std::size_t key = 0;        
        std::sscanf(rval_tok.get_lexeme().c_str(), "%zu", &key);
        std::vector<type_instance>& params = var_instance.get_params();
        try {
            return params.at(key);
        } catch(std::out_of_range err) {
            throw invalid_expression(rval_tok, "This index is not accessible on the tuple.");
        }
    }

    type_instance inferer::infer_list_subscript(type_instance& var_instance, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<type_instance>& params = var_instance.get_params();
        avalon_maybe avl_maybe;
        type_instance& instance = params[0];
        type_instance ret_instance = avl_maybe.get_type_instance(instance);
        return ret_instance;
    }

    type_instance inferer::infer_map_subscript(type_instance& var_instance, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<type_instance>& params = var_instance.get_params();
        avalon_maybe avl_maybe;
        type_instance& instance = params[1];
        type_instance ret_instance = avl_maybe.get_type_instance(instance);
        return ret_instance;
    }

    type_instance inferer::infer_custom_subscript(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        type_instance lval_instance = infer(lval, l_scope, ns_name);

        // we build the function call expression for __getitem__ from which the type instance will be infered
        token rval_tok = rval -> expr_token();
        std::string call_name = "__getitem_" + rval_tok.get_lexeme() + "__";
        token call_tok(rval_tok.get_type(), call_name, rval_tok.get_line(), rval_tok.get_column(), rval_tok.get_source_path());
        std::shared_ptr<call_expression> getitem_expr = std::make_shared<call_expression>(call_tok);
        getitem_expr -> set_namespace(lval_instance.get_namespace());
        // argument is a variable expression
        getitem_expr -> add_argument(star_tok, lval);
        
        function binary_fun(star_tok);
        return infer_function_call(binary_fun, getitem_expr, l_scope, ns_name);
    }

    /**
     * infer_match
     * infers the type instance of a match expression
     */
    type_instance inferer::infer_match(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        avalon_bool avl_bool;
        return avl_bool.get_type_instance();
    }

    /**
     * infer_assignment
     * infers the type instance of an assigment expression
     */
    type_instance inferer::infer_assignment(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name) {
        std::shared_ptr<assignment_expression> const & assign_expr = std::static_pointer_cast<assignment_expression>(an_expression);
        std::shared_ptr<expr>& lval = assign_expr -> get_lval();
        return infer(lval, l_scope, ns_name);
    }
}
