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

#ifndef AVALON_CHECKER_DECL_FUNCTION_TERMINATION_CHECKER_HPP_
#define AVALON_CHECKER_DECL_FUNCTION_TERMINATION_CHECKER_HPP_

#include <utility>
#include <memory>
#include <vector>
#include <string>

#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/decl/decl.hpp"


namespace avalon {
    class termination_checker {
    public:
        /**
         * the constructor expects nothing
         */
        termination_checker();

        /**
         * check_termination
         * this function makes sure that all declarations in the function's body are reachable and that the function terminates
         */
        void check_termination(function& function_decl, std::shared_ptr<scope>& l_scope);

    private:
        /**
         * a vector of pairs containing the token to the unreachable declaration and a message to the user
         */
        std::vector<std::pair<token, std::string> > m_unreachables;

        /**
         * analyze_declaration
         * this function makes sure a declaration is reachable and checks if it terminates
         */
        void analyze_declaration(std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl);

        /**
         * analyze_statement
         * this function makes sure a statement declaration is reachable and checks if it terminates
         */
        void analyze_statement(std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl);

        /**
         * analyze_while
         * this function makes sure a while statement is reachable and checks if it terminates
         */
        void analyze_while(std::shared_ptr<stmt>& current_stmt, std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl);

        /**
         * analyze_if
         * this function makes sure a if statement is reachable and checks if it terminates
         */
        void analyze_if(std::shared_ptr<stmt>& current_stmt, std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl);

        /**
         * analyze_block
         * this function makes sure a block statement is reachable and checks if it terminates
         */
        bool analyze_block(std::shared_ptr<stmt>& current_stmt);
        bool analyze_block(block_stmt& blc_stmt);

        /**
         * is_reachable
         * given the previous declaration
         * this function returns a boolean indicating whether the current declaration will be reachable
         */
        bool is_reachable(std::shared_ptr<decl>& previous_decl);
    };
}

#endif
