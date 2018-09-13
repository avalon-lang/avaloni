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

#ifndef AVALON_INTERPRETER_BUILTIN_FUNCTION_HPP_
#define AVALON_INTERPRETER_BUILTIN_FUNCTION_HPP_

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"

#include "interpreter/builtins/function_implementation.hpp"


namespace avalon {
    class avalon_function {
    public:
        /**
         * the constructor expects nothing
         */
        avalon_function(std::shared_ptr<function>& function_decl);

        /**
         * run
         * given the function arguments (and return type instance if available) find the appropritate implementation and run the function
         */
        std::shared_ptr<expr> run(std::vector<std::shared_ptr<expr> >& arguments);
        std::shared_ptr<expr> run(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

    private:
        /*
         * the function being currently run
         */
        std::shared_ptr<function>& m_function_decl;

        /*
         * a map between function names and their implementations
         */
        std::unordered_map<std::string, std::shared_ptr<function_implementation> > m_implementations;
    };
}

#endif
