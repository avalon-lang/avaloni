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

#ifndef AVALON_CHECKER_DECL_FUNCTION_FUNCTION_CHECKER_HPP_
#define AVALON_CHECKER_DECL_FUNCTION_FUNCTION_CHECKER_HPP_

#include <utility>
#include <memory>
#include <string>

#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/type.hpp"
#include "lexer/token.hpp"


namespace avalon {
    /**
     * find_function
     * given a function name, parameters' type instances and possibly a return type, get the function that matches all
     */
    std::shared_ptr<function> find_function(const std::string& name, std::vector<type_instance>& param_instances, type_instance& ret_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& standins);
    std::shared_ptr<function> find_function(const std::string& name, std::vector<type_instance>& param_instances, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& standins);


    class function_checker {
    public:
        /**
         * a default constructor expects nothing
         */
        function_checker();

        /**
         * check
         * this function makes sure:
         * - all parameters' types and return type are valid
         * - the body of the function is valid
         * - all declarations that make up a function are reachable (conservatively)
         * - all branches do terminate normally (conservatively)
         */
        void check(function& function_decl, const std::string& ns_name);

        /**
         * check_header
         * this function ensures that the function's header (parameters and return type) is of the correct form
         */
        void check_header(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * check_body
         * this function ensures that the body of the function is valid:
         * - makes sure that declarations that make up the function's body are either variable declarations or statement declarations
         * - make sure that all return statements return objects of the correct type
         * - make sure that all declarations that make up the function body are valid
         */
        void check_body(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * check_termination
         * this function makes sure that all declarations in the function's body are reachable and that the function terminates
         */
        void check_termination(function& function_decl, std::shared_ptr<scope>& l_scope);
    };
}

#endif
