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

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/stmt/expression_stmt.hpp"
#include "representer/ast/stmt/continue_stmt.hpp"
#include "representer/ast/stmt/return_stmt.hpp"
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/stmt/while_stmt.hpp"
#include "representer/ast/stmt/break_stmt.hpp"
#include "representer/ast/stmt/pass_stmt.hpp"
#include "representer/ast/decl/statement.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/stmt/if_stmt.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/decl/decl.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_void.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"

/* Symbol table */
#include "representer/symtable/scope.hpp"

/* Checker */
#include "checker/decl/variable/variable_checker.hpp"
#include "checker/expr/expression_checker.hpp"
#include "checker/stmt/block_checker.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_already_declared.hpp"
#include "representer/exceptions/symbol_can_collide.hpp"
#include "checker/exceptions/invalid_expression.hpp"
#include "checker/exceptions/invalid_statement.hpp"
#include "checker/exceptions/invalid_variable.hpp"
#include "checker/exceptions/invalid_block.hpp"


namespace avalon {
    /**
     * the constructor expects nothing
     */
    block_checker::block_checker(type_instance& ret_instance) : m_ret_instance(ret_instance), m_decls_count(0), m_inside_loop(false) {
    }

    /**
     * check
     * checking a blocks statement entails the following:
     * - a block can only contain variable and statement declarations
     * - if a block contains a pass statement, it cannot contain any other type of statement or declaration
     * - if a break or continue statement is encountered, they must be within a loop
     * - all declarations within a block must be valid
     */
    void block_checker::check(block_stmt& l_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<std::shared_ptr<decl> >& block_decls = l_statement.get_declarations();
        m_decls_count = block_decls.size();

        // we iterate over all declarations, validating variables and statements and ignoring the rest (functions and types)
        for(auto& block_decl : block_decls) {
            // we check variable declarations
            if(block_decl -> is_variable()) {                
                check_variable(block_decl, l_scope, ns_name);
            }
            else if(block_decl -> is_statement()) {
                check_statement(block_decl, l_scope, ns_name);
            }
            
            else {
                throw invalid_block("Block statements must contain variable or statement declarations alone.");
            }
        }
    }

    /**
     * check_variable
     * given a variable declaration in the block, check if it is valid
     */
    void block_checker::check_variable(std::shared_ptr<decl>& declaration, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<variable> variable_decl = std::static_pointer_cast<variable>(declaration);
        variable_checker v_checker;

        // add the variable to the provided scope
        try {
            l_scope -> add_variable("*", variable_decl);
        } catch(symbol_already_declared err) {
            throw invalid_variable(variable_decl -> get_token(), err.what());
        } catch(symbol_can_collide err) {
            throw invalid_variable(variable_decl -> get_token(), err.what());
        }

        // check the variable
        try {
            v_checker.check(variable_decl, l_scope, ns_name);
        } catch(invalid_variable err) {
            throw err;
        }
    }

    /**
     * check_statement
     * given a statement declaration, check if it is valid
     */
    void block_checker::check_statement(std::shared_ptr<decl>& declaration, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<statement> const & stmt_decl = std::static_pointer_cast<statement>(declaration);
        std::shared_ptr<stmt>& l_stmt = stmt_decl -> get_statement();
        
        if(l_stmt -> is_while()) {
            check_while(l_stmt, l_scope, ns_name);
        }
        else if(l_stmt -> is_if()) {
            check_if(l_stmt, l_scope, ns_name);
        }
        else if(l_stmt -> is_break()) {
            check_break(l_stmt);
        }
        else if(l_stmt -> is_continue()) {
            check_continue(l_stmt);
        }
        else if(l_stmt -> is_pass()) {
            check_pass(l_stmt);
        }
        else if(l_stmt -> is_return()) {
            check_return(l_stmt, l_scope, ns_name);
        }
        else if(l_stmt -> is_expression()) {
            check_expression(l_stmt, l_scope, ns_name);
        }
        else {
            throw std::runtime_error("[compiler error] unexpected statement type in weak block checker.");
        }
    }

    /**
     * check_while
     * given a statement, check if it is a vali while statement
     */
    void block_checker::check_while(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        bool upper_loop_found = m_inside_loop;
        m_inside_loop = true;

        std::shared_ptr<while_stmt> const & wh_stmt = std::static_pointer_cast<while_stmt>(a_statement);
        std::shared_ptr<expr>& wh_condition = wh_stmt -> get_condition();
        block_stmt& wh_body = wh_stmt -> get_block();
        std::shared_ptr<scope>& wh_scope = wh_stmt -> get_scope();
        expression_checker expr_checker;
        avalon_bool avl_bool;
        type_instance& bool_instance = avl_bool.get_type_instance();

        // we check the condition and make sure it is of bool type
        try {
            type_instance cond_instance = expr_checker.check(wh_condition, wh_scope, ns_name);
            if(type_instance_strong_compare(cond_instance, bool_instance) == false) {
                throw invalid_statement(wh_stmt -> get_token(), "The condition for a while loop is of type instance <" + mangle_type_instance(cond_instance) + "> while the expected type instance is <bool>");
            }
        } catch(invalid_expression err) {
            throw err;
        }

        // we check the body of the while loop
        this -> check(wh_body, wh_scope, ns_name);

        // we only clear the inside loop flag if no other upper loop exists
        if(upper_loop_found == false)
            m_inside_loop = false;
    }

