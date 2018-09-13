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
/* Expressions */
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtin programs */
#include "representer/builtins/lang/avalon_string.hpp"

/* Builtin functions */
#include "interpreter/builtins/lang/avalon_string.hpp"
#include "interpreter/builtins/lang/avalon_hash.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * avl_hash
     * implements the builtin hash function
     */
    std::shared_ptr<expr> avl_hash(std::vector<std::shared_ptr<expr> >& arguments) {
        // string type
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();

        // make sure we got only one argument
        if(arguments.size() != 1)
            throw invalid_call("[compiler error] the builtin __hash__ function expects only one argument.");

        // deal with literal expressions
        std::shared_ptr<expr>& arg = arguments[0];
        if(arg -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & arg_lit = std::static_pointer_cast<literal_expression>(arg);
            type_instance& arg_instance = arg_lit -> get_type_instance();

            if(type_instance_strong_compare(arg_instance, string_instance)) {
                return string_hash(arguments);
            }
            else {
                throw invalid_call("[compiler error] unexpected call to builtin function __hash__ using arguments of unsupported type instances");
            }
        }
        else {
            throw invalid_call("[compiler error] unexpected call to builtin function __hash__ using arguments of unsupported type instances");
        }
    }
}
