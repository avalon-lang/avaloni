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


#include <iostream>
#include <cstdio>
#include <memory>
#include <vector>
#include <string>

/* Lexer */
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
#include "interpreter/builtins/io/avalon_io.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * avl_print
     * prints a string to standard output and returns the number of characters written
     */
    std::shared_ptr<expr> avl_print(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the builtin __print__ function expects only one argument.");

        // make sure the argument is a string literal
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __print__ function expects its argument to be a string literal.");

        // get the string inside the literal
        std::shared_ptr<literal_expression> const & string_lit = std::static_pointer_cast<literal_expression>(arg);
        std::string string_val = string_lit -> get_string_value();

        // print the string
        int chars_buffered = printf("%s", string_val.c_str());

        // we initialize a literal expression that it to contain the number of characters written
        std::string ret_str = std::to_string(chars_buffered);
        token ret_tok(INTEGER, ret_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> ret_expr = std::make_shared<literal_expression>(ret_tok, INTEGER_EXPR, ret_str);
        ret_expr -> set_type_instance(int_instance);
        std::shared_ptr<expr> final_expr = ret_expr;

        return final_expr;
    }

    /**
     * avl_println
     * prints a string followed by a new line to standard output and returns the number of characters written
     */
    std::shared_ptr<expr> avl_println(std::vector<std::shared_ptr<expr> >& arguments) {
        // int type
        avalon_int avl_int;
        type_instance int_instance = avl_int.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the builtin __println__ function expects only one argument.");

        // make sure the argument is a string literal
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression() == false)
            throw invalid_call("[compiler error] the builtin __println__ function expects its argument to be a string literal.");

        // get the string inside the literal
        std::shared_ptr<literal_expression> const & string_lit = std::static_pointer_cast<literal_expression>(arg);
        std::string string_val = string_lit -> get_string_value();

        // print the string
        int chars_buffered = printf("%s\n", string_val.c_str());

        // we initialize a literal expression that it to contain the number of characters written
        std::string ret_str = std::to_string(chars_buffered);
        token ret_tok(INTEGER, ret_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> ret_expr = std::make_shared<literal_expression>(ret_tok, INTEGER_EXPR, ret_str);
        ret_expr -> set_type_instance(int_instance);
        std::shared_ptr<expr> final_expr = ret_expr;

        return final_expr;
    }

    /**
     * avl_readln
     * reads a string followed by a new line from the standard input
     */
    std::shared_ptr<expr> avl_readln(std::vector<std::shared_ptr<expr> >& arguments) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got no arguments
        if(arguments.size() != 0)
            throw invalid_call("[compiler error] the builtin __readln__ function doesn't except any argument.");

        // read the string and return it
        for(std::string input; std::getline(std::cin, input);) {
            token ret_tok(STRING, input, 0, 0, "__bil__");
            std::shared_ptr<literal_expression> ret_expr = std::make_shared<literal_expression>(ret_tok, STRING_EXPR, input);
            ret_expr -> set_type_instance(string_instance);
            std::shared_ptr<expr> final_expr = ret_expr;
            return final_expr;
        }

        return nullptr;
    }
}
