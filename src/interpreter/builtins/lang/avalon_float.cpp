/*
 *  This file is part of the Avalon programming language
 * 
 *  Copyright (c) 2018 Ntwali Bashige Toussafloat
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


#include <sstream>
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
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_float.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_float.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * float_pos
     * computes the unary positive of an floating point number
     */
    std::shared_ptr<expr> float_pos(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the floating point number __pos__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __pos__ function expects its argument to be an floating point number.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __pos__ function expects its argument to be an floating point number.");

        return arg;
    }

    /**
     * float_neg
     * computes the unary negative of an floating point number
     */
    std::shared_ptr<expr> float_neg(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the floating point number __neg__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __neg__ function expects its argument to be an floating point number.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __neg__ function expects its argument to be an floating point number.");

        // negate the value held inside the literal
        double arg_val = arg_lit -> get_float_value();
        double res_val = -arg_val;
        std::ostringstream sstr;
        sstr << res_val;
        std::string res_str = sstr.str();

        // create new literal with the new floating point number
        token lit_tok(FLOATING_POINT, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, FLOATING_POINT_EXPR, res_str);
        res_lit -> set_type_instance(float_instance);

        return res_lit;
    }

    /**
     * float_add
     * computes the addition of two floating point numbers
     */
    std::shared_ptr<expr> float_add(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the floating point number __add__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __add__ function expects its arguments to be floating point numbers.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __add__ function expects its arguments to be floating point numbers.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __add__ function expects its argument to be floating point numbers.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __add__ function expects its argument to be floating point numbers.");

        // add both arguments
        double arg_one_val = arg_one_lit -> get_float_value();
        double arg_two_val = arg_two_lit -> get_float_value();
        double res_val = arg_one_val + arg_two_val;
        std::ostringstream sstr;
        sstr << res_val;
        std::string res_str = sstr.str();

        // create new literal with the new floating point number
        token lit_tok(FLOATING_POINT, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, FLOATING_POINT_EXPR, res_str);
        res_lit -> set_type_instance(float_instance);

        return res_lit;
    }

    /**
     * float_sub
     * computes the substraction of two floating point numbers
     */
    std::shared_ptr<expr> float_sub(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the floating point number __sub__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __sub__ function expects its arguments to be floating point numbers.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __sub__ function expects its arguments to be floating point numbers.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __sub__ function expects its argument to be floating point numbers.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __sub__ function expects its argument to be floating point numbers.");

        // substract both arguments
        double arg_one_val = arg_one_lit -> get_float_value();
        double arg_two_val = arg_two_lit -> get_float_value();
        double res_val = arg_one_val - arg_two_val;
        std::ostringstream sstr;
        sstr << res_val;
        std::string res_str = sstr.str();

        // create new literal with the new floating point number
        token lit_tok(FLOATING_POINT, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, FLOATING_POINT_EXPR, res_str);
        res_lit -> set_type_instance(float_instance);

        return res_lit;
    }

    /**
     * float_mul
     * computes the multiplication of two floating point numbers
     */
    std::shared_ptr<expr> float_mul(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the floating point number __mul__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __mul__ function expects its arguments to be floating point numbers.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __mul__ function expects its arguments to be floating point numbers.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __mul__ function expects its argument to be floating point numbers.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __mul__ function expects its argument to be floating point numbers.");

        // multiply both arguments
        double arg_one_val = arg_one_lit -> get_float_value();
        double arg_two_val = arg_two_lit -> get_float_value();
        double res_val = arg_one_val * arg_two_val;
        std::ostringstream sstr;
        sstr << res_val;
        std::string res_str = sstr.str();

        // create new literal with the new floating point number
        token lit_tok(FLOATING_POINT, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, FLOATING_POINT_EXPR, res_str);
        res_lit -> set_type_instance(float_instance);

        return res_lit;
    }

    /**
     * float_div
     * computes the division of two floating point numbers
     */
    std::shared_ptr<expr> float_div(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the floating point number __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __div__ function expects its arguments to be floating point numbers.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number __div__ function expects its arguments to be floating point numbers.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __div__ function expects its argument to be floating point numbers.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number __div__ function expects its argument to be floating point numbers.");

        // divide both arguments
        double arg_one_val = arg_one_lit -> get_float_value();
        double arg_two_val = arg_two_lit -> get_float_value();
        double res_val = arg_one_val / arg_two_val;
        std::ostringstream sstr;
        sstr << res_val;
        std::string res_str = sstr.str();

        // create new literal with the new floating point number
        token lit_tok(FLOATING_POINT, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, FLOATING_POINT_EXPR, res_str);
        res_lit -> set_type_instance(float_instance);

        return res_lit;
    }

    /**
     * float_cast
     * returns an expression cast that represented the floating point number with a type specified by ret_instance
     */
    std::shared_ptr<expr> float_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        if(type_instance_strong_compare(ret_instance, string_instance)) {
            return float_string(arguments);
        }
        else {
            throw invalid_call("[compiler error] the floating point number __cast__ function cannot be cast to <" + mangle_type_instance(ret_instance) + ">.");
        }
    }

    /**
     * float_string
     * returns a floating point number representation of a string
     */
    std::shared_ptr<expr> float_string(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the floating point number <string> function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the floating point number <string> function expects its argument to be a floating point number.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, float_instance) == false)
            throw invalid_call("[compiler error] the floating point number <string> function expects its argument to be an floating point number.");

        // create new literal with the new floating point number
        double arg_val = arg_lit -> get_float_value();
        std::ostringstream sstr;
        sstr << arg_val;
        std::string float_str = sstr.str();
        if(float_str.find('.') == std::string::npos)
            float_str += ".0";
        float_str += "f";
        token lit_tok(FLOATING_POINT, float_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> float_lit = std::make_shared<literal_expression>(lit_tok, STRING_EXPR, float_str);
        float_lit -> set_type_instance(string_instance);

        return float_lit;
    }
}
