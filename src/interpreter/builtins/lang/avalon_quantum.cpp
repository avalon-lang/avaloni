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
#include <vector>

/* Lexer */
#include "lexer/token_type.hpp"
#include "lexer/token.hpp"

/* AST */
/* Declarations */
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/type.hpp"
/* Expressions */
#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_qubit.hpp"
#include "representer/builtins/lang/avalon_cgate.hpp"
#include "representer/builtins/lang/avalon_gate.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_quantum.hpp"
#include "interpreter/builtins/lang/avalon_qubit.hpp"

/* Quantum processor */
#include "interpreter/qprocessor.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * avl_apply
     * applies a quantum gate to qubits
     */
    std::shared_ptr<expr> avl_apply(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments) {
        // qubit type
        avalon_qubit avl_qubit;
        type_instance qubit_instance = avl_qubit.get_type_instance();

        // gate type
        avalon_gate avl_gate;
        type_instance gate_instance = avl_gate.get_type_instance();

        // cgate type
        avalon_cgate avl_cgate;
        type_instance cgate_instance = avl_cgate.get_type_instance();

        // if we have two arguments, then we are dealing with the application of a qubit gate
        if(arguments.size() == 2) {
            std::shared_ptr<expr>& arg_one = arguments[0];
            std::shared_ptr<expr>& arg_two = arguments[1];

            // we make sure that the first argument is a constructor expression and the second a reference expression
            if(arg_one -> is_call_expression() && arg_two -> is_reference_expression()) {
                std::shared_ptr<call_expression> const & arg_one_cons = std::static_pointer_cast<call_expression>(arg_one);
                std::shared_ptr<reference_expression> const & arg_two_ref = std::static_pointer_cast<reference_expression>(arg_two);

                // we double check the type instances
                type_instance arg_one_instance = arg_one_cons -> get_type_instance();
                if(type_instance_strong_compare(arg_one_instance, gate_instance) == false)
                    throw invalid_call("[compiler error] the quantum <apply> function expects the first argument to be a gate definition of type <gate>.");

                type_instance arg_two_instance = arg_two_ref -> get_type_instance();
                if(arg_two_instance.is_reference() == false)
                    throw invalid_call("[compiler error] the quantum <apply> function expects the second argument to be a reference to avariable containing qubit(s).");
                
                // start discriminating over the second argument
                type_instance arg_two_instance_dref = arg_two_instance.get_params()[0];
                if(type_instance_strong_compare(arg_two_instance_dref, qubit_instance)) {
                    return qubit_apply(qproc, arguments);
                }
                else {
                    throw invalid_call("[compiler error] the builtin <apply> function did not expect a variable of type instance with reference to <" + mangle_type_instance(arg_two_instance_dref) + "> as a second argument.");
                }
            }
            else {
                throw invalid_call("[compiler error] the builtin <apply> function expecting two arguments requires that the first be a non-controlled gate and the second a reference to qubit(s).");
            }
        }
        // if we are dealing with the three arguments, then we are dealing with the application of 1-Qubit controlled gate
        else if(arguments.size() == 3) {
            std::shared_ptr<expr>& arg_one = arguments[0];
            std::shared_ptr<expr>& arg_two = arguments[1];
            std::shared_ptr<expr>& arg_three = arguments[2];

            // we make sure that the first argument is a constructor expression and the second a reference expression
            if(arg_one -> is_call_expression() && arg_two -> is_reference_expression() && arg_three -> is_reference_expression()) {
                std::shared_ptr<call_expression> const & arg_one_cons = std::static_pointer_cast<call_expression>(arg_one);
                std::shared_ptr<reference_expression> const & arg_two_ref = std::static_pointer_cast<reference_expression>(arg_two);
                std::shared_ptr<reference_expression> const & arg_three_ref = std::static_pointer_cast<reference_expression>(arg_three);

                // we double check the type instances
                type_instance arg_one_instance = arg_one_cons -> get_type_instance();
                if(type_instance_strong_compare(arg_one_instance, cgate_instance) == false)
                    throw invalid_call("[compiler error] the quantum <apply> function expects the first argument to be a controlled gate definition of type <cgate>.");

                type_instance arg_two_instance = arg_two_ref -> get_type_instance();
                if(arg_two_instance.is_reference() == false)
                    throw invalid_call("[compiler error] the quantum <apply> function expects the second argument to be a reference to a 1-Qubit variable.");

                type_instance arg_three_instance = arg_three_ref -> get_type_instance();
                if(arg_three_instance.is_reference() == false)
                    throw invalid_call("[compiler error] the quantum <apply> function expects the thrid argument to be a reference to a 1-Qubit variable.");
                
                // we call the 1-Qubit capply function
                type_instance arg_two_instance_dref = arg_two_instance.get_params()[0];
                type_instance arg_three_instance_dref = arg_three_instance.get_params()[0];
                if(type_instance_strong_compare(arg_two_instance_dref, qubit_instance) && type_instance_strong_compare(arg_three_instance_dref, qubit_instance)) {
                    return qubit_capply(qproc, arguments);
                }
                else {
                    throw invalid_call("[compiler error] the builtin <apply> function did not expect the second argument with reference to <" + mangle_type_instance(arg_two_instance_dref) + "> and thrid argument as reference to <" + mangle_type_instance(arg_three_instance_dref) + ">.");
                }
            }
            else {
                throw invalid_call("[compiler error] the builtin <apply> function expecting three arguments requires that the first be a controlled gate and the second and third as references to 1-Qubit.");
            }
        }
        else {
            throw invalid_call("[compiler error] the builtin <apply> function expects only two or three arguments.");
        }
    }

    /**
     * avl_measure
     * measures qubit(s) returning bits
     */
    std::shared_ptr<expr> avl_measure(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments) {
        // qubit type
        avalon_qubit avl_qubit;
        type_instance qubit_instance = avl_qubit.get_type_instance();

        // make sure we were given only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the builtin <measure> function expects a single argument: a reference to the qubit to measure.");

        // make sure that were were given a reference to a qubit as argument
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_reference_expression()) {
            std::shared_ptr<reference_expression> const & arg_ref = std::static_pointer_cast<reference_expression>(arg);
            type_instance arg_instance = arg_ref -> get_type_instance();
            type_instance arg_instance_dref = arg_instance.get_params()[0];
            if(type_instance_strong_compare(arg_instance_dref, qubit_instance)) {
                return qubit_measure(qproc, arguments);
            }
            else {
                throw invalid_call("[compiler error] the builtin <measure> function did not expect a variable of type instance with reference to <" + mangle_type_instance(arg_instance_dref) + "> as argument.");
            }
        }
        else {
            throw invalid_call("[compiler error] the builtin <measure> function expects a reference to the qubit to measure.");
        }
    }
}
