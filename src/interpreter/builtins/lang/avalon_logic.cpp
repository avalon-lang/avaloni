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
#include "representer/builtins/lang/avalon_bit.hpp"
#include "representer/builtins/lang/avalon_bit2.hpp"
#include "representer/builtins/lang/avalon_bit4.hpp"
#include "representer/builtins/lang/avalon_bit8.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_bit.hpp"
#include "interpreter/builtins/lang/avalon_bit2.hpp"
#include "interpreter/builtins/lang/avalon_bit4.hpp"
#include "interpreter/builtins/lang/avalon_bit8.hpp"
#include "interpreter/builtins/lang/avalon_logic.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * avl_and
     * performs logical and
     */
    std::shared_ptr<expr> avl_and(std::vector<std::shared_ptr<expr> >& arguments) {
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

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __band__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_instance = arg_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_instance, bit_instance) && type_instance_strong_compare(arg_two_instance, bit_instance)) {
                return bit_and(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit2_instance) && type_instance_strong_compare(arg_two_instance, bit2_instance)) {
                return bit2_and(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit4_instance) && type_instance_strong_compare(arg_two_instance, bit4_instance)) {
                return bit4_and(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit8_instance) && type_instance_strong_compare(arg_two_instance, bit8_instance)) {
                return bit8_and(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __band__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __band__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_or
     * performs logical or
     */
    std::shared_ptr<expr> avl_or(std::vector<std::shared_ptr<expr> >& arguments) {
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

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __bor__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_instance = arg_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_instance, bit_instance) && type_instance_strong_compare(arg_two_instance, bit_instance)) {
                return bit_or(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit2_instance) && type_instance_strong_compare(arg_two_instance, bit2_instance)) {
                return bit2_or(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit4_instance) && type_instance_strong_compare(arg_two_instance, bit4_instance)) {
                return bit4_or(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit8_instance) && type_instance_strong_compare(arg_two_instance, bit8_instance)) {
                return bit8_or(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __bor__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __bor__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_xor
     * performs logical xor
     */
    std::shared_ptr<expr> avl_xor(std::vector<std::shared_ptr<expr> >& arguments) {
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

        // make sure we got only two arguments
        if(arguments.size() != 2)
            throw invalid_call("[compiler error] the builtin __xor__ function expects only two arguments.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg = arguments[0];
        std::shared_ptr<expr>& arg_two = arguments[1];
        if(arg -> is_literal_expression() && arg_two -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            std::shared_ptr<literal_expression> const & arg_two_lit = std::static_pointer_cast<literal_expression>(arg_two);

            // double check the type instance
            type_instance& arg_instance = arg_lit -> get_type_instance();
            type_instance& arg_two_instance = arg_two_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_instance, bit_instance) && type_instance_strong_compare(arg_two_instance, bit_instance)) {
                return bit_xor(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit2_instance) && type_instance_strong_compare(arg_two_instance, bit2_instance)) {
                return bit2_xor(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit4_instance) && type_instance_strong_compare(arg_two_instance, bit4_instance)) {
                return bit4_xor(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit8_instance) && type_instance_strong_compare(arg_two_instance, bit8_instance)) {
                return bit8_xor(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __xor__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __xor__ using arguments of unsupported type instances");
        }
    }

    /**
     * avl_not
     * performs logical not
     */
    std::shared_ptr<expr> avl_not(std::vector<std::shared_ptr<expr> >& arguments) {
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
            throw invalid_call("[compiler error] the builtin __bnot__ function expects only one argument.");

        // if we have identifier expressions (bool constructors)
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression()) {
            // get the literal expressions
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);

            // double check the type instance
            type_instance& arg_instance = arg_lit -> get_type_instance();
            if(type_instance_strong_compare(arg_instance, bit_instance)) {
                return bit_not(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit2_instance)) {
                return bit2_not(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit4_instance)) {
                return bit4_not(arguments);
            }
            else if(type_instance_strong_compare(arg_instance, bit8_instance)) {
                return bit8_not(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __bnot__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __bnot__ using arguments of unsupported type instances");
        }
    }
}
