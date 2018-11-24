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
#include "representer/ast/decl/type.hpp"
/* Expressions */
#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtin programs */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_qubit.hpp"
#include "representer/builtins/lang/avalon_float.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_bit2.hpp"
#include "representer/builtins/lang/avalon_bit4.hpp"
#include "representer/builtins/lang/avalon_bit8.hpp"
#include "representer/builtins/lang/avalon_int.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_string.hpp"
#include "interpreter/builtins/lang/avalon_qubit.hpp"
#include "interpreter/builtins/lang/avalon_float.hpp"
#include "interpreter/builtins/lang/avalon_bool.hpp"
#include "interpreter/builtins/lang/avalon_bit2.hpp"
#include "interpreter/builtins/lang/avalon_bit4.hpp"
#include "interpreter/builtins/lang/avalon_bit8.hpp"
#include "interpreter/builtins/lang/avalon_int.hpp"
#include "interpreter/builtins/lang/avalon_bit.hpp"

/* Quantum processor */
#include "interpreter/qprocessor.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * avl_cast
     * implements the builtin cast function
     */
    std::shared_ptr<expr> avl_cast(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // qubit type
        avalon_qubit avl_qubit;
        type_instance qubit_instance = avl_qubit.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the builtin __cast__ function expects only one argument.");

        // deal with identifier expressions - all of which are default constructors without parametesr
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_identifier_expression()) {
            std::shared_ptr<identifier_expression> const & arg_id = std::static_pointer_cast<identifier_expression>(arg);
            type_instance& arg_instance = arg_id -> get_type_instance();

            // if we have booleans
            if(type_instance_strong_compare(arg_instance, bool_instance)) {
                return bool_cast(arguments, ret_instance);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __cast__ using arguments of unsupported type instances");
            }
        }
        else if(arg -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            type_instance& arg_instance = arg_lit -> get_type_instance();

            if(type_instance_strong_compare(arg_instance, int_instance)) {
                return int_cast(arguments, ret_instance);
            }
            else if(type_instance_strong_compare(arg_instance, float_instance)) {
                return float_cast(arguments, ret_instance);
            }
            else if(type_instance_strong_compare(arg_instance, bit_instance)) {
                return bit_cast(arguments, ret_instance);
            }
            else if(type_instance_strong_compare(arg_instance, string_instance)) {
                return string_cast(arguments, ret_instance);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __cast__ using arguments of unsupported type instances");
            }
        }
        else if(arg -> is_reference_expression()) {
            std::shared_ptr<reference_expression> const & arg_lit = std::static_pointer_cast<reference_expression>(arg);
            type_instance& arg_instance = arg_lit -> get_type_instance();

            type_instance ref_instance = arg_instance.get_params()[0];
            if(type_instance_strong_compare(ref_instance, qubit_instance)) {
                return qubit_cast(qproc, arguments, ret_instance);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __cast__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __cast__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_bool_cast
     * implements cast to bool function
     */
    std::shared_ptr<expr> avl_bool_cast(std::vector<std::shared_ptr<expr> >& arguments) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the <bool> function expects only one argument.");

        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            type_instance& arg_instance = arg_lit -> get_type_instance();

            if(type_instance_strong_compare(arg_instance, string_instance)) {
                return string_bool(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to the <bool> function using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to the <bool> function using arguments of unsupported type instances");
        }
    }

    /**
     * avl_int_cast
     * implements cast to int function
     */
    std::shared_ptr<expr> avl_int_cast(std::vector<std::shared_ptr<expr> >& arguments) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the <int> function expects only one argument.");

        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            type_instance& arg_instance = arg_lit -> get_type_instance();

            if(type_instance_strong_compare(arg_instance, string_instance)) {
                return string_int(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to the <int> function using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to the <int> function using arguments of unsupported type instances");
        }
    }

    /**
     * avl_flaot_cast
     * implements cast to float function
     */
    std::shared_ptr<expr> avl_float_cast(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the <float> function expects only one argument.");

        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_identifier_expression()) {
            throw invalid_call("[compiler error] unexpected call to the <float> function using arguments of unsupported type instances");
        }
        else if(arg -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            type_instance& arg_instance = arg_lit -> get_type_instance();

            if(type_instance_strong_compare(arg_instance, int_instance)) {
                return int_float(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, string_instance)) {
                return string_float(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to the <float> function using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to the <float> function using arguments of unsupported type instances");
        }
    }

    /**
     * avl_string_cast
     * implements cast to string function
     */
    std::shared_ptr<expr> avl_string_cast(std::vector<std::shared_ptr<expr> >& arguments) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // bit2 type
        avalon_bit2 avl_bit2;
        type_instance bit2_instance = avl_bit2.get_type_instance();

        // bit4 type
        avalon_bit4 avl_bit4;
        type_instance bit4_instance = avl_bit4.get_type_instance();

        // bit8 type
        avalon_bit8 avl_bit8;
        type_instance bit8_instance = avl_bit8.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the <string> function expects only one argument.");

        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_identifier_expression()) {
            std::shared_ptr<identifier_expression> const & arg_id = std::static_pointer_cast<identifier_expression>(arg);
            type_instance& arg_instance = arg_id -> get_type_instance();

            // if we have booleans
            if(type_instance_strong_compare(arg_instance, bool_instance)) {
                return bool_string(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to the <string> function using arguments of unsupported type instances");
            }
        }
        else if(arg -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            type_instance& arg_instance = arg_lit -> get_type_instance();

            if(type_instance_strong_compare(arg_instance, int_instance)) {
                return int_string(arguments);
            }
            if(type_instance_strong_compare(arg_instance, float_instance)) {
                return float_string(arguments);
            }
            if(type_instance_strong_compare(arg_instance, bit_instance)) {
                return bit_string(arguments);
            }
            if(type_instance_strong_compare(arg_instance, bit2_instance)) {
                return bit2_string(arguments);
            }
            if(type_instance_strong_compare(arg_instance, bit4_instance)) {
                return bit4_string(arguments);
            }
            if(type_instance_strong_compare(arg_instance, bit8_instance)) {
                return bit8_string(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to the <string> function using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to the <string> function using arguments of unsupported type instances");
        }
    }
}
