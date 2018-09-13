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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_BOOL_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_BOOL_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * bool_and
     * computes the boolean and of two boolean expressions
     */
    std::shared_ptr<expr> bool_and(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * bool_or
     * computes the boolean or of two boolean expressions
     */
    std::shared_ptr<expr> bool_or(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * bool_not
     * computes the boolean not of a single boolean expressions
     */
    std::shared_ptr<expr> bool_not(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * bool_cast
     * returns an expression cast that represented the boolean with a type specified by ret_instance
     */
    std::shared_ptr<expr> bool_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

    /**
     * bool_string
     * returns a boolean representation of a string
     */
    std::shared_ptr<expr> bool_string(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * bool_eq
     * returns a boolean indicating whether its arguments are equal
     */
    std::shared_ptr<expr> bool_eq(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * bool_ne
     * returns a boolean indicating whether its arguments are not equal
     */
    std::shared_ptr<expr> bool_ne(std::vector<std::shared_ptr<expr> >& arguments);
}

#endif
