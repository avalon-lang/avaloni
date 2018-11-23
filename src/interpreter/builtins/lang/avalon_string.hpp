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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_STRING_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_STRING_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * string_reverse
     * computes the reverse of a string
     */
    std::shared_ptr<expr> string_reverse(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * string_concat
     * computes the concatenation of two strings
     */
    std::shared_ptr<expr> string_concat(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * string_hash
     * computes the hash of a string
     */
    std::shared_ptr<expr> string_hash(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * string_cast
     * returns an expression that represents the string cast to the given return type
     */
    std::shared_ptr<expr> string_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

    /**
     * string_bool
     * returns a boolean representation of a string
     */
    std::shared_ptr<expr> string_bool(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * string_int
     * returns an integer representation of a string
     */
    std::shared_ptr<expr> string_int(std::vector<std::shared_ptr<expr> >& arguments);
}

#endif
