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

#ifndef AVALON_INTERPRETER_HPP_
#define AVALON_INTERPRETER_HPP_

#include <stdexcept>
#include <utility>
#include <memory>
#include <vector>
#include <string>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* AST */
/* Declarations */
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/program.hpp"
/* Statements */
#include "representer/ast/stmt/block_stmt.hpp"
/* Expressions */
#include "representer/ast/expr/binary_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Symtable */
#include "representer/symtable/gtable.hpp"
#include "representer/symtable/scope.hpp"


namespace avalon {
    class interpret_error : public std::runtime_error {
    public:
        /**
         * the contructor expects:
         * - the error handler to be used to show errors
         * - the token where the error occured
         * - the message to show the user
         */
        interpret_error(error& error_handler, token tok, const std::string& message);

        /**
         * what
         * get the error message
         */
        virtual const char * what() const noexcept;

        /**
         * show
         * calls the error handler error reporting function to display the error
         */
        void show();

    private:
        /**
         * the error handler used to show errors to the user
         */
        error m_error_handler;

        /**
         * the token with information needed to display complete and useful error messages
         * information carried by the token include:
         * - the file with the faulty construct
         * - the line where the error occured
         * - the column where the faulty construct occured (combined with the token lexeme length, we can show better messages)
         */
        token m_tok;
    };

    class interpreter {
    public:
        /**
         * the constructor expects:
         * - the global symbol table with programs to interpret
         * - the error handler to use in order to display errors
         */
        interpreter(gtable& gtab, error& error_handler);

        /**
         * interpret
         * the interpreter entry point
         */
        void interpret(std::vector<std::string>& arguments);

    private:
        /**
         * interpret_program
         * given the program that contains the main function and interpret the main function
         */
        void interpret_program(program& main_prog, std::vector<std::string>& arguments);

        /**
         * interpret_function
         * given a function declaration and a list of arguments, evaluate said function given said arguments
         * returns an expressions that is the result of the function evaluation; a null pointer if the function return type is void
         */
        std::shared_ptr<expr> interpret_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments);
        std::shared_ptr<expr> interpret_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

        /**
         * interpret_builtin_function
         * evaluates a builtin function by calling its local evaluate method passing it the given arguments
         */
        std::shared_ptr<expr> interpret_builtin_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments);
        std::shared_ptr<expr> interpret_builtin_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);

        /**
         * interpret_user_function
         * evaluates a user function by using this interpreter
         */
        std::shared_ptr<expr> interpret_user_function(std::shared_ptr<function>& function_decl, std::vector<std::shared_ptr<expr> >& arguments);

        /**
         * interpret_block
         * given a block statement, evaluate the declarations it contains
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_block(block_stmt& blc, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_variable
         * given a variable declaration, interpret the initializer expression and assign it the resulting value
         */
        void interpret_variable(std::shared_ptr<decl>& declaration, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_statement
         * given a statement declaration, interpret it depending on its type and return a pair with the result and a boolean indicating whether this statement was a return statement or not
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_statement(std::shared_ptr<decl>& declaration, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_while
         * given a while statemnt, we repeat ourselves like broken records
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_while(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_if
         * given an if statemnt, we make choices
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_if(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_break
         * given a break statemnt, set the break flag to true
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_break(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_continue
         * given a continue statemnt, set the break flag to true
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_continue(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_pass
         * given a pass statemnt, we do nothing
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_pass(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_return
         * given a return statemnt, return the expression it contains
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_return(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_expression
         * given an expression statement, compute its effective value and return it
         */
        std::pair<bool, std::shared_ptr<expr> > interpret_expression(std::shared_ptr<stmt>& a_statement, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_expression(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_tuple
         * given a tuple expression, compute the effective values of its elements, constructor a new tuple with the new elements and return it
         */
        std::shared_ptr<expr> interpret_tuple(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_list
         * given a list expression, compute the effective values of its elements, constructor a new list with the new elements and return it
         */
        std::shared_ptr<expr> interpret_list(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_map
         * given a map expression, compute the effective values of its elements, constructor a new map with the new elements and return it
         */
        std::shared_ptr<expr> interpret_map(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_call
         * given a call expression:
         * - if it is a default constructor, go over its arguments and compute their effective values, construct a call expression and return it
         * - if it is a record constructor, do the same as for record constructor
         * - if it is a function call, interpret the call and return the returned expression
         */
        std::shared_ptr<expr> interpret_call(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_identifier
         * if we have a:
         * - a variable, find it in the symbol table and return the value it contains
         * - a default constructor, return it
         */
        std::shared_ptr<expr> interpret_identifier(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_grouped
         * return the effective value of the expression contained in the grouped expression
         */
        std::shared_ptr<expr> interpret_grouped(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_cast
         * perform the function call corresponding to the cast expression and return the function call returned value
         */
        std::shared_ptr<expr> interpret_cast(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_unary
         * perform the function call corresponding to the unary operator and return the function call returned value
         */
        std::shared_ptr<expr> interpret_unary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_binary
         * perform the function call corresponding to the binary operator and return the function call returned value
         */
        std::shared_ptr<expr> interpret_binary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_functional_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_dot_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_subscript_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_namespace_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_variable_binary(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_variable_attribute(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_tuple_attribute(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_custom_attribute(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_variable_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_tuple_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_list_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_map_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_custom_subscript(std::shared_ptr<binary_expression> const & bin_expr, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_match
         * compare the lval to the rval and update the values of any variables if any captures were requested
         */
        std::shared_ptr<expr> interpret_match(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_literal_match(std::shared_ptr<expr>& lval_expr, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_identifier_match(std::shared_ptr<expr>& lval_expr, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        std::shared_ptr<expr> interpret_call_match(std::shared_ptr<expr>& lval_expr, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * interpret_assignment
         * get the effective value of the rval and assign it to the variable in the lval
         */
        std::shared_ptr<expr> interpret_assignment(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /*
         * the error handler to call in case of an error
         */
        error m_error_handler;

        /*
         * the global symbol table with programs to interpret
         */
        gtable m_gtable;

        /*
         * flag set to true if we are to continue to the next line
         */
        bool m_continue_loop;

        /*
         * flag set to true if we are to break out of the current loop
         */
        bool m_break_loop;

        /**
         * import_declarations
         * we imports immediate declarations into the given program scope from the imported scope
         */
        void import_declarations();
        void import_declarations(program& from, program& to);

        /**
         * interpretation_error
         * contructs and returns a interpret_error exception
         */
        interpret_error interpretation_error(const token& tok, const std::string& message);
    };
}

#endif