    /**
     * check_if
     * given a statement, check if it is a valid if statement
     */
    void block_checker::check_if(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<if_stmt> const & f_stmt = std::static_pointer_cast<if_stmt>(a_statement);
        std::shared_ptr<expr>& f_condition = f_stmt -> get_condition();
        block_stmt& f_body = f_stmt -> get_block();
        std::shared_ptr<scope>& f_scope = f_stmt -> get_scope();
        expression_checker expr_checker;
        avalon_bool avl_bool;
        type_instance& bool_instance = avl_bool.get_type_instance();

        // we check the condition and make sure it is of bool type
        try {
            type_instance cond_instance = expr_checker.check(f_condition, f_scope, ns_name);
            if(type_instance_strong_compare(cond_instance, bool_instance) == false) {
                throw invalid_statement(f_stmt -> get_token(), "The condition for an if conditional is of type instance <" + mangle_type_instance(cond_instance) + "> while the expected type instance is <bool>");
            }
        } catch(invalid_expression err) {
            throw err;
        }

        // we check the body of the if conditional
        this -> check(f_body, f_scope, ns_name);

        // handle elifs
        std::vector<elif_branch>& f_elifs = f_stmt -> get_elifs();
        for(auto& f_elif : f_elifs) {
            std::shared_ptr<expr>& elif_condition = f_elif.get_condition();
            block_stmt& elif_body = f_elif.get_block();
            std::shared_ptr<scope>& elif_scope = f_elif.get_scope();

            // we check the condition and make sure it is of bool type
            try {
                type_instance cond_instance = expr_checker.check(elif_condition, elif_scope, ns_name);
                if(type_instance_strong_compare(cond_instance, bool_instance) == false) {
                    throw invalid_statement(f_elif.get_token(), "The condition for an elif branch is of type instance <" + mangle_type_instance(cond_instance) + "> while the expected type instance is <bool>");
                }
            } catch(invalid_expression err) {
                throw err;
            }

            // we check the body of the elif branch
            this -> check(elif_body, elif_scope, ns_name);
        }

        // handle else
        if(f_stmt -> has_else()) {
            else_branch e_branch = f_stmt -> get_else();
            block_stmt& e_body = e_branch.get_block();
            std::shared_ptr<scope>& e_scope = e_branch.get_scope();
            this -> check(e_body, e_scope, ns_name);
        }
    }

    /**
     * check_break
     * check if a break statement occurs inside a loop
     */
    void block_checker::check_break(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<break_stmt> const & br_stmt = std::static_pointer_cast<break_stmt>(a_statement);
        if(m_inside_loop == false) {
            throw invalid_statement(br_stmt -> get_token(), "Unexpected break statement. A break statement can only occur within a loop.");
        }
    }

    /**
     * check_continue
     * check if a continue statement occurs inside a loop
     */
    void block_checker::check_continue(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<continue_stmt> const & cont_stmt = std::static_pointer_cast<continue_stmt>(a_statement);
        if(m_inside_loop == false) {
            throw invalid_statement(cont_stmt -> get_token(), "Unexpected continue statement. A continue statement can only occur within a loop.");
        }
    }

    /**
     * check_pass
     * check if a pass statement occurs inside a block
     */
    void block_checker::check_pass(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<pass_stmt> const & p_stmt = std::static_pointer_cast<pass_stmt>(a_statement);
        if(m_decls_count > 1)
            throw invalid_statement(p_stmt -> get_token(), "A pass statement cannot occur within a block with other statements.");
    }

    /**
     * check_return
     * given a statemnt, check if it is a valid return statement
     */
    void block_checker::check_return(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<return_stmt> const & ret_stmt = std::static_pointer_cast<return_stmt>(a_statement);
        expression_checker expr_checker;

        if(ret_stmt -> has_expression()) {
            std::shared_ptr<expr>& ret_expr = ret_stmt -> get_expression();
            try {
                type_instance ret_instance = expr_checker.check(ret_expr, l_scope, ns_name);
                if(type_instance_strong_compare(ret_instance, m_ret_instance) == false) {
                    throw invalid_statement(ret_stmt -> get_token(), "The returned expression is of type <" + mangle_type_instance(ret_instance) + "> while the expected return type instance is <" + mangle_type_instance(m_ret_instance) + ">.");
                }
            } catch(invalid_expression err) {
                throw err;
            }
        }
        else {
            avalon_void avl_void;
            type_instance& void_instance = avl_void.get_type_instance();
            if(type_instance_strong_compare(m_ret_instance, void_instance) == false) {
                throw invalid_statement(ret_stmt -> get_token(), "The return statement returns no expression while the expected return type instance is <" + mangle_type_instance(m_ret_instance) + ">.");
            }
        }
    }

    /**
     * check_expression
     * given a statement, check if it is a valid expression
     */
    void block_checker::check_expression(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expression_stmt> const & expr_stmt = std::static_pointer_cast<expression_stmt>(a_statement);
        expression_checker expr_checker;

        try {
            expr_checker.check(expr_stmt, l_scope, ns_name);
        } catch(invalid_expression err) {
            throw err;
        }
    }
}
