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
#include "representer/builtins/lang/avalon_gate.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_qubit.hpp"

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
     * avl_apply
     * applies a quantum gate to the qubits stored at the given reference
     */
    std::shared_ptr<expr> avl_apply(std::vector<std::shared_ptr<expr> >& arguments) {
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

        // 2. get the ket it variable references and apply the gate to it
        qpp::ket qubit = qubit_expr -> get_qubit_value();
        qpp::ket result = unitary * qubit;

        // 3. set the new evolved qubit on the literal
        qubit_expr -> set_qubit_value(result);

        // DONE.
        return nullptr;
    }

    /**
     * avl_measure
     * performs a measure on the qubits stored at the given reference in the Z basis
     */
    std::shared_ptr<expr> avl_measure(std::vector<std::shared_ptr<expr> >& arguments) {
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

        // 2. get the ket it variable references and perform a Pauli Z measurement - equivalent to measurement in the computational basis
        qpp::ket qubit = qubit_expr -> get_qubit_value();
        std::tuple<qpp::idx, std::vector<double>, std::vector<qpp::cmat>> result = qpp::measure(qubit, qpp::gt.Z, {0});

        // 3. Create a bit expression with the measurement result
        std::string res_str = std::to_string(std::get<0>(result));
        token lit_tok(BITS, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, BIT_EXPR, res_str);
        res_lit -> set_type_instance(bit_instance);

        return res_lit;
    }
}