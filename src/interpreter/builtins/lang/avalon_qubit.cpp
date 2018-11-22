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


#include <complex>
#include <utility>
#include <memory>
#include <vector>
#include <bitset>
#include <cmath>
#include <tuple>

#include <Eigen/Dense>

#include "qpp.h"

/* Lexer */
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
#include "interpreter/builtins/lang/avalon_qubit.hpp"

/* Quantum processor */
#include "interpreter/qprocessor.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    static std::complex<double> get_00(double theta, double phi, double lambda) {
        std::complex<double> i(0, 1);
        std::complex<double> e = std::exp(i * ((-phi - lambda)/2));
        double c = cos(theta / 2);
        return e * c;
    }

    static std::complex<double> get_10(double theta, double phi, double lambda) {
        std::complex<double> i(0, 1);
        std::complex<double> e = std::exp(i * ((phi - lambda)/2));
        double s = sin(theta / 2);
        return e * s;
    }

    static std::complex<double> get_01(double theta, double phi, double lambda) {
        std::complex<double> i(0, 1);
        std::complex<double> e = -std::exp(i * ((-phi + lambda)/2));
        double s = sin(theta / 2);
        return e * s;
    }

    static std::complex<double> get_11(double theta, double phi, double lambda) {
        std::complex<double> i(0, 1);
        std::complex<double> e = std::exp(i * ((phi + lambda)/2));
        double c = cos(theta / 2);
        return e * c;
    }

    /**
     * qubit_apply
     * applies a quantum gate to the qubits stored at the given reference
     */
    std::shared_ptr<expr> qubit_apply(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments) {
        // qubit type
        avalon_qubit avl_qubit;
        type_instance qubit_instance = avl_qubit.get_type_instance();

        // gate type
        avalon_gate avl_gate;
        type_instance gate_instance = avl_gate.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the quantum <apply> function expects only two arguments.");

        // make sure the first argument is a call expression and the second a reference expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_call_expression() == false)
            throw invalid_call("[compiler error] the quantum <apply> function expects the first argument to be a gate definition.");
        if(arg_two -> is_reference_expression() == false)
            throw invalid_call("[compiler error] the quantum <apply> function expects the second argument to be a reference to a quantum variable.");

        // we cast the arguments
        std::shared_ptr<call_expression> const & arg_one_cons = std::static_pointer_cast<call_expression>(arg_one);
        std::shared_ptr<reference_expression> const & arg_two_ref = std::static_pointer_cast<reference_expression>(arg_two);

        // we double check the type instances
        type_instance arg_one_instance = arg_one_cons -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, gate_instance) == false)
            throw invalid_call("[compiler error] the quantum <apply> function expects the first argument to be a gate definition of type <gate>.");

        type_instance arg_two_instance = arg_two_ref -> get_type_instance();
        type_instance arg_two_instance_dref = arg_two_instance.get_params()[0];
        if(type_instance_strong_compare(arg_two_instance_dref, qubit_instance) == false)
            throw invalid_call("[compiler error] the quantum <apply> function expects the second argument to be a reference to a single <qubit> variable. Instead we got <" + mangle_type_instance(arg_two_instance_dref) + ">.");

        // transform the gate definition into an Eigen matrix
        // 1. first, we get the gate parameters and transform them into double values
        std::vector<std::pair<token, std::shared_ptr<expr> > >& gate_args = arg_one_cons -> get_arguments();
        // work on theta
        std::shared_ptr<expr> theta_expr = gate_args[0].second;
        std::shared_ptr<literal_expression> const & theta_lit = std::static_pointer_cast<literal_expression>(theta_expr);
        double theta = theta_lit -> get_float_value();
        // work on phi
        std::shared_ptr<expr> phi_expr = gate_args[1].second;
        std::shared_ptr<literal_expression> const & phi_lit = std::static_pointer_cast<literal_expression>(phi_expr);
        double phi = phi_lit -> get_float_value();
        // work on lambda
        std::shared_ptr<expr> lambda_expr = gate_args[2].second;
        std::shared_ptr<literal_expression> const & lambda_lit = std::static_pointer_cast<literal_expression>(lambda_expr);
        double lambda = lambda_lit -> get_float_value();

        // 2. we built the 2x2 Eigen matrix to be used as gate
        Eigen::MatrixXcd unitary(2, 2);
        unitary(0, 0) = get_00(theta, phi, lambda);
        unitary(1, 0) = get_10(theta, phi, lambda);
        unitary(0, 1) = get_01(theta, phi, lambda);
        unitary(1, 1) = get_11(theta, phi, lambda);

        // we now apply the gate to the ket contained in the second argument
        // 1. get the content of the referenced variable
        std::shared_ptr<variable>& ref_var = arg_two_ref -> get_variable();
        std::shared_ptr<expr>& var_expr = ref_var -> get_value();
        std::shared_ptr<literal_expression> const & qubit_expr = std::static_pointer_cast<literal_expression>(var_expr);
        // we make sure that the qubit was not measured already
        if(qubit_expr -> was_measured())
            throw invalid_call("[compiler error] the quantum <apply> function second argument has already been measured and further gates cannot be applied to it.");

        // 2. apply the gate to qubit
        qproc -> apply(unitary, qubit_expr -> get_start_index(), qubit_expr -> get_end_index());

        // DONE.
        return nullptr;
    }

    /**
     * qubit_capply
     * applies a controlled quantum gate to the qubit given as third argument using the qubit given in second argument as control
     */
    std::shared_ptr<expr> qubit_capply(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments) {
        // qubit type
        avalon_qubit avl_qubit;
        type_instance qubit_instance = avl_qubit.get_type_instance();

        // gate type
        avalon_gate avl_gate;
        type_instance gate_instance = avl_gate.get_type_instance();

        // cgate type
        avalon_cgate avl_cgate;
        type_instance cgate_instance = avl_cgate.get_type_instance();

        // make sure we got only three arguments
        if(arguments.size() != 3)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects only three arguments.");

        // make sure the first argument is a call expression and the second a reference expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        std::shared_ptr<expr>& arg_three = arguments[2];
        if(arg_one -> is_call_expression() == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the first argument to be a gate definition.");
        if(arg_two -> is_reference_expression() == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the second argument to be a reference to a quantum variable.");
        if(arg_two -> is_reference_expression() == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the second argument to be a reference to a quantum variable.");

        // we cast the arguments
        std::shared_ptr<call_expression> const & arg_one_cons = std::static_pointer_cast<call_expression>(arg_one);
        std::shared_ptr<reference_expression> const & arg_two_ref = std::static_pointer_cast<reference_expression>(arg_two);
        std::shared_ptr<reference_expression> const & arg_three_ref = std::static_pointer_cast<reference_expression>(arg_three);

        // we double check the type instances
        type_instance arg_one_instance = arg_one_cons -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, cgate_instance) == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the first argument to be a gate definition of type <cgate>.");

        type_instance arg_two_instance = arg_two_ref -> get_type_instance();
        if(arg_two_instance.is_reference() == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the second argument to be a reference to a single <qubit>.");
        type_instance arg_two_instance_dref = arg_two_instance.get_params()[0];
        if(type_instance_strong_compare(arg_two_instance_dref, qubit_instance) == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the second argument to be a reference to a single <qubit> variable. Instead we got <" + mangle_type_instance(arg_two_instance_dref) + ">.");

        type_instance arg_three_instance = arg_three_ref -> get_type_instance();
        if(arg_three_instance.is_reference() == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the third argument to be a reference to a single <qubit>.");
        type_instance arg_three_instance_dref = arg_three_instance.get_params()[0];
        if(type_instance_strong_compare(arg_three_instance_dref, qubit_instance) == false)
            throw invalid_call("[compiler error] the quantum <apply> function for controlled gates expects the third argument to be a reference to a single <qubit> variable. Instead we got <" + mangle_type_instance(arg_three_instance_dref) + ">.");

        // transform the gate definition into an Eigen matrix
        // 0. We get the gate that parametrizes this controlled gate
        std::vector<std::pair<token, std::shared_ptr<expr> > >& cgate_args = arg_one_cons -> get_arguments();
        std::shared_ptr<call_expression> const & cgate_arg = std::static_pointer_cast<call_expression>(cgate_args[0].second);
        // 1. first, we get the gate parameters and transform them into double values

        std::vector<std::pair<token, std::shared_ptr<expr> > >& gate_args = cgate_arg -> get_arguments();
        // work on theta
        std::shared_ptr<expr> theta_expr = gate_args[0].second;
        std::shared_ptr<literal_expression> const & theta_lit = std::static_pointer_cast<literal_expression>(theta_expr);
        double theta = theta_lit -> get_float_value();
        // work on phi
        std::shared_ptr<expr> phi_expr = gate_args[1].second;
        std::shared_ptr<literal_expression> const & phi_lit = std::static_pointer_cast<literal_expression>(phi_expr);
        double phi = phi_lit -> get_float_value();
        // work on lambda
        std::shared_ptr<expr> lambda_expr = gate_args[2].second;
        std::shared_ptr<literal_expression> const & lambda_lit = std::static_pointer_cast<literal_expression>(lambda_expr);
        double lambda = lambda_lit -> get_float_value();

        // 2. we built the 2x2 Eigen matrix to be used as control gate
        Eigen::MatrixXcd unitary(2, 2);
        unitary(0, 0) = get_00(theta, phi, lambda);
        unitary(1, 0) = get_10(theta, phi, lambda);
        unitary(0, 1) = get_01(theta, phi, lambda);
        unitary(1, 1) = get_11(theta, phi, lambda);

        // we now apply the gate to the ket contained in the second argument
        // 1. get the content of the first referenced variable (argument two)
        std::shared_ptr<variable>& ref_var_two = arg_two_ref -> get_variable();
        std::shared_ptr<expr>& var_expr_two = ref_var_two -> get_value();
        std::shared_ptr<literal_expression> const & qubit_expr_two = std::static_pointer_cast<literal_expression>(var_expr_two);
        // we make sure that the qubit was not measured already
        if(qubit_expr_two -> was_measured())
            throw invalid_call("[compiler error] the quantum <apply> function second argument has already been measured and further gates cannot be applied to it.");

        // 1. get the content of the second referenced variable (argument three)
        std::shared_ptr<variable>& ref_var_three = arg_three_ref -> get_variable();
        std::shared_ptr<expr>& var_expr_three = ref_var_three -> get_value();
        std::shared_ptr<literal_expression> const & qubit_expr_three = std::static_pointer_cast<literal_expression>(var_expr_three);
        // we make sure that the qubit was not measured already
        if(qubit_expr_three -> was_measured())
            throw invalid_call("[compiler error] the quantum <apply> function third argument has already been measured and further gates cannot be applied to it.");

        // 2. apply the controlled operation
        qproc -> capply(unitary, qubit_expr_two -> get_start_index(), qubit_expr_three -> get_end_index());

        // DONE.
        return nullptr;
    }

    /**
     * qubit_measure
     * performs a measure on the qubits stored at the given reference in the Z basis
     */
    std::shared_ptr<expr> qubit_measure(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments) {
        // qubit type
        avalon_qubit avl_qubit;
        type_instance qubit_instance = avl_qubit.get_type_instance();

        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the quantum <measure> function expects only one argument.");

        // make sure the argument a reference expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_reference_expression() == false)
            throw invalid_call("[compiler error] the quantum <measure> function expects the argument to be a reference to a quantum variable.");

        // we cast the argument
        std::shared_ptr<reference_expression> const & arg_ref = std::static_pointer_cast<reference_expression>(arg);

        // we double check the type instance
        type_instance arg_instance = arg_ref -> get_type_instance();
        type_instance arg_instance_dref = arg_instance.get_params()[0];
        if(type_instance_strong_compare(arg_instance_dref, qubit_instance) == false)
            throw invalid_call("[compiler error] the quantum <measure> function expects the argument to be a reference to a single <qubit> variable.");

        // get the qubit held inside the variable
        // 1. get the content of the referenced variable
        std::shared_ptr<variable>& ref_var = arg_ref -> get_variable();
        std::shared_ptr<expr>& var_expr = ref_var -> get_value();
        std::shared_ptr<literal_expression> const & qubit_expr = std::static_pointer_cast<literal_expression>(var_expr);

        // 2. check if the qubit has not been measured before
        // TODO: it doesn't make sense to throw an exception here - best to just return the existing measurement
        if(qubit_expr -> was_measured())
            throw invalid_call("[compiler error] The qubit given has already been measured");

        // 3. perform the measurement
        std::vector<qpp::idx> results = qproc -> measure(qubit_expr -> get_start_index(), qubit_expr -> get_end_index());
        qpp::idx result = results[0];

        // 4. we set the qubit measurement result
        qubit_expr -> was_measured(true);
        qpp::ket l_ket = result == 0 ? qpp::mket({0}) : qpp::mket({1});
        qubit_expr -> set_qubit_value(l_ket);

        // 5. Create a bit expression with the measurement result
        std::string res_str = std::to_string(result);
        token lit_tok(BITS, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, BIT_EXPR, res_str);
        res_lit -> set_type_instance(bit_instance);

        return res_lit;
    }

    /**
     * qubit_cast
     * Performs a measurement of the qubits stored at the given reference in the Z basis
     */
    std::shared_ptr<expr> qubit_cast(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        if(type_instance_strong_compare(ret_instance, bit_instance)) {
            return qubit_measure(qproc, arguments);
        }
        else {
            throw invalid_call("[compiler error] the qubit __cast__ function cannot cast to <" + mangle_type_instance(ret_instance) + ">.");
        }
    }
}
