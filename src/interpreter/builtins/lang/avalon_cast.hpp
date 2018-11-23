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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_CAST_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_CAST_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"

/* Quantum processor */
#include "interpreter/qprocessor.hpp"


namespace avalon {
    /**
     * avl_cast
     * implements the builtin cast function
     */
    std::shared_ptr<expr> avl_cast(std::shared_ptr<qprocessor>& qproc, std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

    /**
     * avl_bool_cast
     * implements cast to bool function
     */
    std::shared_ptr<expr> avl_bool_cast(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_int_cast
     * implements cast to int function
     */
    std::shared_ptr<expr> avl_int_cast(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_flaot_cast
     * implements cast to float function
     */
    std::shared_ptr<expr> avl_float_cast(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * avl_string_cast
     * implements cast to string function
     */
    std::shared_ptr<expr> avl_string_cast(std::vector<std::shared_ptr<expr> >& arguments);
}

#endif
