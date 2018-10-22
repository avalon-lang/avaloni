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
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_float.hpp"
#include "representer/builtins/lang/avalon_int.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_arithmetic.hpp"
#include "interpreter/builtins/lang/avalon_string.hpp"
#include "interpreter/builtins/lang/avalon_float.hpp"
#include "interpreter/builtins/lang/avalon_int.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * avl_pos
     * computes the unary positive of an object
     */
    std::shared_ptr<expr> avl_pos(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the builtin __pos__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __pos__ function expects its argument to be a literal.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // decide which function to call based on the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, int_instance)) {
            return int_pos(arguments);
        }
        else if(type_instance_strong_compare(arg_instance, float_instance)) {
            return float_pos(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __pos__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_neg
     * computes the unary negative of an object
     */
    std::shared_ptr<expr> avl_neg(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the builtin __neg__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __neg__ function expects its argument to be a literal.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // decide which function to call based on the arguments type instances
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, int_instance)) {
            return int_neg(arguments);
        }
        else if(type_instance_strong_compare(arg_instance, float_instance)) {
            return float_neg(arguments);
        }
        else if(type_instance_strong_compare(arg_instance, string_instance)) {
            return string_reverse(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __neg__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_add
     * computes the addition of two objects
     */
    std::shared_ptr<expr> avl_add(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __add__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __add__ function expects its arguments to be literals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __add__ function expects its arguments to be literals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
            return int_add(arguments);
        }
        if(type_instance_strong_compare(arg_one_instance, float_instance) && type_instance_strong_compare(arg_two_instance, float_instance)) {
            return float_add(arguments);
        }
        if(type_instance_strong_compare(arg_one_instance, string_instance) && type_instance_strong_compare(arg_two_instance, string_instance)) {
            return string_concat(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __add__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_sub
     * computes the substraction of two objects
     */
    std::shared_ptr<expr> avl_sub(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __sub__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __sub__ function expects its arguments to be literals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __sub__ function expects its arguments to be literals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
            return int_sub(arguments);
        }
        if(type_instance_strong_compare(arg_one_instance, float_instance) && type_instance_strong_compare(arg_two_instance, float_instance)) {
            return float_sub(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __sub__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_mul
     * computes the multiplication of two objects
     */
    std::shared_ptr<expr> avl_mul(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __mul__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __mul__ function expects its arguments to be literals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __mul__ function expects its arguments to be literals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
            return int_mul(arguments);
        }
        if(type_instance_strong_compare(arg_one_instance, float_instance) && type_instance_strong_compare(arg_two_instance, float_instance)) {
            return float_mul(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __mul__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_div
     * computes the division of two objects
     */
    std::shared_ptr<expr> avl_div(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __div__ function expects its arguments to be literals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __div__ function expects its arguments to be literals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
            return int_div(arguments);
        }
        if(type_instance_strong_compare(arg_one_instance, float_instance) && type_instance_strong_compare(arg_two_instance, float_instance)) {
            return float_div(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __div__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_mod
     * computes the modulo of two objects
     */
    std::shared_ptr<expr> avl_mod(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __mod__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __mod__ function expects its arguments to be literals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __mod__ function expects its arguments to be literals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
            return int_mod(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __mod__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_pow
     * computes the power of two objects
     */
    std::shared_ptr<expr> avl_pow(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __pow__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __pow__ function expects its arguments to be literals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __pow__ function expects its arguments to be literals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, int_instance) && type_instance_strong_compare(arg_two_instance, int_instance)) {
            return int_pow(arguments);
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __pow__ using arguments of unsupported type instances");
        }
    }
}
