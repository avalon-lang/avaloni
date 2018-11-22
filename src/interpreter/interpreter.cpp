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

#include <stdexcept>
#include <memory>
#include <vector>
#include <string>
#include <tuple>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* AST */
/* Declarations */
#include "representer/ast/decl/statement.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/import.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/program.hpp"
/* Statements */
#include "representer/ast/stmt/expression_stmt.hpp"
#include "representer/ast/stmt/continue_stmt.hpp"
#include "representer/ast/stmt/return_stmt.hpp"
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/stmt/while_stmt.hpp"
#include "representer/ast/stmt/break_stmt.hpp"
#include "representer/ast/stmt/pass_stmt.hpp"
#include "representer/ast/stmt/if_stmt.hpp"
/* Expressions */
#include "representer/ast/expr/dereference_expression.hpp"
#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/assignment_expression.hpp"
#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/grouped_expression.hpp"
#include "representer/ast/expr/binary_expression.hpp"
#include "representer/ast/expr/unary_expression.hpp"
#include "representer/ast/expr/match_expression.hpp"
#include "representer/ast/expr/cast_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/tuple_expression.hpp"
#include "representer/ast/expr/list_expression.hpp"
#include "representer/ast/expr/map_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_maybe.hpp"
#include "representer/builtins/lang/avalon_list.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_void.hpp"

/* Symtable */
#include "representer/symtable/gtable.hpp"
#include "representer/symtable/scope.hpp"

/* Interpreter */
#include "interpreter/interpreter.hpp"

/* Quantum processor */
#include "interpreter/qprocessor.hpp"

/* Builtin functions implementation */
#include "interpreter/builtins/avalon_function.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_already_declared.hpp"
#include "representer/exceptions/symbol_can_collide.hpp"
#include "representer/exceptions/symbol_not_found.hpp"


namespace avalon {
/**
 * the contructor expects:
 * - the error handler to be used to show errors
 * - the token where the error occured
 * - the message to show the user
 */
interpret_error::interpret_error(error& error_handler, token tok, const std::string& message) : std::runtime_error(message), m_error_handler(error_handler), m_tok(tok) {
}

    /**
     * what
     * get the error message
     */
    const char * interpret_error::what() const noexcept {
        return std::runtime_error::what();
    }

    /**
     * show
     * calls the error handler error reporting function to display the error
     */
    void interpret_error::show() {
        m_error_handler.log(m_tok . get_source_path(), m_tok . get_line(), m_tok . get_column(), what());
    }

/**
 * the constructor expects:
 * - the global symbol table with programs to interpret
 * - the error handler to use in order to display errors
 */
interpreter::interpreter(gtable& gtab, error& error_handler) : m_error_handler(error_handler), m_gtable(gtab), m_qproc(nullptr), m_continue_loop(false), m_break_loop(false) {
    m_qproc = std::make_shared<qprocessor>(m_error_handler);
}

    /**
     * interpret
     * the interpreter entry point
     */
    void interpreter::interpret(std::vector<std::string>& arguments) {
        if(m_gtable.has_main_fqn() == false)
            throw interpretation_error(star_tok, "The main program could not be found.");

        // import all of the main program dependencies
        import_declarations();

        // get the main program
        program& main_prog = m_gtable.get_main_program();

        // interpret the main program
        interpret_program(main_prog, arguments);
    }

    /**
     * interpret_program
     * given the program that contains the main function and interpret the main function
     */
    void interpreter::interpret_program(program& main_prog, std::vector<std::string>& arguments) {
        // string type instance
        avalon_string avl_string;
        type_instance string_instance = avl_string.get_type_instance();
        // list type instance
        avalon_list avl_list(string_instance);
        type_instance list_instance = avl_list.get_type_instance();

        // prepare the arguments in a form usable to us
        std::shared_ptr<list_expression> args_list = std::make_shared<list_expression>(star_tok);        
        args_list -> set_type_instance(list_instance);
        for(auto& argument : arguments) {
            std::shared_ptr<literal_expression> arg = std::make_shared<literal_expression>(star_tok, STRING_EXPR, argument);
            arg -> set_type_instance(string_instance);
            std::shared_ptr<expr> element = arg;
            args_list -> add_element(element);
        }
        std::shared_ptr<expr> args = args_list;
        std::vector<std::shared_ptr<expr> > argv {args};

        // find the main function
        std::shared_ptr<scope>& l_scope = main_prog.get_scope();
        try {
            std::shared_ptr<function>& main_fun = l_scope -> get_function("*", "__main__", 1);
            interpret_function(main_fun, argv);
        } catch(std::runtime_error err) {
            throw interpretation_error(star_tok, err.what());
        }
    }

