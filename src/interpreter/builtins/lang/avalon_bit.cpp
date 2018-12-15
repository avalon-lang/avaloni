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
#include <bitset>

/* Lexer */
#include "lexer/token.hpp"

/* AST */
/* Declarations */
#include "representer/ast/decl/type.hpp"
/* Expressions */
#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_int.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_bit.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * bit_and
     * computes the bitwise and of two bit expressions
     */
    std::shared_ptr<expr> bit_and(std::vector<std::shared_ptr<expr> >& arguments) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the bitwise __band__ function expects only two arguments.");

        // make sure each argument is a literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_literal_expression() == false || arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bitwise __band__ function expects both its arguments to be bits.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __band__ function expects its argument to be bits.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __band__ function expects its argument to be bits.");

        // and both arguments
        std::bitset<1> arg_one_val = arg_one_lit -> get_bit_value();
        std::bitset<1> arg_two_val = arg_two_lit -> get_bit_value();
        std::bitset<1> res_val = arg_one_val & arg_two_val;
        std::string res_str = res_val.to_string();

        // create new literal with the new bitstring
        token lit_tok(BITS, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, BIT_EXPR, res_str);
        res_lit -> set_type_instance(bit_instance);

        return res_lit;
    }

    /**
     * bit_or
     * computes the bitwise or of two bit expressions
     */
    std::shared_ptr<expr> bit_or(std::vector<std::shared_ptr<expr> >& arguments) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the bitwise __bor__ function expects only two arguments.");

        // make sure each argument is a literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_literal_expression() == false || arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bitwise __bor__ function expects both its arguments to be bits.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __bor__ function expects its argument to be bits.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __bor__ function expects its argument to be bits.");

        // or both arguments
        std::bitset<1> arg_one_val = arg_one_lit -> get_bit_value();
        std::bitset<1> arg_two_val = arg_two_lit -> get_bit_value();
        std::bitset<1> res_val = arg_one_val | arg_two_val;
        std::string res_str = res_val.to_string();

        // create new literal with the new bitstring
        token lit_tok(BITS, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, BIT_EXPR, res_str);
        res_lit -> set_type_instance(bit_instance);

        return res_lit;
    }

    /**
     * bit_xor
     * computes the bitwise xor of two bit expressions
     */
    std::shared_ptr<expr> bit_xor(std::vector<std::shared_ptr<expr> >& arguments) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the bitwise __xor__ function expects only two arguments.");

        // make sure each argument is a literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_one -> is_literal_expression() == false || arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bitwise __xor__ function expects both its arguments to be bits.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __xor__ function expects its argument to be bits.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __xor__ function expects its argument to be bits.");

        // xor both arguments
        std::bitset<1> arg_one_val = arg_one_lit -> get_bit_value();
        std::bitset<1> arg_two_val = arg_two_lit -> get_bit_value();
        std::bitset<1> res_val = arg_one_val ^ arg_two_val;
        std::string res_str = res_val.to_string();

        // create new literal with the new bitstring
        token lit_tok(BITS, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, BIT_EXPR, res_str);
        res_lit -> set_type_instance(bit_instance);

        return res_lit;
    }

    /**
     * bit_not
     * computes the bitwise not of a single bit expressions
     */
    std::shared_ptr<expr> bit_not(std::vector<std::shared_ptr<expr> >& arguments) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the bitwise __bnot__ function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bitwise __bnot__ function expects its argument to be bits.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __bnot__ function expects its argument to be bits.");

        // negate the value held inside the literal
        std::bitset<1> arg_val = arg_lit -> get_bit_value();
        std::bitset<1> res_val = ~arg_val;
        std::string res_str = res_val.to_string();

        // create new literal with the new bitstring
        token lit_tok(BITS, res_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> res_lit = std::make_shared<literal_expression>(lit_tok, BIT_EXPR, res_str);
        res_lit -> set_type_instance(bit_instance);

        return res_lit;
    }

    /**
     * bit_cast
     * returns an expression cast that represented the bit with a type specified by ret_instance
     */
    std::shared_ptr<expr> bit_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        if(type_instance_strong_compare(ret_instance, string_instance)) {
            return bit_string(arguments);
        }
        else {
            throw invalid_call("[compiler error] the bit __cast__ function cannot cast to <" + mangle_type_instance(ret_instance) + ">.");
        }
    }

    /**
     * bit_string
     * returns a bit representation of a string
     */
    std::shared_ptr<expr> bit_string(std::vector<std::shared_ptr<expr> >& arguments) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the bit <string> function expects only one argument.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bit <string> function expects its argument to bebits.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

        // double check the type instance
        type_instance& arg_instance = arg_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bit <string> function expects its argument to be bits.");

        // create new literal with the new integer
        std::bitset<1> arg_val = arg_lit -> get_bit_value();
        std::string bit_str = "0b" + arg_val.to_string();
        token lit_tok(STRING, bit_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> string_lit = std::make_shared<literal_expression>(lit_tok, STRING_EXPR, bit_str);
        string_lit -> set_type_instance(string_instance);
        return string_lit;
    }

    /**
     * bit_eq
     * returns a bit indicating whether its arguments are equal
     */
    std::shared_ptr<expr> bit_eq(std::vector<std::shared_ptr<expr> >& arguments) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

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
            throw invalid_call("[compiler error] the bitwise __eq__ function expects only two arguments.");

        // make sure each argument is an literal expression
        std::shared_ptr<expr>& arg_one = arguments[0];        
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bitwise __eq__ function expects its arguments to be bits.");

        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bitwise __eq__ function expects its arguments to be bits.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instance
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __eq__ function expects its argument to be bits.");

        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bitwise __eq__ function expects its argument to be bits.");

        // compare both arguments
        std::bitset<1> arg_one_val = arg_one_lit -> get_bit_value();
        std::bitset<1> arg_two_val = arg_two_lit -> get_bit_value();
        if(arg_one_val == arg_two_val)
            return true_final_expr;
        else
            return false_final_expr;
    }

    /**
     * bit_refitem
     * returns the effective index where to find the requested bit
     */
    std::shared_ptr<expr> bit_refitem(std::vector<std::shared_ptr<expr> >& arguments) {
        // bit type
        avalon_bit avl_bit;
        type_instance bit_instance = avl_bit.get_type_instance();

        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // make sure we got exactly two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the bit __refitem__ function expects exactly two arguments.");

        // make sure each the first argument in a bit literal
        std::shared_ptr<expr>& arg_one = arguments[0];
        if(arg_one -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bit __refitem__ function expects its first argument to be the referenced bit.");

        // make sure each the first argument in a integer literal
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg_two -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the bit __refitem__ function expects its second argument to be an integer index.");

        // get the literal expressions
        std::shared_ptr<literal_expression> const & arg_one_lit = std::static_pointer_cast<literal_expression>(arg_one);
        std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

        // double check the type instances
        type_instance& arg_one_instance = arg_one_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_one_instance, bit_instance) == false)
            throw invalid_call("[compiler error] the bit __refitem__ function expects its first argument to be an bit.");

        // make sure the second argument is an integer
        type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
        if(type_instance_strong_compare(arg_two_instance, int_instance) == false)
            throw invalid_call("[compiler error] the bit __refitem__ function expects its second argument to be an integer.");

        // get the index
        long long int user_index = arg_two_lit -> get_int_value();
        if(user_index < 0)
            throw invalid_call("[compiler error] the bit __refitem__ function expects the index to be a positive integer.");

        std::size_t index = (std::size_t) user_index;

        // if the index is not 0, then the checker did not do its job properly
        if(index != 0)
            throw invalid_call("[compiler error] trying to access a bit using index different from 0.");

        // we return the reference holding the integer index
        token gen_tok(MUL, "*", 0, 0, "__dummy__");
        std::shared_ptr<reference_expression> ref_expr = std::make_shared<reference_expression>(gen_tok, nullptr);
        ref_expr -> set_index(index);
        std::shared_ptr<expr> final_ref_expr = ref_expr;
        return final_ref_expr;
    }
}
