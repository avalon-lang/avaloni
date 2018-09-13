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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_FLOAT_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_FLOAT_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * float_pos
     * computes the unary positive of an floating point number
     */
    std::shared_ptr<expr> float_pos(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * float_neg
     * computes the unary negative of an floating point number
     */
    std::shared_ptr<expr> float_neg(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * float_add
     * computes the addition of two floating point numbers
     */
    std::shared_ptr<expr> float_add(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * float_sub
     * computes the substraction of two floating point numbers
     */
    std::shared_ptr<expr> float_sub(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * float_mul
     * computes the multiplication of two floating point numbers
     */
    std::shared_ptr<expr> float_mul(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * float_div
     * computes the division of two floating point numbers
     */
    std::shared_ptr<expr> float_div(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * float_cast
     * returns an expression cast that represented the floating point number with a type specified by ret_instance
     */
    std::shared_ptr<expr> float_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

    /**
     * float_string
     * returns a floating point number representation of a string
     */
    std::shared_ptr<expr> float_string(std::vector<std::shared_ptr<expr> >& arguments);
}

#endif
