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

#include <memory>
#include <string>

#include <iostream>

#include "checker/decl/variable/variable_checker.hpp"
#include "checker/exceptions/invalid_expression.hpp"
#include "checker/expr/expression_checker.hpp"
#include "checker/exceptions/invalid_variable.hpp"
#include "checker/decl/type/type_checker.hpp"
#include "checker/exceptions/invalid_type.hpp"
#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/tuple_expression.hpp"
#include "representer/ast/expr/list_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/map_expression.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"


namespace avalon {
    /**
     * the constructor expects nothing
     */
    variable_checker::variable_checker() {
    }

    /**
     * check
     * validate the variable declaration
     */
    void variable_checker::check(const std::shared_ptr<variable>& variable_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& variable_val = variable_decl -> get_value();
        // we set the scope that will actually be used depending on whether the variable is a global one or not
        // here is how this is important: say we have 3 files A, B and C.
        // in A, we declared variable `var x = 10`, in B we import A and declare `var y = x` and in C we import B and declare `var z = y`.
        // if we use the provided `l_scope`, the checker will fail validating `x` in B since `l_scope` belongs to C.
        // but by using the scope that already exists on `x`, checking will go well.
        std::shared_ptr<scope>& var_scope = (variable_decl -> is_global() == true) ? variable_decl -> get_scope() : l_scope;

        // we only proceed to check the variable if we don't know that it is valid or invalid
        if(variable_decl -> is_valid(UNKNOWN) == false)
            return;

        // we automatically mark the variable as invalid and let it be valid only after the checks below are finished running
        variable_decl -> set_is_valid(INVALID);

        // if the variable can be in an unitiazed state (such as function parameters), we don't bother checking the initializer if there is none
        if(variable_decl -> check_initializer() == false && variable_val == nullptr) {
            // if the variable has a type instance, we check it
            if(variable_decl -> has_type_instance()) {
                type_instance& var_instance = variable_decl -> get_type_instance();
                try {
                    // if we have a function parameter, we use the given scope as it is the function scope
                    type_instance_checker::complex_check(var_instance, var_scope, ns_name);
                } catch(invalid_type err) {
                    throw invalid_variable(err.get_token(), err.what());
                }
            }
            // but if no initializer was provided, we ensure that the variable is typed
            else {
                throw invalid_variable(variable_decl -> get_token(), "Variable declarations must have a type instance.");
            }

            variable_decl -> set_is_valid(VALID);
            return;
        }

        // we make sure the variable is initialized
        if(variable_val == nullptr) {
            throw invalid_variable(variable_decl -> get_token(), "Variable declarations must be initialized.");
        }

        // if the variable has a type instance, we check it
        if(variable_decl -> has_type_instance()) {
            type_instance& var_instance = variable_decl -> get_type_instance();
            try {
                type_instance_checker::complex_check(var_instance, var_scope, ns_name);
            } catch(invalid_type err) {
                throw invalid_variable(err.get_token(), err.what());
            }
        }

        // we check the initializer expression
        try {
            // we make sure that if the initializer is a call expression, it doesn't depend on an underscore expression
            if(variable_val -> is_call_expression()) {
                std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(variable_val);
                if(call_expr -> has_underscore()) {
                    throw invalid_variable(variable_decl -> get_token(), "The variable initializer cannot depend on the underscore expression.");
                }
            }

            // and of course, the initializer can't be the underscore expression
            if(variable_val -> is_underscore_expression()) {
                throw invalid_variable(variable_decl -> get_token(), "The variable initializer cannot be the underscore expression.");
            }

            // we check the type instance of the initializer
            expression_checker e_checker;
            // for checking the expression, if it is a global variable variable, we use the namespace set on the variable
            // otherwise, it is a local variable and it will always have a global namespace set
            std::string sub_ns_name = (variable_decl -> is_global() == true) ? variable_decl -> get_namespace() : "*";
            type_instance expr_instance = e_checker.check(variable_val, var_scope, sub_ns_name);
            
            // if the expression type instance is not complete after checking, we raise an error
            if(expr_instance.is_complete() == false) {
                throw invalid_variable(variable_decl -> get_token(), "The initializer expression to a variable expression must be complete.");
            }
            
            // if the variable has a type instance set, we make sure it is the same as the one on the expression
            if(variable_decl -> has_type_instance()) {
                type_instance& var_instance = variable_decl -> get_type_instance();
                if(type_instance_strong_compare(var_instance, expr_instance) == false) {
                    throw invalid_variable(variable_decl -> get_token(), "The variable has a different type instance <" + mangle_type_instance(var_instance) + "> than the expression it is initialized with <" + mangle_type_instance(expr_instance) + ">.");
                }
            }
            else {
                variable_decl -> set_type_instance(expr_instance);
            }            
        } catch(invalid_expression err) {
            throw invalid_variable(err.get_token(), err.what());
        }

        // string, tuples, lists and maps must always be immutable
        if(variable_val -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & lit_expr = std::static_pointer_cast<literal_expression>(variable_val);
            if(lit_expr -> get_expression_type() == STRING_EXPR && variable_decl -> is_mutable() == true)
                throw invalid_variable(variable_decl -> get_token(), "Variable declaration initialized with string literals must be immutable.");

            if(lit_expr -> get_expression_type() == QUBIT_EXPR && variable_decl -> is_mutable() == true)
                throw invalid_variable(variable_decl -> get_token(), "Variable declaration initialized with qubit literals must be immutable.");
        }
        else if(variable_val -> is_tuple_expression()) {
            if(variable_decl -> is_mutable() == true)
                throw invalid_variable(variable_decl -> get_token(), "Variable declaration initialized with tuple expressions must be immutable.");
        }
        else if(variable_val -> is_list_expression()) {
            if(variable_decl -> is_mutable() == true)
                throw invalid_variable(variable_decl -> get_token(), "Variable declaration initialized with list expressions must be immutable.");
        }
        else if(variable_val -> is_map_expression()) {
            if(variable_decl -> is_mutable() == true)
                throw invalid_variable(variable_decl -> get_token(), "Variable declaration initialized with map expressions must be immutable.");
        }

        // if we have a reference variable, make sure that if it points to an immutable variable, it is also immutable
        type_instance var_instance = variable_decl -> get_type_instance();
        if(var_instance.is_reference()) {
            std::shared_ptr<reference_expression> const & var_expr = std::static_pointer_cast<reference_expression>(variable_val);
            std::shared_ptr<variable>& ref_var = var_expr -> get_variable();
            if(ref_var -> is_mutable() == false && variable_decl -> is_mutable() == true)
                throw invalid_variable(variable_decl -> get_token(), "A mutable reference variable cannot reference an immutable variable.");
        }

        // if we are here, then the variable is valid
        variable_decl -> set_is_valid(VALID);
    }
}
