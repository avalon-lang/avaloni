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

#ifndef AVALON_CHECKER_DECL_FUNCTION_HEADER_CHECKER_HPP_
#define AVALON_CHECKER_DECL_FUNCTION_HEADER_CHECKER_HPP_

#include <memory>
#include <string>

#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"


namespace avalon {
    class header_checker {
    public:
        /**
         * check_header
         * this function ensures that the function's header (parameters and return type) is of the correct form
         */
        static void check_header(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * prepare_header
         * this functions validates all type instances found in the function's header
         */
        static void prepare_header(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

    private:
        /**
         * we don't allow instatiation of this class
         */
        header_checker();
    };
}

#endif
