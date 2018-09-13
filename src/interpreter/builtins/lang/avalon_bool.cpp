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
#include "representer/builtins/lang/avalon_bool.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_bool.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * bool_and
     * computes the boolean and of two boolean expressions
     */
    std::shared_ptr<expr> bool_and(std::vector<std::shared_ptr<expr> >& arguments) {
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
            throw invalid_call("[compiler error] the boolean __and__ function expects only two arguments.");

        // make sure each argument is an identifier expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_identifier_expression() == false || arg_two -> is_identifier_expression() == false)
            throw invalid_call("[compiler error] the boolean __and__ function expects both its arguments to be booleans.");

        // get the identifier expressions
        std::shared_ptr<identifier_expression> const & arg_one_id = std::static_pointer_cast<identifier_expression>(arg_one);
        std::shared_ptr<identifier_expression> const & arg_two_id = std::static_pointer_cast<identifier_expression>(arg_two);

        // compute the values
        if(arg_one_id -> get_token() == true_cons_tok && arg_two_id -> get_token() == true_cons_tok)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * bool_or
     * computes the boolean or of two boolean expressions
     */
    std::shared_ptr<expr> bool_or(std::vector<std::shared_ptr<expr> >& arguments) {
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
            throw invalid_call("[compiler error] the boolean __or__ function expects only two arguments.");

        // make sure each argument is an identifier expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_identifier_expression() == false || arg_two -> is_identifier_expression() == false)
            throw invalid_call("[compiler error] the boolean __or__ function expects both its arguments to be booleans.");

        // get the identifier expressions
        std::shared_ptr<identifier_expression> const & arg_one_id = std::static_pointer_cast<identifier_expression>(arg_one);
        std::shared_ptr<identifier_expression> const & arg_two_id = std::static_pointer_cast<identifier_expression>(arg_two);

        // compute the values
        if(arg_one_id -> get_token() == false_cons_tok && arg_two_id -> get_token() == false_cons_tok)
            return false_final_expr;
        else
            return true_final_expr;
    }

    /**
     * bool_not
     * computes the boolean not of a single boolean expressions
     */
    std::shared_ptr<expr> bool_not(std::vector<std::shared_ptr<expr> >& arguments) {
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

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the boolean __not__ function expects only one argument.");

        // make sure each argument is an identifier expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_identifier_expression() == false)
            throw invalid_call("[compiler error] the boolean __not__ function expects its argument to be a boolean.");

        // get the identifier expressions
        std::shared_ptr<identifier_expression> const & arg_id = std::static_pointer_cast<identifier_expression>(arg);

        // compute the value
        if(arg_id -> get_token() == true_cons_tok)
            return false_final_expr;
        else
            return true_final_expr;
    }

    /**
     * bool_cast
     * returns an expression cast that represented the boolean with a type specified by ret_instance
     */
    std::shared_ptr<expr> bool_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        if(type_instance_strong_compare(ret_instance, string_instance)) {
            return bool_string(arguments);
        }
        else {
            throw invalid_call("[compiler error] the boolean __cast__ function cannot be cast to <" + mangle_type_instance(ret_instance) + ">.");
        }
    }

    /**
     * bool_string
     * returns a boolean representation of a string
     */
    std::shared_ptr<expr> bool_string(std::vector<std::shared_ptr<expr> >& arguments) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // string true expression
        std::shared_ptr<literal_expression> true_expr = std::make_shared<literal_expression>(true_cons_tok, STRING_EXPR, "True");
        true_expr -> set_type_instance(string_instance);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // string false expression
        std::shared_ptr<literal_expression> false_expr = std::make_shared<literal_expression>(false_cons_tok, STRING_EXPR, "False");
        false_expr -> set_type_instance(string_instance);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the boolean <string> function expects only one argument.");

        // make sure each argument is an identifier expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_identifier_expression() == false)
            throw invalid_call("[compiler error] the boolean <string> function expects its argument to be a boolean.");

        // get the identifier expressions
        std::shared_ptr<identifier_expression> const & arg_id = std::static_pointer_cast<identifier_expression>(arg);

        // get the string representation
        if(arg_id -> get_token() == true_cons_tok)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * bool_eq
     * returns a boolean indicating whether its arguments are equal
     */
    std::shared_ptr<expr> bool_eq(std::vector<std::shared_ptr<expr> >& arguments) {
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
            throw invalid_call("[compiler error] the boolean __eq__ function expects only two arguments.");

        // make sure each argument is an identifier expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_identifier_expression() == false || arg_two -> is_identifier_expression() == false)
            throw invalid_call("[compiler error] the boolean __eq__ function expects both its arguments to be booleans.");

        // get the identifier expressions
        std::shared_ptr<identifier_expression> const & arg_one_id = std::static_pointer_cast<identifier_expression>(arg_one);
        std::shared_ptr<identifier_expression> const & arg_two_id = std::static_pointer_cast<identifier_expression>(arg_two);

        // compute the values
        if(
           (arg_one_id -> get_token() == true_cons_tok && arg_two_id -> get_token() == true_cons_tok) ||
           (arg_one_id -> get_token() == false_cons_tok && arg_two_id -> get_token() == false_cons_tok)
        )
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * bool_ne
     * returns a boolean indicating whether its arguments are not equal
     */
    std::shared_ptr<expr> bool_ne(std::vector<std::shared_ptr<expr> >& arguments) {
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
            throw invalid_call("[compiler error] the boolean __eq__ function expects only two arguments.");

        // make sure each argument is an identifier expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_identifier_expression() == false || arg_two -> is_identifier_expression() == false)
            throw invalid_call("[compiler error] the boolean __eq__ function expects both its arguments to be booleans.");

        // get the identifier expressions
        std::shared_ptr<identifier_expression> const & arg_one_id = std::static_pointer_cast<identifier_expression>(arg_one);
        std::shared_ptr<identifier_expression> const & arg_two_id = std::static_pointer_cast<identifier_expression>(arg_two);

        // compute the values
        if(
           (arg_one_id -> get_token() == true_cons_tok && arg_two_id -> get_token() == false_cons_tok) ||
           (arg_one_id -> get_token() == false_cons_tok && arg_two_id -> get_token() == true_cons_tok)
        )
            return true_final_expr;
        else
            return false_final_expr;
    }
}
