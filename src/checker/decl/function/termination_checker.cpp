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

#include <utility>
#include <memory>
#include <vector>
#include <string>

/* AST */
#include "representer/ast/stmt/while_stmt.hpp"
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/statement.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/ast/stmt/if_stmt.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/decl/decl.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_void.hpp"

/* Checker */
#include "checker/decl/function/termination_checker.hpp"
#include "checker/exceptions/invalid_function.hpp"

/* Lexer */
#include "lexer/token.hpp"


namespace avalon {
    /**
     * the constructor expects nothing
     */
    termination_checker::termination_checker() {        
    }

    /**
     * check_termination
     * this function makes sure that all declarations in the function's body are reachable and that the function terminates
     */
    void termination_checker::check_termination(function& function_decl, std::shared_ptr<scope>& l_scope) {
        block_stmt& body = function_decl.get_body();
        bool terminates = analyze_block(body);
        type_instance ret_type_instance = function_decl.get_return_type_instance();

        // if the function's body doesn't terminate and it doesn't return void, we raise an error
        avalon_void avl_void;
        type_instance void_type_instance = avl_void.get_type_instance();
        if(terminates == false && type_instance_strong_compare(ret_type_instance, void_type_instance) == false)
            throw invalid_function(function_decl.get_token(), "This function's body is missing a <return> statement.");
    }

    /**
     * analyze_declaration
     * this function makes sure a declaration is reachable and checks if it terminates
     */
    void termination_checker::analyze_declaration(std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl) {
        if(current_decl -> is_variable()) {
            // reachability
            current_decl -> is_reachable(is_reachable(previous_decl));
        }
        else if(current_decl -> is_statement()) {
            analyze_statement(current_decl, previous_decl);
        }
        else {
            // the reachability and termination checker is not responsible for ensure only variable and statement declarations are found in functions
        }
    }

    /**
     * analyze_statement
     * this function makes sure a statement is reachable and checks if it terminates
     */
    void termination_checker::analyze_statement(std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl) {
        std::shared_ptr<statement> const & stmt_decl = std::static_pointer_cast<statement>(current_decl);
        std::shared_ptr<stmt>& l_stmt = stmt_decl -> get_statement();
                
        if(l_stmt -> is_while()) {
            analyze_while(l_stmt, current_decl, previous_decl);
        }
        else if(l_stmt -> is_if()) {
            analyze_if(l_stmt, current_decl, previous_decl);
        }
        // a break statement never terminates execution
        // but it is reachable only if the previous declaration is reachable
        else if(l_stmt -> is_break()) {
            // reachability
            current_decl -> is_reachable(is_reachable(previous_decl));

            // termination
            current_decl -> terminates(false);
            current_decl -> passes(false);
            
        }
        // a continue statement never terminates execution
        // but it is reachable only if the previous declaration is reachable
        else if(l_stmt -> is_continue()) {
            // reachability
            current_decl -> is_reachable(is_reachable(previous_decl));

            // termination
            current_decl -> terminates(false);
            current_decl -> passes(false);
        }
        // a pass statement never terminates execution
        // and it always reachable since it is the only statement that occur within a block
        else if(l_stmt -> is_pass()) {
            // reachability
            current_decl -> is_reachable(true);

            // termination
            current_decl -> terminates(false);
        }
        // a return statement terminates execution if it is reachable
        // but it is reachable only if the previous declaration is reachable
        else if(l_stmt -> is_return()) {
            // reachability
           current_decl -> is_reachable(is_reachable(previous_decl));

            // termination
            current_decl -> terminates(current_decl -> is_reachable());
            current_decl -> passes(false);
        }
        // an expression statement never terminates execution
        // but it is reachable only if the previous declaration doesn't terminate
        else if(l_stmt -> is_expression()) {
            // reachability
            current_decl -> is_reachable(is_reachable(previous_decl));

            // termination
            current_decl -> terminates(false);
        }
        else {
            throw std::runtime_error("[compiler error] unexpected statement type in checker.");
        }
    }

    /**
     * analyze_while
     * this function makes sure a while statement is reachable and checks if it terminates
     */
    void termination_checker::analyze_while(std::shared_ptr<stmt>& current_stmt, std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl) {
        std::shared_ptr<while_stmt> const & whl_stmt = std::static_pointer_cast<while_stmt>(current_stmt);

        // reachability
        current_decl -> is_reachable(is_reachable(previous_decl));

        // termination
        block_stmt& body = whl_stmt -> get_block();
        current_decl -> terminates(analyze_block(body));
    }

    /**
     * analyze_if
     * this function makes sure a if statement is reachable and checks if it terminates
     */
    void termination_checker::analyze_if(std::shared_ptr<stmt>& current_stmt, std::shared_ptr<decl>& current_decl, std::shared_ptr<decl>& previous_decl) {
        std::shared_ptr<if_stmt> f_stmt = std::static_pointer_cast<if_stmt>(current_stmt);

        // reachability
        current_decl -> is_reachable(is_reachable(previous_decl));

        // reachability
        std::vector<elif_branch>& elif_branches = f_stmt -> get_elifs();
        bool terminates = true;
        for(auto elf : elif_branches) {
            block_stmt& elif_body = elf.get_block();
            terminates = analyze_block(elif_body);
        }

        // we valiate the body of the else branch if any
        if(f_stmt -> has_else()) {
            else_branch els_branch = f_stmt -> get_else();
            block_stmt& else_body = els_branch.get_block();
            terminates = analyze_block(else_body);
        }

        current_decl -> terminates(terminates);
    }

    /**
     * analyze_block
     * this function makes sure a block statement is reachable and checks if it terminates
     */
    bool termination_checker::analyze_block(std::shared_ptr<stmt>& current_stmt) {
        std::shared_ptr<block_stmt> blc_stmt = std::static_pointer_cast<block_stmt>(current_stmt);
        return analyze_block(* blc_stmt);
    }

    bool termination_checker::analyze_block(block_stmt& blc_stmt) {
        bool terminates = false;
        std::shared_ptr<decl> previous_decl = nullptr;

        // termination
        std::vector<std::shared_ptr<decl> >& declarations = blc_stmt.get_declarations();
        for(auto it = declarations.begin(), end = declarations.end(); it != end; ++it) {
            if(it == declarations.begin())
                previous_decl = nullptr;
            else
                previous_decl = * (it - 1);

            analyze_declaration(* it, previous_decl);

            // a block terminates if any of its declarations terminates
            if((* it) -> terminates())
                terminates = true;
        }

        return terminates;
    }

    /**
     * is_reachable
     * given the previous declaration
     * this function returns a boolean indicating whether the current declaration will be reachable
     */
    bool termination_checker::is_reachable(std::shared_ptr<decl>& previous_decl) {
        // if there is no previous declaration,
        // then the current declaration is reachable
        if(previous_decl == nullptr)
            return true;
        else
            // if the previous declaration terminates the function,
            // then the current declaration is not reachable
            if(previous_decl -> terminates() == true)
                return false;
            else
                // if this previous declaration doesn't pass control to the current declaration,
                // then the current declaration is not rechable
                if(previous_decl -> passes() == false)
                    return false;
                // in other cases, the current declaration reachability depends on the previous declarations reachability
                else
                    return previous_decl -> is_reachable();
    }
}
