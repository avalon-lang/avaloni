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
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_int.hpp"
#include "representer/builtins/lang/avalon_dec.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_comparison.hpp"
#include "interpreter/builtins/lang/avalon_bool.hpp"
#include "interpreter/builtins/lang/avalon_int.hpp"
#include "interpreter/builtins/lang/avalon_dec.hpp"
#include "interpreter/builtins/lang/avalon_bit.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * avl_eq
     * returns true if both its arguments are equal
     */
    std::shared_ptr<expr> avl_eq(std::vector<std::shared_ptr<expr> >& arguments) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __eq__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];        
        if(arg_one -> is_identifier_expression() && arg_two -> is_identifier_expression()) {
            // get the identifier expressions
            std::shared_ptr<identifier_expression> const & arg_one_id = std::static_pointer_cast<identifier_expression>(arg_one);
            std::shared_ptr<identifier_expression> const & arg_two_id = std::static_pointer_cast<identifier_expression>(arg_two);
            type_instance& arg_one_instance = arg_one_id -> get_type_instance();
            type_instance& arg_two_instance = arg_two_id -> get_type_instance();

            // if we have booleans
            if(type_instance_strong_compare(arg_one_instance, bool_instance) && type_instance_strong_compare(arg_two_instance, bool_instance)) {
                return bool_eq(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __eq__ using arguments of unsupported type instances");
            }
        }
        else if(arg_one -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
                return int_eq(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, dec_instance) && type_instance_strong_compare(arg_two_instance, dec_instance)) {
                return dec_eq(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, bit_instance) && type_instance_strong_compare(arg_two_instance, bit_instance)) {
                return bit_eq(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __eq__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __eq__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_ne
     * returns true if both its arguments are not equal
     */
    std::shared_ptr<expr> avl_ne(std::vector<std::shared_ptr<expr> >& arguments) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __ne__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];        
        if(arg_one -> is_identifier_expression() && arg_two -> is_identifier_expression()) {
            // get the identifier expressions
            std::shared_ptr<identifier_expression> const & arg_one_id = std::static_pointer_cast<identifier_expression>(arg_one);
            std::shared_ptr<identifier_expression> const & arg_two_id = std::static_pointer_cast<identifier_expression>(arg_two);
            type_instance& arg_one_instance = arg_one_id -> get_type_instance();
            type_instance& arg_two_instance = arg_two_id -> get_type_instance();

            // if we have booleans
            if(type_instance_strong_compare(arg_one_instance, bool_instance) && type_instance_strong_compare(arg_two_instance, bool_instance)) {
                return bool_ne(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __ne__ using arguments of unsupported type instances");
            }
        }
        else if(arg_one -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
                return int_ne(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, dec_instance) && type_instance_strong_compare(arg_two_instance, dec_instance)) {
                return dec_ne(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, bit_instance) && type_instance_strong_compare(arg_two_instance, bit_instance)) {
                return bit_eq(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __ne__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __ne__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_gt
     * returns true if the first argument is greater than the second one
     */
    std::shared_ptr<expr> avl_gt(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __gt__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];        
        if(arg_one -> is_identifier_expression() && arg_two -> is_identifier_expression()) {
            throw invalid_call("[compiler error] unexpected call to builtin function __gt__ using arguments of unsupported type instances");
        }
        else if(arg_one -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
                return int_gt(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, dec_instance) && type_instance_strong_compare(arg_two_instance, dec_instance)) {
                return dec_gt(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __gt__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __gt__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_ge
     * returns true if the first argument is greater or equal to the second one
     */
    std::shared_ptr<expr> avl_ge(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __ge__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];        
        if(arg_one -> is_identifier_expression() && arg_two -> is_identifier_expression()) {
            throw invalid_call("[compiler error] unexpected call to builtin function __ge__ using arguments of unsupported type instances");
        }
        else if(arg_one -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
                return int_ge(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, dec_instance) && type_instance_strong_compare(arg_two_instance, dec_instance)) {
                return dec_ge(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __ge__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __ge__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_lt
     * returns true if the first argument is less than the second one
     */
    std::shared_ptr<expr> avl_lt(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __lt__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];        
        if(arg_one -> is_identifier_expression() && arg_two -> is_identifier_expression()) {
            throw invalid_call("[compiler error] unexpected call to builtin function __lt__ using arguments of unsupported type instances");
        }
        else if(arg_one -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
                return int_lt(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, dec_instance) && type_instance_strong_compare(arg_two_instance, dec_instance)) {
                return dec_lt(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __lt__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __lt__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_le
     * returns true if the first argument is less or equal to the second one
     */
    std::shared_ptr<expr> avl_le(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __le__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];        
        if(arg_one -> is_identifier_expression() && arg_two -> is_identifier_expression()) {
            throw invalid_call("[compiler error] unexpected call to builtin function __le__ using arguments of unsupported type instances");
        }
        else if(arg_one -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
                return int_le(arguments);
            }
            else if(type_instance_strong_compare(arg_one_instance, dec_instance) && type_instance_strong_compare(arg_two_instance, dec_instance)) {
                return dec_le(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __le__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __le__ using arguments of unsupported type instances");
        }
    }
}
