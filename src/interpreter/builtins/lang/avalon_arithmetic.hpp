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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_ARITHMETIC_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_ARITHMETIC_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * avl_pos
     * computes the unary positive of an object
     */
    std::shared_ptr<expr> avl_pos(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_neg
     * computes the unary negative of an object
     */
    std::shared_ptr<expr> avl_neg(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_add
     * computes the addition of two objects
     */
    std::shared_ptr<expr> avl_add(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_sub
     * computes the substraction of two objects
     */
    std::shared_ptr<expr> avl_sub(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_mul
     * computes the multiplication of two objects
     */
    std::shared_ptr<expr> avl_mul(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_div
     * computes the division of two objects
     */
    std::shared_ptr<expr> avl_div(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_mod
     * computes the modulo of two objects
     */
    std::shared_ptr<expr> avl_mod(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_pow
     * computes the power of two objects
     */
    std::shared_ptr<expr> avl_pow(std::vector<std::shared_ptr<expr> >& arguments);
}

#endif
