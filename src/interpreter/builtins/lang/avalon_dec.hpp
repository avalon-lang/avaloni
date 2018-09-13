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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_DEC_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_DEC_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * dec_pos
     * computes the unary positive of an decimal
     */
    std::shared_ptr<expr> dec_pos(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_neg
     * computes the unary negative of an decimal
     */
    std::shared_ptr<expr> dec_neg(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_add
     * computes the addition of two decimals
     */
    std::shared_ptr<expr> dec_add(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_sub
     * computes the substraction of two decimals
     */
    std::shared_ptr<expr> dec_sub(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_mul
     * computes the multiplication of two decimals
     */
    std::shared_ptr<expr> dec_mul(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_div
     * computes the division of two decimals
     */
    std::shared_ptr<expr> dec_div(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_cast
     * returns an expression cast that represented the decimal with a type specified by ret_instance
     */
    std::shared_ptr<expr> dec_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

    /**
     * dec_string
     * returns a decimal representation of a string
     */
    std::shared_ptr<expr> dec_string(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_eq
     * returns a boolean indicating whether its arguments are equal
     */
    std::shared_ptr<expr> dec_eq(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_ne
     * returns a boolean indicating whether its arguments are not equal
     */
    std::shared_ptr<expr> dec_ne(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_gt
     * returns a boolean indicating whether the first argument is greater than the second
     */
    std::shared_ptr<expr> dec_gt(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_ge
     * returns a boolean indicating whether the first argument is greater or equal to the second
     */
    std::shared_ptr<expr> dec_ge(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_lt
     * returns a boolean indicating whether the first argument is less than the second
     */
    std::shared_ptr<expr> dec_lt(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * dec_le
     * returns a boolean indicating whether the first argument is less or equal to the second
     */
    std::shared_ptr<expr> dec_le(std::vector<std::shared_ptr<expr> >& arguments);
}

#endif
