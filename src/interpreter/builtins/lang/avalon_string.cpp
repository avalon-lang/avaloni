/*
 *  This file is part of the Avalon programming language
 * 
 *  Copyright (c) 2018 Ntwali Bashige Toussastring
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


#include <functional>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token_type.hpp"
#include "lexer/token.hpp"
#include "lexer/lexer.hpp"

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
#include "representer/builtins/lang/avalon_float.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_int.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_string.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * string_reverse
     * computes the reverse of a string
     */
    std::shared_ptr<expr> string_reverse(std::vector<std::shared_ptr<expr> >& arguments) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the string __reverse__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the string __reverse__ function expects its argument to be an string.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, string_instance) == false)
            throw invalid_call("[compiler error] the string __reverse__ function expects its argument to be an string.");

        // reverse the value held inside the literal
        std::string arg_val = arg_lit -> get_string_value();
        std::string res_str = arg_val;
        std::reverse(res_str.begin(), res_str.end());

        // create new literal with the new string
        token lit_tok(INTEGER, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, STRING_EXPR, res_str);
        res_lit -> set_type_instance(string_instance);

        return res_lit;
    }

    /**
     * string_concat
     * computes the concatenation of two strings
     */
    std::shared_ptr<expr> string_concat(std::vector<std::shared_ptr<expr> >& arguments) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the string __concat__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the string __concat__ function expects its arguments to be strings.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the string __concat__ function expects its arguments to be strings.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, string_instance) == false)
            throw invalid_call("[compiler error] the string __concat__ function expects its argument to be strings.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, string_instance) == false)
            throw invalid_call("[compiler error] the string __concat__ function expects its argument to be strings.");

        // add both arguments
        std::string arg_one_val = arg_one_lit -> get_string_value();
        std::string arg_two_val = arg_two_lit -> get_string_value();
        std::string res_str = arg_one_val + arg_two_val;

        // create new literal with the new string
        token lit_tok(STRING, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, STRING_EXPR, res_str);
        res_lit -> set_type_instance(string_instance);

        return res_lit;
    }

    /**
     * string_hash
     * computes the hash of a string
     */
    std::shared_ptr<expr> string_hash(std::vector<std::shared_ptr<expr> >& arguments) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the string __hash__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the string __hash__ function expects its argument to be an string.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, string_instance) == false)
            throw invalid_call("[compiler error] the string __hash__ function expects its argument to be an string.");

        // computes the hash
        std::string arg_val = arg_lit -> get_string_value();
        std::hash<std::string> hasher;
        std::size_t int_hash = hasher(arg_val);
        std::string string_hash = std::to_string(int_hash);

        // create new literal with the new string
        token lit_tok(INTEGER, string_hash, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> hash_lit = std::make_shared<literal_expression>(lit_tok, INTEGER_EXPR, string_hash);
        hash_lit -> set_type_instance(int_instance);

        return hash_lit;
    }

    /**
     * string_cast
     * returns an expression that represents the string cast to the given return type
     */
    std::shared_ptr<expr> string_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();
        
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();
        
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        type_instance maybe_bool_instance = avl_maybe.get_type_instance(bool_instance);
        type_instance maybe_int_instance = avl_maybe.get_type_instance(int_instance);
        type_instance maybe_float_instance = avl_maybe.get_type_instance(float_instance);

        if(type_instance_strong_compare(ret_instance, maybe_bool_instance)) {
            return string_bool(arguments);
        }
        else if(type_instance_strong_compare(ret_instance, maybe_int_instance)) {
            return string_int(arguments);
        }
        else if(type_instance_strong_compare(ret_instance, maybe_float_instance)) {
            return string_float(arguments);
        }
        else {
            throw invalid_call("[compiler error] the string __cast__ function cannot be cast to <" + mangle_type_instance(ret_instance) + ">.");
        }
    }

    /**
     * string_bool
     * returns a boolean representation of a string
     */
    std::shared_ptr<expr> string_bool(std::vector<std::shared_ptr<expr> >& arguments) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        type_instance maybe_bool_instance = avl_maybe.get_type_instance(bool_instance);

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // prepare None expression
        std::shared_ptr<identifier_expression> none_expr = std::make_shared<identifier_expression>(none_cons_tok);
        none_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        none_expr -> set_type_instance(maybe_bool_instance);

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the string <bool> function expects only one argument.");

        // make sure each argument is a literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the string <bool> function expects its argument to be a string.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, string_instance) == false)
            throw invalid_call("[compiler error] the string <bool> function expects its argument to be a string.");

        // retrieve the string held inside the literal
        std::string arg_val = arg_lit -> get_string_value();

        // lex the string and hope to find a boolean in it
        std::string source_path = arg_lit -> get_token() . get_source_path();
        error error_handler(source_path);
        lexer lxr(source_path, arg_val, error_handler);
        try {
            std::vector<std::shared_ptr<token> >& tokens = lxr.lex();
            // if we got more or less than three token, we don't have a boolean (first token will be the boolean, the second will be a new line and the last one an EOF)
            if(tokens.size() != 3) {
                std::shared_ptr<expr> final_expr = none_expr;
                return final_expr;
            }
            else {
                std::shared_ptr<token> tok = tokens[0];
                // if the token is not a identifier, we don't have a boolean
                if(tok -> get_type() != IDENTIFIER) {
                    std::shared_ptr<expr> final_expr = none_expr;
                    return final_expr;
                }
                // we got ourselves an identifier, we make sure that it is either <True> or <False>
                else {
                    if(* tok == true_cons_tok || * tok == false_cons_tok) {
                        std::shared_ptr<identifier_expression> bool_expr = std::make_shared<identifier_expression>(* tok);
                        bool_expr -> set_type_instance(bool_instance);
                        bool_expr -> set_expression_type(CONSTRUCTOR_EXPR);
                        std::shared_ptr<expr> res_bool_expr = bool_expr;

                        std::shared_ptr<call_expression> just_expr = std::make_shared<call_expression>(just_cons_tok);
                        just_expr -> add_argument(star_tok, res_bool_expr);
                        just_expr -> set_expression_type(DEFAULT_CONSTRUCTOR_EXPR);
                        just_expr -> set_type_instance(maybe_bool_instance);
                        std::shared_ptr<expr> final_expr = just_expr;
                        return final_expr;
                    }
                    else {
                        std::shared_ptr<expr> final_expr = none_expr;
                        return final_expr;
                    }
                }
            }
        } catch(lex_error err) {
            err.show();
            // any lexing error implies the return of None
            std::shared_ptr<expr> final_expr = none_expr;
            return final_expr;
        }
    }

    /**
     * string_int
     * returns an integer representation of a string
     */
    std::shared_ptr<expr> string_int(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        type_instance maybe_int_instance = avl_maybe.get_type_instance(int_instance);

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // prepare None expression
        std::shared_ptr<identifier_expression> none_expr = std::make_shared<identifier_expression>(none_cons_tok);
        none_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        none_expr -> set_type_instance(maybe_int_instance);

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the string <int> function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the string <int> function expects its argument to be a string.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, string_instance) == false)
            throw invalid_call("[compiler error] the string <int> function expects its argument to be a string.");

        // retrieve the string held inside the literal
        std::string arg_val = arg_lit -> get_string_value();

        // lex the string and hope to find an integer in it
        std::string source_path = arg_lit -> get_token() . get_source_path();
        error error_handler(source_path);
        lexer lxr(source_path, arg_val, error_handler);
        try {
            std::vector<std::shared_ptr<token> >& tokens = lxr.lex();
            // if we got more or less than three token, we don't have an integer (first token will be the integer, the second will be a new line and the last one an EOF)
            if(tokens.size() != 3) {
                std::shared_ptr<expr> final_expr = none_expr;
                return final_expr;
            }
            else {
                std::shared_ptr<token> tok = tokens[0];
                // if the token is not an integer literal, we don't have an integer
                if(tok -> get_type() != INTEGER) {
                    std::shared_ptr<expr> final_expr = none_expr;
                    return final_expr;
                }
                // we got ourselves an integer, we return it wrapped inside the Just constructor
                else {
                    std::shared_ptr<number> const & num = std::static_pointer_cast<number>(tok);
                    std::string value = num -> get_integral();
                    value.erase(std::remove(value.begin(), value.end(), '\''), value.end());
                    std::shared_ptr<literal_expression> int_expr = std::make_shared<literal_expression>(* tok, INTEGER_EXPR, value);
                    int_expr -> set_type_instance(int_instance);
                    std::shared_ptr<expr> res_int_expr = int_expr;

                    std::shared_ptr<call_expression> just_expr = std::make_shared<call_expression>(just_cons_tok);
                    just_expr -> add_argument(star_tok, res_int_expr);
                    just_expr -> set_expression_type(DEFAULT_CONSTRUCTOR_EXPR);
                    just_expr -> set_type_instance(maybe_int_instance);
                    std::shared_ptr<expr> final_expr = just_expr;
                    return final_expr;
                }
            }
        } catch(lex_error err) {
            err.show();
            // any lexing error implies the return of None
            std::shared_ptr<expr> final_expr = none_expr;
            return final_expr;
        }
    }

    /**
     * string_float
     * returns a floating point number representation of a string
     */
    std::shared_ptr<expr> string_float(std::vector<std::shared_ptr<expr> >& arguments) {
        // float type
        avalon_float avl_float;
        type_instance float_instance = avl_float.get_type_instance();

        // maybe type
        avalon_maybe avl_maybe;
        type_instance maybe_float_instance = avl_maybe.get_type_instance(float_instance);

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // prepare None expression
        std::shared_ptr<identifier_expression> none_expr = std::make_shared<identifier_expression>(none_cons_tok);
        none_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        none_expr -> set_type_instance(maybe_float_instance);

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the string <float> function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the string <float> function expects its argument to be a string.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, string_instance) == false)
            throw invalid_call("[compiler error] the string <float> function expects its argument to be a string.");

        // retrieve the string held inside the literal
        std::string arg_val = arg_lit -> get_string_value();

        // lex the string and hope to find an flaoating pointer number in it
        std::string source_path = arg_lit -> get_token() . get_source_path();
        error error_handler(source_path);
        lexer lxr(source_path, arg_val, error_handler);
        try {
            std::vector<std::shared_ptr<token> >& tokens = lxr.lex();
            // if we got more or less than three token, we don't have an flaoating pointer number (first token will be the flaoating pointer number, the second will be a new line and the last one an EOF)
            if(tokens.size() != 3) {
                std::shared_ptr<expr> final_expr = none_expr;
                return final_expr;
            }
            else {
                std::shared_ptr<token> tok = tokens[0];
                // if the token is not an flaoating pointer number literal, we don't have an flaoating pointer number
                if(tok -> get_type() != FLOATING_POINT) {
                    std::shared_ptr<expr> final_expr = none_expr;
                    return final_expr;
                }
                // we got ourselves an flaoating pointer number, we return it wrapped inside the Just constructor
                else {
                    std::shared_ptr<number> const & num = std::static_pointer_cast<number>(tok);
                    std::string integral = num -> get_integral();
                    integral.erase(std::remove(integral.begin(), integral.end(), '\''), integral.end());
                    std::string decimal = num -> get_decimal();
                    decimal.erase(std::remove(decimal.begin(), decimal.end(), '\''), decimal.end());
                    std::string value = integral + "." + decimal;
                    std::shared_ptr<literal_expression> float_expr = std::make_shared<literal_expression>(* tok, FLOATING_POINT_EXPR, value);
                    float_expr -> set_type_instance(float_instance);
                    std::shared_ptr<expr> res_float_expr = float_expr;

                    std::shared_ptr<call_expression> just_expr = std::make_shared<call_expression>(just_cons_tok);
                    just_expr -> add_argument(star_tok, res_float_expr);
                    just_expr -> set_expression_type(DEFAULT_CONSTRUCTOR_EXPR);
                    just_expr -> set_type_instance(maybe_float_instance);
                    std::shared_ptr<expr> final_expr = just_expr;
                    return final_expr;
                }
            }
        } catch(lex_error err) {
            err.show();
            // any lexing error implies the return of None
            std::shared_ptr<expr> final_expr = none_expr;
            return final_expr;
        }
    }
}
