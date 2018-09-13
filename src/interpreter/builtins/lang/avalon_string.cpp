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
}
