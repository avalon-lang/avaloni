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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_COMPARISON_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_COMPARISON_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * avl_eq
     * returns true if both its arguments are equal
     */
    std::shared_ptr<expr> avl_eq(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_ne
     * returns true if both its arguments are not equal
     */
    std::shared_ptr<expr> avl_ne(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_gt
     * returns true if the first argument is greater than the second one
     */
    std::shared_ptr<expr> avl_gt(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_ge
     * returns true if the first argument is greater or equal to the second one
     */
    std::shared_ptr<expr> avl_ge(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_lt
     * returns true if the first argument is less than the second one
     */
    std::shared_ptr<expr> avl_lt(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_le
     * returns true if the first argument is less or equal to the second one
     */
    std::shared_ptr<expr> avl_le(std::vector<std::shared_ptr<expr> >& arguments);
}

#endif