    /**
     * interpret_function
     * given a function declaration and a list of arguments, evaluate said function given said arguments
     * returns an expressions that is the result of the function evaluation; a null pointer if the function return type is void
     */
    std::shared_ptr<expr> interpreter::interpret_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments) {        
        if(function_decl -> is_builtin()) {
            return interpret_builtin_function(function_decl, arguments);
        }
        else {
            return interpret_user_function(function_decl, arguments);
        }
    }

    std::shared_ptr<expr> interpreter::interpret_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        if(function_decl -> is_builtin()) {
            return interpret_builtin_function(function_decl, arguments, ret_instance);
        }
        else {
            // generally the mangled name of a function is enough to find it and pass it arguments
            // but the situation if different for builtin functions because we can't use their mangled names to find their implementations
            // so whenever we know that we are passing a function with a return type instance, we first make sure it is not a user defined function
            throw interpretation_error(function_decl -> get_token(), "[compiler error] an expression passed a return type instance for a user defined function.");
        }
    }

    /**
     * interpret_builtin_function
     * evaluates a builtin function by calling its local evaluate method passing it the given arguments
     */
    std::shared_ptr<expr> interpreter::interpret_builtin_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments) {
        avalon_function avl_function(function_decl, m_qproc);
        return avl_function.run(arguments);
    }

    std::shared_ptr<expr> interpreter::interpret_builtin_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        avalon_function avl_function(function_decl, m_qproc);
        return avl_function.run(arguments, ret_instance);
    }

    /**
     * interpret_user_function
     * evaluates a user function by using this interpreter
     */
    std::shared_ptr<expr> interpreter::interpret_user_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments) {
        // first, we assign the function parameters values passed as arguments
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = function_decl -> get_params();
        auto param_it = params.begin(), param_end = params.end();
        auto arg_it = arguments.begin(), arg_end = arguments.end();
        for(; param_it != param_end && arg_it != arg_end; ++param_it, ++arg_it)
            param_it -> second -> set_value(* arg_it);

        // then we work on the function body
        block_stmt& body = function_decl -> get_body();
        std::shared_ptr<scope>& l_scope = function_decl -> get_scope();
        const std::string& ns_name = function_decl -> get_namespace();
        std::pair<bool, std::shared_ptr<expr> > res = interpret_block(body, l_scope, ns_name);
        return res.second;
    }

    /**
     * interpret_block
     * given a block statement, evaluate the declarations it contains
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_block(block_stmt& blc, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::vector<std::shared_ptr<decl> >& declarations = blc.get_declarations();

        for(auto& declaration : declarations) {
            if(declaration -> is_variable()) {
                interpret_variable(declaration, l_scope, ns_name);
            }
            else if(declaration -> is_statement()) {
                std::pair<bool, std::shared_ptr<expr> > res = interpret_statement(declaration, l_scope, ns_name);

                // handle return statement
                if(res.first == true)
                    return res;

                // handle continue statement
                if(m_continue_loop) {
                    m_continue_loop = false;
                    return std::make_pair(false, nullptr);
                }

                // handle break statement
                if(m_break_loop)
                    return std::make_pair(false, nullptr);
            }
            else {
                throw interpretation_error(declaration -> decl_token(), "[compiler error] unexpected declaration in function body.");
            }
        }

        return std::make_pair(false, nullptr);
    }

    /**
     * interpret_variable
     * given a variable declaration, interpret the initializer expression and assign it the resulting value
     */
    void interpreter::interpret_variable(std::shared_ptr<decl>& declaration, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<variable> const & variable_decl = std::static_pointer_cast<variable>(declaration);
        std::shared_ptr<expr>& current_value = variable_decl -> get_value();
        std::shared_ptr<expr> new_value = interpret_expression(current_value, l_scope, ns_name);
        variable_decl -> set_value(new_value);

        // if we are dealing with a quantum variable initialization, we need to let the quantum processor know about the new initialization
        type_instance var_instance = variable_decl -> get_type_instance();
        if(var_instance.get_type() -> is_quantum()) {
            std::shared_ptr<literal_expression> const & lit_expr = std::static_pointer_cast<literal_expression>(new_value);
            std::pair<std::size_t, std::size_t> indices = m_qproc -> add_ket(lit_expr, lit_expr -> get_qubit_value(), lit_expr -> get_length());
            lit_expr -> set_start_index(indices.first);
            lit_expr -> set_end_index(indices.second);
        }
    }

    /**
     * interpret_statement
     * given a statement declaration, interpret it depending on its type
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_statement(std::shared_ptr<decl>& declaration, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<statement> const & statement_decl = std::static_pointer_cast<statement>(declaration);
        std::shared_ptr<stmt>& a_statement = statement_decl -> get_statement();

        if(a_statement -> is_while()) {
            return interpret_while(a_statement, l_scope, ns_name);
        }
        else if(a_statement -> is_if()) {
            return interpret_if(a_statement, l_scope, ns_name);
        }
        else if(a_statement -> is_break()) {
            return interpret_break(a_statement, l_scope, ns_name);
        }
        else if(a_statement -> is_continue()) {
            return interpret_continue(a_statement, l_scope, ns_name);
        }
        else if(a_statement -> is_pass()) {
            return interpret_pass(a_statement, l_scope, ns_name);
        }
        else if(a_statement -> is_return()) {
            return interpret_return(a_statement, l_scope, ns_name);
        }
        else if(a_statement -> is_expression()) {
            return interpret_expression(a_statement, l_scope, ns_name);
        }
        else {
            throw interpretation_error(declaration -> decl_token(), "[compiler error] unexpected statement encountered during statement interpretation.");
        }        
    }

    /**
     * interpret_while
     * given a while statemnt, we repeat ourselves like broken records
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_while(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<while_stmt> const & w_stmt = std::static_pointer_cast<while_stmt>(a_statement);
        std::shared_ptr<expr>& w_condition = w_stmt -> get_condition();
        std::shared_ptr<scope>& w_scope = w_stmt -> get_scope();
        block_stmt& w_body = w_stmt -> get_block();

        std::shared_ptr<expr> final_condition = interpret_expression(w_condition, w_scope, ns_name);
        std::shared_ptr<identifier_expression> condition_id = std::static_pointer_cast<identifier_expression>(final_condition);
        while(condition_id -> get_token() == true_cons_tok) {
            std::pair<bool, std::shared_ptr<expr> > res = interpret_block(w_body, w_scope, ns_name);
            // handle return statement
            if(res.first == true)
                return res;

            // handle break statement
            if(m_break_loop) {
                m_break_loop = false;
                return std::make_pair(false, nullptr);
            }

            // update the condition in case anything changed
            final_condition = interpret_expression(w_condition, w_scope, ns_name);
            condition_id = std::static_pointer_cast<identifier_expression>(final_condition);
        }

        return std::make_pair(false, nullptr);
    }

    /**
     * interpret_if
     * given an if statemnt, we make choices
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_if(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<if_stmt> const & i_stmt = std::static_pointer_cast<if_stmt>(a_statement);
        std::shared_ptr<expr>& i_condition = i_stmt -> get_condition();
        std::shared_ptr<scope>& i_scope = i_stmt -> get_scope();
        block_stmt& i_body = i_stmt -> get_block();
        bool check_else = true; // a flag that let us know whether to check the else branch

        // we first try the if condition
        std::shared_ptr<expr> i_final_condition = interpret_expression(i_condition, i_scope, ns_name);
        std::shared_ptr<identifier_expression> const & condition_id = std::static_pointer_cast<identifier_expression>(i_final_condition);
        if(condition_id -> get_token() == true_cons_tok) {
            check_else = false;
            std::pair<bool, std::shared_ptr<expr> > res = interpret_block(i_body, i_scope, ns_name);
            if(res.first == true)
                return res;
        }
        // so we can't run the main if body, we try the elifs
        else {
            std::vector<elif_branch>& i_elifs = i_stmt -> get_elifs();
            for(auto& i_elif : i_elifs) {
                std::shared_ptr<expr>& ei_condition = i_elif.get_condition();
                std::shared_ptr<scope>& ei_scope = i_elif.get_scope();
                block_stmt& ei_body = i_elif.get_block();
                std::shared_ptr<expr> ei_final_condition = interpret_expression(ei_condition, ei_scope, ns_name);
                std::shared_ptr<identifier_expression> const & condition_id = std::static_pointer_cast<identifier_expression>(ei_final_condition);
                if(condition_id -> get_token() == true_cons_tok) {
                    check_else = false;
                    std::pair<bool, std::shared_ptr<expr> > res = interpret_block(ei_body, ei_scope, ns_name);
                    if(res.first == true)
                        return res;
                    break; // we prevent the testing of other elifs branches if this one matches
                }
            }
        }

        // check the else branch
        if(check_else == true) {
            if(i_stmt -> has_else()) {
                else_branch e_branch = i_stmt -> get_else();
                std::shared_ptr<scope>& e_scope = e_branch.get_scope();
                block_stmt& e_body = e_branch.get_block();
                std::pair<bool, std::shared_ptr<expr> > res = interpret_block(e_body, e_scope, ns_name);
                if(res.first == true)
                    return res;
            }
        }

        return std::make_pair(false, nullptr);
    }

    /**
     * interpret_break
     * given a break statemnt, set the break flag to true
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_break(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        m_break_loop = true;
        return std::make_pair(false, nullptr);
    }

    /**
     * interpret_continue
     * given a continue statemnt, set the break flag to true
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_continue(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        m_continue_loop = true;
        return std::make_pair(false, nullptr);
    }

    /**
     * interpret_pass
     * given a pass statemnt, we do nothing
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_pass(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        return std::make_pair(false, nullptr);
    }

    /**
     * interpret_return
     * given a return statemnt, return the expression it contains
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_return(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<return_stmt> const & ret_stmt = std::static_pointer_cast<return_stmt>(a_statement);
        if(ret_stmt -> has_expression()) {
            std::shared_ptr<expr>& ret_expr = ret_stmt -> get_expression();
            return std::make_pair(true, interpret_expression(ret_expr, l_scope, ns_name));
        }
        else {
            return std::make_pair(true, nullptr);
        }
    }

    /**
     * interpret_expression
     * given an expression statement, compute its effective value and return it
     */
    std::pair<bool, std::shared_ptr<expr> > interpreter::interpret_expression(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expression_stmt> const & expr_stmt = std::static_pointer_cast<expression_stmt>(a_statement);
        std::shared_ptr<expr>& an_expression = expr_stmt -> get_expression();
        return std::make_pair(false, interpret_expression(an_expression, l_scope, ns_name));
    }

    std::shared_ptr<expr> interpreter::interpret_expression(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        if(an_expression -> is_underscore_expression()) {
            return an_expression;
        }
        else if(an_expression -> is_reference_expression()) {
            return interpret_reference(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_dereference_expression()) {
            return interpret_dereference(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_literal_expression()) {
            return an_expression;
        }
        else if(an_expression -> is_tuple_expression()) {
            return interpret_tuple(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_list_expression()) {
            return interpret_list(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_map_expression()) {
            return interpret_map(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_call_expression()) {
            return interpret_call(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_identifier_expression()) {
            return interpret_identifier(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_grouped_expression()) {
            return interpret_grouped(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_cast_expression()) {
            return interpret_cast(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_unary_expression()) {
            return interpret_unary(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_binary_expression()) {
            return interpret_binary(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_match_expression()) {
            return interpret_match(an_expression, l_scope, ns_name);
        }
        else if(an_expression -> is_assignment_expression()) {
            return interpret_assignment(an_expression, l_scope, ns_name);
        }
        else {
            throw interpretation_error(an_expression -> expr_token(), "[compiler error] unexpected expression encountered during expression interpretation.");
        }
    }

    /**
     * interpret_reference
     * given a reference expression, ...
     */
    std::shared_ptr<expr> interpreter::interpret_reference(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        return an_expression;
    }

    /**
     * interpret_dereference
     * given a dereference expression, find the value contained in the dereferenced variable and return it
     */
    std::shared_ptr<expr> interpreter::interpret_dereference(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<dereference_expression> const & dref_expr = std::static_pointer_cast<dereference_expression>(an_expression);
        std::shared_ptr<variable>& dref_var = dref_expr -> get_variable();
        std::shared_ptr<expr>& ref_expression = dref_var -> get_value();
        std::shared_ptr<reference_expression> const & ref_expr = std::static_pointer_cast<reference_expression>(ref_expression);
        std::shared_ptr<variable>& ref_var = ref_expr -> get_variable();
        std::shared_ptr<expr>& value = ref_var -> get_value();
        std::shared_ptr<scope>& var_scope = (ref_var -> is_global() == true) ? ref_var -> get_scope() : l_scope;
        return interpret_expression(value, var_scope, ns_name);
    }

    /**
     * interpret_tuple
     * given a tuple expression, compute the effective values of its elements, constructor a new tuple with the new elements and return it
     */
    std::shared_ptr<expr> interpreter::interpret_tuple(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<tuple_expression> const & tup_expr = std::static_pointer_cast<tuple_expression>(an_expression);
        std::vector<std::pair<std::string, std::shared_ptr<expr> > >& elements = tup_expr -> get_elements();
        type_instance& instance = tup_expr -> get_type_instance();
        token tok = tup_expr -> get_token();

        // create a new tuple expression and fill it with elements
        std::shared_ptr<tuple_expression> new_tup_expr = std::make_shared<tuple_expression>(tok);
        new_tup_expr -> set_type_instance(instance);
        for(auto& element : elements)
            new_tup_expr -> add_element(element.first, interpret_expression(element.second, l_scope, ns_name));

        std::shared_ptr<expr> final_expr = new_tup_expr;
        return final_expr;
    }

    /**
     * interpret_list
     * given a list expression, compute the effective values of its elements, constructor a new list with the new elements and return it
     */
    std::shared_ptr<expr> interpreter::interpret_list(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<list_expression> const & list_expr = std::static_pointer_cast<list_expression>(an_expression);
        std::vector<std::shared_ptr<expr> >& elements = list_expr -> get_elements();
        type_instance& instance = list_expr -> get_type_instance();
        token tok = list_expr -> get_token();

        // create a new list expression and fill it with elements
        std::shared_ptr<list_expression> new_list_expr = std::make_shared<list_expression>(tok);
        new_list_expr -> set_type_instance(instance);
        for(auto& element : elements)
            new_list_expr -> add_element(interpret_expression(element, l_scope, ns_name));

        std::shared_ptr<expr> final_expr = new_list_expr;
        return final_expr;
    }

    /**
     * interpret_map
     * given a map expression, compute the effective values of its elements, constructor a new map with the new elements and return it
     */
    std::shared_ptr<expr> interpreter::interpret_map(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<map_expression> const & map_expr = std::static_pointer_cast<map_expression>(an_expression);
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& elements = map_expr -> get_elements();
        type_instance& instance = map_expr -> get_type_instance();
        token tok = map_expr -> get_token();

        // create a new map expression and fill it with elements
        std::shared_ptr<map_expression> new_map_expr = std::make_shared<map_expression>(tok);
        new_map_expr -> set_comparator_callee(map_expr -> get_comparator_callee());
        new_map_expr -> set_callee(map_expr -> get_callee());
        new_map_expr -> set_type_instance(instance);

        // set up new elements if the map is not empty
        if(elements.size() > 0) {
            for(auto& element : elements) {
                std::shared_ptr<expr> key = interpret_expression(element.first, l_scope, ns_name);
                std::shared_ptr<expr> value = interpret_expression(element.second, l_scope, ns_name);
                new_map_expr -> add_element(key, value);
            }
        }        

        std::shared_ptr<expr> final_expr = new_map_expr;
        return final_expr;
    }

    /**
     * interpret_call
     * given a call expression:
     * - if it is a default constructor, go over its arguments and compute their effective values, construct a call expression and return it
     * - if it is a record constructor, do the same as for record constructor
     * - if it is a function call, interpret the call and return the returned expression
     */
    std::shared_ptr<expr> interpreter::interpret_call(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(an_expression);
        std::vector<std::pair<token, std::shared_ptr<expr> > >& arguments = call_expr -> get_arguments();
        call_expression_type expr_type = call_expr -> get_expression_type();
        const std::string& cons_ns = call_expr -> get_namespace();
        type_instance instance = call_expr -> get_type_instance();
        token tok = call_expr -> get_token();

        if(call_expr -> get_expression_type() == DEFAULT_CONSTRUCTOR_EXPR || call_expr -> get_expression_type() == RECORD_CONSTRUCTOR_EXPR) {
            std::shared_ptr<call_expression> cons_call_expr = std::make_shared<call_expression>(tok);
            cons_call_expr -> set_type_instance(instance);
            cons_call_expr -> set_namespace(cons_ns);
            cons_call_expr -> set_expression_type(expr_type);
            for(auto& argument : arguments) {
                std::shared_ptr<expr> cons_argument = interpret_expression(argument.second, l_scope, ns_name);
                cons_call_expr -> add_argument(argument.first, cons_argument);
            }
            std::shared_ptr<expr> final_expr = cons_call_expr;
            return final_expr;
        }
        // if the call expression is not a default or record constructor expression then it is a call expression
        else {
            // find the function
            std::string fun_ns_name = "";
            std::string fun_name = "";
            std::size_t fun_arity = 0;
            std::tie(fun_ns_name, fun_name, fun_arity) = call_expr -> get_callee();            
            std::shared_ptr<function>& function_decl = l_scope -> get_function(fun_ns_name, fun_name, fun_arity);
            // build arguments
            std::vector<std::shared_ptr<expr> > fun_arguments;
            for(auto& argument : arguments) {
                std::shared_ptr<expr> fun_argument = interpret_expression(argument.second, l_scope, ns_name);
                fun_arguments.push_back(fun_argument);
            }
            // "call" the function
            return interpret_function(function_decl, fun_arguments);
        }
    }

    /**
     * interpret_identifier
     * if we have a:
     * - a variable, find it in the symbol table and return the value it contains
     * - a default constructor, return it
     */
    std::shared_ptr<expr> interpreter::interpret_identifier(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(an_expression);
        if(id_expr -> get_expression_type() == VAR_EXPR) {
            try {
                std::shared_ptr<variable>& variable_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
                std::shared_ptr<expr>& value = variable_decl -> get_value();
                std::shared_ptr<scope>& var_scope = (variable_decl -> is_global() == true) ? variable_decl -> get_scope() : l_scope;
                return interpret_expression(value, var_scope, ns_name);
            } catch(symbol_not_found err) {
                throw interpretation_error(id_expr -> get_token(), err.what());
            }
        }
        // if the identifier expression is not a variable expression, then it is a default constructor expression
        else {
            // if we have a default constructor, we return it since it is basically like a user defined literal
            return an_expression;
        }
    }

    /**
     * interpret_grouped
     * return the effective value of the expression contained in the grouped expression
     */
    std::shared_ptr<expr> interpreter::interpret_grouped(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<grouped_expression> const & gr_expr = std::static_pointer_cast<grouped_expression>(an_expression);
        std::shared_ptr<expr>& value = gr_expr -> get_value();
        return interpret_expression(value, l_scope, ns_name);
    }

    /**
     * interpret_cast
     * perform the function call corresponding to the cast expression and return the function call returned value
     */
    std::shared_ptr<expr> interpreter::interpret_cast(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<cast_expression> const & cast_expr = std::static_pointer_cast<cast_expression>(an_expression);
        std::shared_ptr<expr>& value = cast_expr -> get_val();
        type_instance& value_instance = value -> expr_type_instance();
        
        // find the function that will perform the cast
        std::shared_ptr<function>& function_decl = l_scope -> get_function(value_instance.get_namespace(), cast_expr -> get_callee(), 1);
        
        // build arguments
        std::shared_ptr<expr> argument = interpret_expression(value, l_scope, ns_name);
        std::vector<std::shared_ptr<expr> > arguments {
            argument
        };

        // "call" the function        
        return interpret_function(function_decl, arguments, cast_expr -> get_cast_type_instance());
    }

    /**
     * interpret_unary
     * perform the function call corresponding to the unary operator and return the function call returned value
     */
    std::shared_ptr<expr> interpreter::interpret_unary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<unary_expression> const & unary_expr = std::static_pointer_cast<unary_expression>(an_expression);
        std::shared_ptr<expr>& value = unary_expr -> get_val();
        type_instance& value_instance = value -> expr_type_instance();
        
        // find the function that will perform the unary operation
        std::shared_ptr<function>& function_decl = l_scope -> get_function(value_instance.get_namespace(), unary_expr -> get_callee(), 1);

        // build arguments
        std::shared_ptr<expr> argument = interpret_expression(value, l_scope, ns_name);
        std::vector<std::shared_ptr<expr> > arguments {
            argument
        };

        // "call" the function
        return interpret_function(function_decl, arguments);
    }

    /**
     * interpret_binary
     * perform the function call corresponding to the binary operator and return the function call returned value
     */
    std::shared_ptr<expr> interpreter::interpret_binary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<binary_expression> const & bin_expr = std::static_pointer_cast<binary_expression>(an_expression);
        binary_expression_type expr_type = bin_expr -> get_expression_type();

        // work on operations that decay into functions
        if(
           expr_type == PLUS_EXPR           ||
           expr_type == MINUS_EXPR          ||
           expr_type == MUL_EXPR            ||
           expr_type == DIV_EXPR            ||
           expr_type == MOD_EXPR            ||
           expr_type == POW_EXPR            ||
           expr_type == LEFT_SHIFT_EXPR     ||
           expr_type == RIGHT_SHIFT_EXPR    ||
           expr_type == LOGICAL_AND_EXPR    ||
           expr_type == LOGICAL_OR_EXPR     ||
           expr_type == BITWISE_AND_EXPR    ||
           expr_type == BITWISE_OR_EXPR     ||
           expr_type == BITWISE_XOR_EXPR    ||
           expr_type == EQUAL_EQUAL_EXPR    ||
           expr_type == NOT_EQUAL_EXPR      ||
           expr_type == LESS_EXPR           ||
           expr_type == LESS_EQUAL_EXPR     ||
           expr_type == GREATER_EXPR        ||
           expr_type == GREATER_EQUAL_EXPR

        ) {
            return interpret_functional_binary(bin_expr, l_scope, ns_name);
        }
        else if(expr_type == IS_EXPR || expr_type == IS_NOT_EXPR) {
            return interpret_is_binary(bin_expr, l_scope, ns_name);
        }
        else if(expr_type == DOT_EXPR) {
            return interpret_dot_binary(bin_expr, l_scope, ns_name);
        }
        else if(expr_type == SUBSCRIPT_EXPR) {
            return interpret_subscript_binary(bin_expr, l_scope, ns_name);
        }
        else {
            throw interpretation_error(bin_expr -> get_token(), "[compiler error] unexpected binary operator during interpretation.");
        }
    }

    std::shared_ptr<expr> interpreter::interpret_functional_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();
        type_instance& lval_instance = lval -> expr_type_instance();
        
        // find the function that will perform the binary operation
        std::shared_ptr<function>& function_decl = l_scope -> get_function(lval_instance.get_namespace(), bin_expr -> get_callee(), 2);

        // build arguments
        std::shared_ptr<expr> argument_one = interpret_expression(lval, l_scope, ns_name);
        std::shared_ptr<expr> argument_two = interpret_expression(rval, l_scope, ns_name);
        std::vector<std::shared_ptr<expr> > arguments {
            argument_one,
            argument_two
        };

        // "call" the function
        return interpret_function(function_decl, arguments);
    }

    std::shared_ptr<expr> interpreter::interpret_is_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();

        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // we get the variable contained in the lval and rval references
        std::shared_ptr<variable> lval_var = nullptr;
        std::shared_ptr<variable> rval_var = nullptr;

        // if the lval is a reference expression
        if(lval -> is_reference_expression()) {
            std::shared_ptr<reference_expression> const & lval_ref = std::static_pointer_cast<reference_expression>(lval);
            lval_var = lval_ref -> get_variable();
        }
        // otherwise it is an identifier expression
        else if(lval -> is_identifier_expression()) {
            std::shared_ptr<expr> ref_expr = interpret_identifier(lval, l_scope, ns_name);
            std::shared_ptr<reference_expression> const & lval_ref = std::static_pointer_cast<reference_expression>(ref_expr);
            lval_var = lval_ref -> get_variable();
        }

        // if the rval is a reference expression
        if(rval -> is_reference_expression()) {
            std::shared_ptr<reference_expression> const & rval_ref = std::static_pointer_cast<reference_expression>(rval);
            rval_var = rval_ref -> get_variable();
        }
        // otherwise it is an identifier expression
        else if(rval -> is_identifier_expression()) {
            std::shared_ptr<expr> ref_expr = interpret_identifier(rval, l_scope, ns_name);
            std::shared_ptr<reference_expression> const & rval_ref = std::static_pointer_cast<reference_expression>(ref_expr);
            rval_var = rval_ref -> get_variable();
        }

        if(bin_expr -> get_expression_type() == IS_EXPR)
            return (lval_var == rval_var) ? true_expr : false_expr;
        else
            return (lval_var == rval_var) ? false_expr : true_expr;
    }

    std::shared_ptr<expr> interpreter::interpret_dot_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        const std::string& name = id_expr -> get_name();
        const std::string& sub_ns_name = id_expr -> get_namespace();

        if(l_scope -> has_namespace(name)) {
            return interpret_namespace_binary(bin_expr, l_scope, ns_name);
        }
        else if(l_scope -> variable_exists(sub_ns_name, name)) {
            return interpret_variable_binary(bin_expr, l_scope, ns_name);
        }
        else {
            throw interpretation_error(bin_expr -> get_token(), "[compiler error] unexpected binary expression during interpretation a binary dot expression.");
        }
    }

    std::shared_ptr<expr> interpreter::interpret_subscript_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        const std::string& name = id_expr -> get_name();
        const std::string& sub_ns_name = id_expr -> get_namespace();

        if(l_scope -> variable_exists(sub_ns_name, name)) {
            return interpret_variable_subscript(bin_expr, l_scope, ns_name);
        }
        else {
            throw interpretation_error(bin_expr -> get_token(), "[compiler error] unexpected binary expression during interpretation of a binary subscript expression.");
        }
    }

    std::shared_ptr<expr> interpreter::interpret_namespace_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();

        if(rval -> is_identifier_expression()) {
            return interpret_identifier(rval, l_scope, ns_name);
        }
        else if(rval -> is_call_expression()) {
            return interpret_call(rval, l_scope, ns_name);
        }
        else if(rval -> is_binary_expression()) {
            return interpret_binary(rval, l_scope, ns_name);
        }
        else {
            throw interpretation_error(rval -> expr_token(), "[compiler error] unexpected rval in binary expression during interpretation of binary expression starting with a namespace.");
        }
    }

    std::shared_ptr<expr> interpreter::interpret_variable_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();

        if(rval -> is_identifier_expression()) {
            return interpret_variable_attribute(bin_expr, l_scope, ns_name);
        }
        else {
            throw interpretation_error(rval -> expr_token(), "[compiler error] unexpected rval when interpreting a binary expression where the lval is a variable.");
        }
    }

    std::shared_ptr<expr> interpreter::interpret_variable_attribute(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
        std::shared_ptr<scope>& var_scope = (var_decl -> is_global() == true) ? var_decl -> get_scope() : l_scope;

        if(var_decl -> is_global()) {
            std::shared_ptr<expr>& var_value = var_decl -> get_value();
            if(var_value -> is_tuple_expression()) {
                return interpret_tuple_attribute(bin_expr, var_scope, ns_name);
            }
            else {
                return interpret_custom_attribute(bin_expr, var_scope, ns_name);
            }
        }
        else {
            return interpret_custom_attribute(bin_expr, var_scope, ns_name);
        }
    }

    std::shared_ptr<expr> interpreter::interpret_tuple_attribute(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();

        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
        std::shared_ptr<scope>& var_scope = (var_decl -> is_global() == true) ? var_decl -> get_scope() : l_scope;
        std::shared_ptr<expr>& var_value = var_decl -> get_value();
        std::shared_ptr<tuple_expression> const & tuple_expr = std::static_pointer_cast<tuple_expression>(var_value);
        std::vector<std::pair<std::string, std::shared_ptr<expr> > >& elements = tuple_expr -> get_elements();

        for(auto& element : elements) {
            if(element.first == rval -> expr_token().get_lexeme()) {
                std::shared_ptr<expr>& tup_value = element.second;
                return interpret_expression(tup_value, var_scope, ns_name);
            }
        }

        // we should never reach here but the compiler function termination checker will complain if we don't have this
        return nullptr;
    }

    std::shared_ptr<expr> interpreter::interpret_custom_attribute(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();

        // get the __getattr__ function
        type_instance& lval_instance = lval -> expr_type_instance();
        std::shared_ptr<function>& function_decl = l_scope -> get_function(lval_instance.get_namespace(), bin_expr -> get_callee(), 1);

        // build arguments
        std::shared_ptr<expr> argument = interpret_expression(lval, l_scope, ns_name);
        std::vector<std::shared_ptr<expr> > arguments = {
            argument
        };

        // "call" the function
        return interpret_function(function_decl, arguments);
    }

    std::shared_ptr<expr> interpreter::interpret_variable_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
        //std::shared_ptr<scope>& var_scope = (var_decl -> is_global() == true) ? variable_decl -> get_scope() : l_scope;
        type_instance& var_instance = var_decl -> get_type_instance();

        if(var_instance.get_category() == TUPLE) {
            return interpret_tuple_subscript(bin_expr, l_scope, ns_name);
        }
        else if(var_instance.get_category() == LIST) {
            return interpret_list_subscript(bin_expr, l_scope, ns_name);
        }
        else if(var_instance.get_category() == MAP) {
            return interpret_map_subscript(bin_expr, l_scope, ns_name);
        }
        else {
            return interpret_custom_subscript(bin_expr, l_scope, ns_name);
        }
    }

    std::shared_ptr<expr> interpreter::interpret_tuple_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();
        
        // get the tuple expression inside the lval (which is a variable expression)
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
        std::shared_ptr<scope>& var_scope = (var_decl -> is_global() == true) ? var_decl -> get_scope() : l_scope;
        std::shared_ptr<expr>& var_value = var_decl -> get_value();
        std::shared_ptr<expr> value = interpret_expression(var_value, var_scope, ns_name);
        std::shared_ptr<tuple_expression> const & tup_expr = std::static_pointer_cast<tuple_expression>(value);
        std::vector<std::pair<std::string, std::shared_ptr<expr> > >& elements = tup_expr -> get_elements();
        
        // get the key
        std::size_t key = 0;
        std::shared_ptr<literal_expression> const & key_expr = std::static_pointer_cast<literal_expression>(rval);
        std::sscanf(rval -> expr_token().get_lexeme().c_str(), "%zu", &key);

        // deduce and return the expression
        std::pair<std::string, std::shared_ptr<expr> >& element = elements.at(key);
        std::shared_ptr<expr> final_expr = interpret_expression(element.second, l_scope, ns_name);
        return final_expr;
    }

    std::shared_ptr<expr> interpreter::interpret_list_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();

        // get the list expression inside the lval (which is a variable expression)
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());        
        std::shared_ptr<scope>& var_scope = (var_decl -> is_global() == true) ? var_decl -> get_scope() : l_scope;
        std::shared_ptr<expr>& var_value = var_decl -> get_value();
        std::shared_ptr<expr> value = interpret_expression(var_value, var_scope, ns_name);
        std::shared_ptr<list_expression> const & list_expr = std::static_pointer_cast<list_expression>(value);
        std::vector<std::shared_ptr<expr> >& elements = list_expr -> get_elements();
        
        // get the key
        long long int key = 0;
        std::shared_ptr<expr> key_expr = interpret_expression(rval, l_scope, ns_name);
        if(key_expr -> is_literal_expression() == false)
            throw interpretation_error(rval -> expr_token(), "Unexpected expression used as key. Excepted an integer.");
        std::shared_ptr<literal_expression> const & lit_expr = std::static_pointer_cast<literal_expression>(key_expr);
        if(lit_expr -> get_expression_type() == INTEGER_EXPR)
            key = lit_expr -> get_int_value();
        else
            throw interpretation_error(lit_expr -> get_token(), "Expected an integer expression.");

        // list subscript access returns a maybe object
        type_instance& list_instance = list_expr -> get_type_instance();
        std::vector<type_instance> list_instance_params = list_instance.get_params();
        avalon_maybe avl_maybe;
        type_instance ret_instance = avl_maybe.get_type_instance(list_instance_params[0]);

        try {
            // deduce and return the expression
            std::shared_ptr<expr>& element = elements.at(key);
            std::shared_ptr<expr> element_expr = interpret_expression(element, l_scope, ns_name);
            std::shared_ptr<call_expression> ret_expr = std::make_shared<call_expression>(just_cons_tok);
            ret_expr -> add_argument(star_tok, element_expr);
            ret_expr -> set_expression_type(DEFAULT_CONSTRUCTOR_EXPR);
            ret_expr -> set_type_instance(ret_instance);
            std::shared_ptr<expr> final_expr = ret_expr;
            return final_expr;
        } catch(std::out_of_range err) {
            // get the type instance of elements in the list
            std::shared_ptr<identifier_expression> ret_expr = std::make_shared<identifier_expression>(none_cons_tok);
            ret_expr -> set_type_instance(ret_instance);
            ret_expr -> set_expression_type(CONSTRUCTOR_EXPR);
            std::shared_ptr<expr> final_expr = ret_expr;
            return final_expr;
        }
    }

    std::shared_ptr<expr> interpreter::interpret_map_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();
        std::shared_ptr<expr>& rval = bin_expr -> get_rval();

        // get the list expression inside the lval (which is a variable expression)
        std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
        std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
        std::shared_ptr<scope>& var_scope = (var_decl -> is_global() == true) ? var_decl -> get_scope() : l_scope;
        std::shared_ptr<expr>& var_value = var_decl -> get_value();
        std::shared_ptr<expr> value = interpret_expression(var_value, var_scope, ns_name);
        std::shared_ptr<map_expression> const & map_expr = std::static_pointer_cast<map_expression>(value);
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& elements = map_expr -> get_elements();
        type_instance& map_instance = map_expr -> get_type_instance();

        // return type instance
        std::vector<type_instance> map_instance_params = map_instance.get_params();
        avalon_maybe avl_maybe;
        type_instance ret_instance = avl_maybe.get_type_instance(map_instance_params[1]);

        // failure return value
        std::shared_ptr<identifier_expression> ret_expr = std::make_shared<identifier_expression>(none_cons_tok);
        ret_expr -> set_type_instance(ret_instance);
        ret_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> final_expr = ret_expr;

        // if the map is empty, we return None
        if(elements.size() == 0) {
            return final_expr;
        }

        // get the namespace
        std::shared_ptr<expr> key = interpret_expression(rval, l_scope, ns_name);
        type_instance& key_instance = key -> expr_type_instance();

        // get the hash function and hash the key
        std::shared_ptr<function>& hash_function = l_scope -> get_function(key_instance.get_namespace(), map_expr -> get_callee(), 1);
        std::vector<std::shared_ptr<expr> > hash_arguments = { key };
        std::shared_ptr<expr> hashed_key = interpret_function(hash_function, hash_arguments);

        // get the key comparator function
        std::shared_ptr<function>& eq_function = l_scope -> get_function("*", map_expr -> get_comparator_callee(), 2);

        // deduce and return the expression
        for(auto& element : elements) {
            std::shared_ptr<expr> key_expr = interpret_expression(element.first, l_scope, ns_name);
            // hash the map key
            std::vector<std::shared_ptr<expr> > hash_arguments = { key_expr };
            std::shared_ptr<expr> hashed_key_expr = interpret_function(hash_function, hash_arguments);
            // compare it with the given key
            std::vector<std::shared_ptr<expr> > eq_arguments = { hashed_key, hashed_key_expr };
            std::shared_ptr<expr> res = interpret_function(eq_function, eq_arguments);
            // the resulting expression is identifier expression containing a boolean
            std::shared_ptr<identifier_expression> const & res_id = std::static_pointer_cast<identifier_expression>(res);
            // while we could build an another boolean comparator function but for now we just compare the names here
            if(res_id -> get_token() == true_cons_tok) {
                std::shared_ptr<expr> element_expr = interpret_expression(element.second, l_scope, ns_name);
                std::shared_ptr<call_expression> ret_expr = std::make_shared<call_expression>(just_cons_tok);
                ret_expr -> add_argument(star_tok, element_expr);
                ret_expr -> set_expression_type(DEFAULT_CONSTRUCTOR_EXPR);
                ret_expr -> set_type_instance(ret_instance);
                std::shared_ptr<expr> final_expr = ret_expr;
                return final_expr;
            }
        }

        // we didn't find the value
        return final_expr;
    }

    std::shared_ptr<expr> interpreter::interpret_custom_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<expr>& lval = bin_expr -> get_lval();

        // get the __getitem__ function
        type_instance& lval_instance = lval -> expr_type_instance();
        std::shared_ptr<function>& function_decl = l_scope -> get_function(lval_instance.get_namespace(), bin_expr -> get_callee(), 1);

        // build arguments
        std::shared_ptr<expr> argument = interpret_expression(lval, l_scope, ns_name);
        std::vector<std::shared_ptr<expr> > arguments = {
            argument
        };

        // "call" the function
        return interpret_function(function_decl, arguments);
    }

    /**
     * interpret_match
     * compare the lval to the rval and update the values of any variables if any captures were requested
     */
    std::shared_ptr<expr> interpreter::interpret_match(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<match_expression> const & match_expr = std::static_pointer_cast<match_expression>(an_expression);
        std::shared_ptr<expr>& lval = match_expr -> get_lval();
        std::shared_ptr<expr>& rval = match_expr -> get_rval();

        std::shared_ptr<expr> lval_expr = interpret_expression(lval, l_scope, ns_name);
        if(lval_expr -> is_literal_expression()) {
            return interpret_literal_match(lval_expr, rval, l_scope, ns_name);
        }
        else if(lval_expr -> is_identifier_expression()) {
            return interpret_identifier_match(lval_expr, rval, l_scope, ns_name);
        }
        else if(lval_expr -> is_call_expression()) {
            return interpret_call_match(lval_expr, rval, l_scope, ns_name);
        }
        else {
            throw interpretation_error(lval_expr -> expr_token(), "[compiler error] unexpected effective expression as lval in match expression.");
        }
    }

    std::shared_ptr<expr> interpreter::interpret_literal_match(std::shared_ptr<expr>& lval_expr, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;
        
        // if the rval is not a literal expression, we return false
        if(rval -> is_literal_expression() == false)
            return false_final_expr;

        // if it is, we compare both for value
        std::shared_ptr<literal_expression> const & lval_lit = std::static_pointer_cast<literal_expression>(lval_expr);
        std::shared_ptr<literal_expression> const & rval_lit = std::static_pointer_cast<literal_expression>(rval);
        const literal_expression_type& expr_type = lval_lit -> get_expression_type();

        // if they have different expression types, we return false
        if(expr_type != rval_lit -> get_expression_type())
            return false_final_expr;

        // if we have the integers
        if(expr_type == INTEGER_EXPR) {
            long long int lval_value = lval_lit -> get_int_value();
            long long int rval_value = rval_lit -> get_int_value();
            if(lval_value == rval_value)
                return true_final_expr;
            else
                return false_final_expr;
        }
        else if(expr_type == FLOATING_POINT_EXPR) {
            double lval_value = lval_lit -> get_float_value();
            double rval_value = rval_lit -> get_float_value();
            if(lval_value == rval_value)
                return true_final_expr;
            else
                return false_final_expr;
        }
        else {
            std::string lval_value = lval_lit -> get_string_value();
            std::string rval_value = rval_lit -> get_string_value();
            if(lval_value == rval_value)
                return true_final_expr;
            else
                return false_final_expr;
        }
    }

    std::shared_ptr<expr> interpreter::interpret_identifier_match(std::shared_ptr<expr>& lval_expr, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // if the rval is not an identifier expression, we return false
        if(rval -> is_identifier_expression() == false)
            return false_final_expr;

        // if it is, we compare both
        std::shared_ptr<identifier_expression> const & lval_id = std::static_pointer_cast<identifier_expression>(lval_expr);
        std::shared_ptr<identifier_expression> const & rval_id = std::static_pointer_cast<identifier_expression>(rval);
        if(lval_id -> get_name() != rval_id -> get_name())
            return false_final_expr;
        else
            return true_final_expr;
    }

    std::shared_ptr<expr> interpreter::interpret_call_match(std::shared_ptr<expr>& lval_expr, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        // bool type
        avalon_bool avl_bool;
        type_instance bool_instance = avl_bool.get_type_instance();

        // true expression
        std::shared_ptr<identifier_expression> true_expr = std::make_shared<identifier_expression>(true_cons_tok);
        true_expr -> set_type_instance(bool_instance);
        true_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> true_final_expr = true_expr;

        // false expression
        std::shared_ptr<identifier_expression> false_expr = std::make_shared<identifier_expression>(false_cons_tok);
        false_expr -> set_type_instance(bool_instance);
        false_expr -> set_expression_type(CONSTRUCTOR_EXPR);
        std::shared_ptr<expr> false_final_expr = false_expr;

        // if the rval is not a call expression, we return false
        if(rval -> is_call_expression() == false)
            return false_final_expr;

        // if it is, we compare both
        std::shared_ptr<call_expression> const & lval_call = std::static_pointer_cast<call_expression>(lval_expr);
        std::shared_ptr<call_expression> const & rval_call = std::static_pointer_cast<call_expression>(rval);
        
        // return false if they have different names
        if(lval_call -> get_name() != rval_call -> get_name())
            return false_final_expr;

        // return false if they have different number of arguments
        std::vector<std::pair<token, std::shared_ptr<expr> > >& lval_args = lval_call -> get_arguments();
        std::vector<std::pair<token, std::shared_ptr<expr> > >& rval_args = rval_call -> get_arguments();
        if(lval_args.size() != rval_args.size())
            return false_final_expr;

        // compare arguments while capturing variables where necessary
        auto lval_it = lval_args.begin(), lval_end = lval_args.end();
        auto rval_it = rval_args.begin(), rval_end = rval_args.end();
        for(; lval_it != lval_end && rval_it != rval_end; ++lval_it, ++rval_it) {
            // if the rval argument is an underscore expression, we move on
            if(rval_it -> second -> is_underscore_expression()) {
                continue;
            }
            // if it is a literal, we compare both
            else if(rval_it -> second -> is_literal_expression()) {
                std::shared_ptr<expr> res = interpret_literal_match(lval_it -> second, rval_it -> second, l_scope, ns_name);
                if(res -> expr_token() == false_cons_tok)
                    return false_final_expr;
            }
            // if it is an identifier expression, it could be a variable or a constructor
            else if(rval_it -> second -> is_identifier_expression()) {
                std::shared_ptr<identifier_expression> const & rval_id = std::static_pointer_cast<identifier_expression>(rval_it -> second);
                // we have a constructor, we compare the names
                if(rval_id -> get_expression_type() == CONSTRUCTOR_EXPR) {
                    std::shared_ptr<expr> res = interpret_identifier_match(lval_it -> second, rval_it -> second, l_scope, ns_name);
                    if(res -> expr_token() == false_cons_tok)
                        return false_final_expr;
                }
                // we have variable, we put the lval value into it
                else {
                    std::shared_ptr<variable>& var_decl = l_scope -> get_variable(rval_id -> get_namespace(), rval_id -> get_name());
                    std::shared_ptr<expr> var_value = interpret_expression(lval_it -> second, l_scope, ns_name);
                    var_decl -> set_value(var_value);
                }
            }
            // we have a call expression, then we perform a recursive call to this function
            else if(rval_it -> second -> is_call_expression()) {
                std::shared_ptr<expr> res = interpret_call_match(lval_it -> second, rval_it -> second, l_scope, ns_name);
                if(res -> expr_token() == false_cons_tok)
                    return false_final_expr;
            }
            // if we have a binary expression, we check the rval as usual above because the lval of the binary expression is just the namespace
            else if(rval_it -> second -> is_binary_expression()) {
                if(lval_it -> second -> is_binary_expression() == false)
                    return false_final_expr;

                std::shared_ptr<binary_expression> const & lval_bin = std::static_pointer_cast<binary_expression>(lval_it -> second);
                std::shared_ptr<binary_expression> const & rval_bin = std::static_pointer_cast<binary_expression>(rval_it -> second);
                std::shared_ptr<expr>& lbin_rval = lval_bin -> get_rval();
                std::shared_ptr<expr>& rbin_rval = rval_bin -> get_rval();

                // if both the lval and rval are identifier expression
                if(lbin_rval -> is_identifier_expression() == true && rbin_rval -> is_identifier_expression() == true) {
                    std::shared_ptr<expr> res = interpret_identifier_match(lbin_rval, rbin_rval, l_scope, ns_name);
                    if(res -> expr_token() == false_cons_tok)
                        return false_final_expr;
                }
                // in case both are call expressions
                else if(lbin_rval -> is_call_expression() == true && rbin_rval -> is_call_expression() == true) {
                    std::shared_ptr<expr> res = interpret_call_match(lbin_rval, rbin_rval, l_scope, ns_name);
                    if(res -> expr_token() == false_cons_tok)
                        return false_final_expr;
                }
                // anything else is wrong
                else {
                    throw interpretation_error(rval_bin -> get_token(), "[compiler error] unexpected binary expression in match expression.");
                }
            }
        }

        // if we reached here then everything checked out
        return true_final_expr;
    }

    /**
     * interpret_assignment
     * get the effective value of the rval and assign it to the variable in the lval
     */
    std::shared_ptr<expr> interpreter::interpret_assignment(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<assignment_expression> const & assign_expr = std::static_pointer_cast<assignment_expression>(an_expression);
        std::shared_ptr<expr>& lval = assign_expr -> get_lval();
        std::shared_ptr<expr>& rval = assign_expr -> get_rval();

        // we get the effective value to put into the lval from the rval
        std::shared_ptr<expr> value = interpret_expression(rval, l_scope, ns_name);

        // get the variable in the lval
        if(lval -> is_identifier_expression()) {
            std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(lval);
            std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
            var_decl -> set_value(value);
        }
        else if(lval -> is_binary_expression()) {
            std::shared_ptr<binary_expression> const & bin_expr = std::static_pointer_cast<binary_expression>(lval);            
            std::shared_ptr<expr>& bin_rval = bin_expr -> get_rval();
            std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(bin_rval);
            std::shared_ptr<variable>& var_decl = l_scope -> get_variable(id_expr -> get_namespace(), id_expr -> get_name());
            var_decl -> set_value(value);
        }
        else if(lval -> is_dereference_expression()) {
            std::shared_ptr<dereference_expression> const & dref_expr = std::static_pointer_cast<dereference_expression>(lval);
            std::shared_ptr<variable>& dref_var = dref_expr -> get_variable();
            std::shared_ptr<expr>& ref_expression = dref_var -> get_value();
            std::shared_ptr<reference_expression> const & ref_expr = std::static_pointer_cast<reference_expression>(ref_expression);
            std::shared_ptr<variable>& ref_var = ref_expr -> get_variable();
            ref_var -> set_value(value);
        }
        else {
            throw interpretation_error(lval -> expr_token(), "[compiler error] unexpected lval to assignment expression.");
        }

        return value;
    }

    /**
     * import_declarations
     * we imports immediate declarations into the given program scope from the imported scope
     */
    void interpreter::import_declarations() {
        std::unordered_map<std::string, program>& progs = m_gtable.get_programs();

        for(auto& prog : progs) {
            std::vector<std::shared_ptr<decl> >& declarations = prog.second.get_declarations();
            for(auto& declaration : declarations) {
                // perform the import of declared imports
                if(declaration -> is_import()) {
                    const std::shared_ptr<import>& import_decl = std::static_pointer_cast<import>(declaration);
                    program& imported_prog = m_gtable.get_program(import_decl -> get_fqn_name());
                    import_declarations(imported_prog, prog.second);
                }
            }
        }
    }

    void interpreter::import_declarations(program& from, program& to) {
        // declarations defined in from with hope of find namespaces with declarations in them
        std::vector<std::shared_ptr<decl> >& declarations = from.get_declarations();
        std::vector<std::shared_ptr<ns> > namespace_decls;
        // the scope into which to insert the found declarations
        std::shared_ptr<scope>& to_scope = to.get_scope();

        // we get all the namespaces defined in the "from" program        
        for(auto& declaration : declarations) {
            if(declaration -> is_namespace()) {
                std::shared_ptr<ns> namespace_decl = std::static_pointer_cast<ns>(declaration);
                namespace_decls.push_back(namespace_decl);
                to_scope -> add_namespace(namespace_decl -> get_name());
            }
        }

        // go over the namespaces and insert the public declarations found in them into the "to" scope
        for(auto& namespace_decl : namespace_decls) {
            std::vector<std::shared_ptr<decl> >& l_declarations = namespace_decl -> get_declarations();
            for(auto& l_declaration : l_declarations) {
                const std::string& namespace_name = namespace_decl -> get_name();
                if(l_declaration -> is_type()) {
                    std::shared_ptr<type> type_decl = std::static_pointer_cast<type>(l_declaration);
                    if(type_decl -> is_public()) {
                        try {
                            to_scope -> add_type(namespace_name, type_decl);
                        } catch(symbol_already_declared err) {
                            throw interpretation_error(type_decl -> get_token(), err.what());
                        }
                    }
                }
                else if(l_declaration -> is_function()) {
                    std::shared_ptr<function> function_decl = std::static_pointer_cast<function>(l_declaration);
                    if(function_decl -> is_public()) {
                        try {
                            to_scope -> add_function(namespace_name, function_decl);
                        } catch(symbol_already_declared err) {
                            throw interpretation_error(function_decl -> get_token(), err.what());
                        }
                    }
                }
                else if(l_declaration -> is_variable()) {
                    std::shared_ptr<variable> variable_decl = std::static_pointer_cast<variable>(l_declaration);
                    if(variable_decl -> is_public()) {
                        try {
                            to_scope -> add_variable(namespace_name, variable_decl);
                        } catch(symbol_already_declared err) {
                            throw interpretation_error(variable_decl -> get_token(), err.what());
                        }
                    }
                }
                else {
                    throw std::runtime_error("Unknown declaration found in program.");
                }
            }
        }
    }


    /**
     * interpretation_error
     * contructs and returns a interpret_error exception
     */
    interpret_error interpreter::interpretation_error(const token& tok, const std::string& message) {
        return interpret_error(m_error_handler, tok, message);
    }
}
