/*
 *  This file is part of the Avalon programming language
 * 
 *  Copyright (c) 2018 Ntwali Bashige Toussadec
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

#include <boost/multiprecision/cpp_dec_float.hpp>

/* Lexer */
#include "lexer/token_type.hpp"
#include "lexer/token.hpp"

/* AST */
/* Declarations */
#include "representer/ast/decl/type.hpp"
/* Expressions */
#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_maybe.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_dec.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_dec.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * dec_pos
     * computes the unary positive of an decimal
     */
    std::shared_ptr<expr> dec_pos(std::vector<std::shared_ptr<expr> >& arguments) {
        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the decimal __pos__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __pos__ function expects its argument to be an decimal.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __pos__ function expects its argument to be an decimal.");

        return arg;
    }

    /**
     * dec_neg
     * computes the unary negative of an decimal
     */
    std::shared_ptr<expr> dec_neg(std::vector<std::shared_ptr<expr> >& arguments) {
        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the decimal __neg__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __neg__ function expects its argument to be an decimal.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __neg__ function expects its argument to be an decimal.");

        // negate the value held inside the literal
        boost::multiprecision::cpp_dec_float_100 arg_val = arg_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 res_val = -arg_val;
        std::string res_str = res_val.convert_to<std::string>();

        // create new literal with the new decimal
        token lit_tok(DECIMAL, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, DECIMAL_EXPR, res_str);
        res_lit -> set_type_instance(dec_instance);

        return res_lit;
    }

    /**
     * dec_add
     * computes the addition of two decimals
     */
    std::shared_ptr<expr> dec_add(std::vector<std::shared_ptr<expr> >& arguments) {
        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __add__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __add__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __add__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __add__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __add__ function expects its argument to be decimals.");

        // add both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 res_val = arg_one_val + arg_two_val;
        std::string res_str = res_val.convert_to<std::string>();

        // create new literal with the new decimal
        token lit_tok(DECIMAL, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, DECIMAL_EXPR, res_str);
        res_lit -> set_type_instance(dec_instance);

        return res_lit;
    }

    /**
     * dec_sub
     * computes the substraction of two decimals
     */
    std::shared_ptr<expr> dec_sub(std::vector<std::shared_ptr<expr> >& arguments) {
        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __sub__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __sub__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __sub__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __sub__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __sub__ function expects its argument to be decimals.");

        // substract both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 res_val = arg_one_val - arg_two_val;
        std::string res_str = res_val.convert_to<std::string>();

        // create new literal with the new decimal
        token lit_tok(DECIMAL, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, DECIMAL_EXPR, res_str);
        res_lit -> set_type_instance(dec_instance);

        return res_lit;
    }

    /**
     * dec_mul
     * computes the multiplication of two decimals
     */
    std::shared_ptr<expr> dec_mul(std::vector<std::shared_ptr<expr> >& arguments) {
        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __mul__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __mul__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __mul__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __mul__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __mul__ function expects its argument to be decimals.");

        // multiply both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 res_val = arg_one_val * arg_two_val;
        std::string res_str = res_val.convert_to<std::string>();

        // create new literal with the new decimal
        token lit_tok(DECIMAL, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, DECIMAL_EXPR, res_str);
        res_lit -> set_type_instance(dec_instance);

        return res_lit;
    }

    /**
     * dec_div
     * computes the division of two decimals
     */
    std::shared_ptr<expr> dec_div(std::vector<std::shared_ptr<expr> >& arguments) {
        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        type_instance maybe_instance = avl_maybe.get_type_instance(dec_instance);

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __div__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __div__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __div__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __div__ function expects its argument to be decimals.");

        // divide both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();

        if(arg_two_val == 0) {
            std::shared_ptr<call_expression> res_expr = std::make_shared<call_expression>(none_cons_tok);
            res_expr -> set_expression_type(DEFAULT_CONSTRUCTOR_EXPR);
            res_expr -> set_type_instance(maybe_instance);
            std::shared_ptr<expr> final_expr = res_expr;
            return final_expr;
        }
        else {
            boost::multiprecision::cpp_dec_float_100 res_val = arg_one_val / arg_two_val;
            std::string res_str = res_val.convert_to<std::string>();
            token lit_tok(DECIMAL, res_str, 0, 0, "__bil__");
            std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, DECIMAL_EXPR, res_str);
            res_lit -> set_type_instance(dec_instance);
            std::shared_ptr<expr> res_lit_expr = res_lit;

            std::shared_ptr<call_expression> res_expr = std::make_shared<call_expression>(just_cons_tok);
            res_expr -> add_argument(star_tok, res_lit_expr);
            res_expr -> set_expression_type(DEFAULT_CONSTRUCTOR_EXPR);
            res_expr -> set_type_instance(maybe_instance);
            std::shared_ptr<expr> final_expr = res_expr;
            return final_expr;
        }
    }

    /**
     * dec_cast
     * returns an expression cast that represented the decimal with a type specified by ret_instance
     */
    std::shared_ptr<expr> dec_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        if(type_instance_strong_compare(ret_instance, string_instance)) {
            return dec_string(arguments);
        }
        else {
            throw invalid_call("[compiler error] the decimal __cast__ function cannot be cast to <" + mangle_type_instance(ret_instance) + ">.");
        }
    }

    /**
     * dec_string
     * returns a decimal representation of a string
     */
    std::shared_ptr<expr> dec_string(std::vector<std::shared_ptr<expr> >& arguments) {
        // dec type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the decimal <string> function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal <string> function expects its argument to be a decimal.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal <string> function expects its argument to be an decimal.");

        // create new literal with the new decimal
        boost::multiprecision::cpp_dec_float_100 arg_val = arg_lit -> get_dec_value();
        std::string dec_str = arg_val.convert_to<std::string>();
        if(dec_str.find('.') == std::string::npos)
            dec_str += ".0";
        dec_str += "d";
        token lit_tok(STRING, dec_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> string_lit = std::make_shared<literal_expression>(lit_tok, STRING_EXPR, dec_str);
        string_lit -> set_type_instance(string_instance);
        return string_lit;
    }

    /**
     * dec_eq
     * returns a boolean indicating whether its arguments are equal
     */
    std::shared_ptr<expr> dec_eq(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        // compare both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        if(arg_one_val == arg_two_val)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * dec_ne
     * returns a boolean indicating whether its arguments are not equal
     */
    std::shared_ptr<expr> dec_ne(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        // compare both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        if(arg_one_val != arg_two_val)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * dec_gt
     * returns a boolean indicating whether the first argument is greater than the second
     */
    std::shared_ptr<expr> dec_gt(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        // compare both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        if(arg_one_val > arg_two_val)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * dec_ge
     * returns a boolean indicating whether the first argument is greater or equal to the second
     */
    std::shared_ptr<expr> dec_ge(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        // compare both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        if(arg_one_val >= arg_two_val)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * dec_lt
     * returns a boolean indicating whether the first argument is less than the second
     */
    std::shared_ptr<expr> dec_lt(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        // compare both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        if(arg_one_val < arg_two_val)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * dec_le
     * returns a boolean indicating whether the first argument is less or equal to the second
     */
    std::shared_ptr<expr> dec_le(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_dec avl_dec;
        type_instance dec_instance = avl_dec.get_type_instance();

        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the decimal __div__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its arguments to be decimals.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, dec_instance) == false)
            throw invalid_call("[compiler error] the decimal __eq__ function expects its argument to be decimals.");

        // compare both arguments
        boost::multiprecision::cpp_dec_float_100 arg_one_val = arg_one_lit -> get_dec_value();
        boost::multiprecision::cpp_dec_float_100 arg_two_val = arg_two_lit -> get_dec_value();
        if(arg_one_val <= arg_two_val)
            return true_final_expr;
        else
            return false_final_expr;
    }
}
