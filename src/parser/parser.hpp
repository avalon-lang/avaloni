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

#ifndef AVALON_PARSER_HPP_
#define AVALON_PARSER_HPP_

#include <stdexcept>
#include <utility>
#include <memory>
#include <vector>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/program.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/ast/stmt/block_stmt.hpp"


namespace avalon {
    class parse_error : public std::runtime_error {
    public:
        /**
         * the contructor expects:
         * - the error handler to be used to show errors
         * - the token where the error occured
         * - whether the error is fatal or not
         * - the message to show the user
         */
        parse_error(error& error_handler, token tok, bool fatal, const std::string& message);

        /**
         * what
         * get the error message
         */
        virtual const char * what() const noexcept;

        /**
         * is_fatal
         * returns true if the error is fatal, false otherwise
         */
        bool is_fatal();

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

        /**
         * fatal is used decide whether parsing should terminate when an error occurs
         */
        bool m_fatal;
    };



    class parser {
    public:
        /**
         * the constructor expects:
         * - the tokens to walk over in order to build the AST
         * - the error handler to use in order to display errors
         */
        parser(std::vector<std::shared_ptr<token> >& tokens, const std::string& source_path, error& error_handler);

        /**
         * parse
         * the parser entry point
         * it constructs the program by calling the appropriate functions
         */
        program& parse();

    private:
        /**
         * prog
         * this function loops over the token stream until it reaches the end
         */
        program& prog();

        /**
         * declaration
         * a program is a series of declarations:
         * - import declaration
         * - namespace declaration
         *
         * this function decides which type of declaration we are dealing with
         * and calls the appropriate subroutine to parse that declaration.
         */
        void declaration(std::shared_ptr<scope>& parent_scope);

        /**
         * import_declaration
         * if the declaration function matches the "import" keyword, then this function
         * continues parsing in anticipation of a full import declaration.
         */
        void import_declaration();

        /**
         * namespace declaration
         * if the declaration function matches the "namespace" keyword, then this function
         * continues parsing in anticipation of type, function, variable and statement declarations
         */
        void namespace_declaration(std::shared_ptr<scope>& parent_scope);

        /**
         * top_declaration
         * types, functions and variables form the top declarations that can occur within a namespace
         * this function detect if we have any of them then calls the appropriate parsing functions
         */
        std::vector<std::shared_ptr<decl> > top_declaration(bool is_public, std::shared_ptr<scope>& parent_scope);

        /**
         * type_declaration
         * if the declaration function matches the "type" keyword, then this function
         * continues the parsing process in anticipation of a full type declaration.
         */
        std::shared_ptr<decl> type_declaration(bool is_public);

        /**
         * type_parameters
         * if a type depends of type parameters, this function identifies them,
         * makes sure they are unique within the type and they don't match existing types
         */
        void type_parameters(std::shared_ptr<type>& type_decl);

        /**
         * type_constructors
         * if a type has type constructors, this function identifies what kind of constructor they are
         * and calls the appropriate function to complete said constructors.
         */
        void type_constructors(std::shared_ptr<type>& type_decl);

        /**
         * parse_default_constructor
         * this function parses a default constructor.
         */
        void parse_default_constructor(std::shared_ptr<token>& constructor_tok, std::shared_ptr<type>& type_decl);

        /**
         * parse_record_constructor
         * this function parses a record constructor.
         */
        void parse_record_constructor(std::shared_ptr<token>& constructor_tok, std::shared_ptr<type>& type_decl);

        /**
         * parse_list_constructor
         * this function parses a list constructor.
         */
        void parse_list_constructor(std::shared_ptr<token>& constructor_tok, std::shared_ptr<type>& type_decl);

        /**
         * parse_map_constructor
         * this function parses a map constructor.
         */
        void parse_map_constructor(std::shared_ptr<token>& constructor_tok, std::shared_ptr<type>& type_decl);

        /**
         * function_declaration
         * if the declaration function matches the "function" keyword, then this function
         * continues the parsing process in anticipation of a full function declaration.
         */
        std::shared_ptr<decl> function_declaration(bool is_public, std::shared_ptr<scope>& parent_scope);

        /**
         * parse_parameters_constraints
         * parse the parameters a function depends on and add them to the function
         */
        void parse_parameters_constraints(std::shared_ptr<function>& function_decl);

        /**
         * parse_parameters
         * parse the function's parameters
         */
        void parse_parameters(std::shared_ptr<function>& function_decl);

        /**
         * variable_declaration
         * if the declaration function matches the "var" or "val" keywords, then this function
         * continues the parsing process in anticipation of a full variable declaration.
         */
        std::vector<std::shared_ptr<decl> > variable_declaration(bool is_public, std::shared_ptr<scope>& parent_scope);

        /**
         * statement_declaration
         * when we can't any of the existing declarations, then all we have is either a statement declaration
         * or a malformed expression.
         */
        std::shared_ptr<decl> statement_declaration(std::shared_ptr<scope>& parent_scope);

        /**
         * while_statement
         * match a while statement
         */
        std::shared_ptr<stmt> while_statement(std::shared_ptr<scope>& parent_scope);

        /**
         * parse_if
         * match an if statement
         */
        std::shared_ptr<stmt> if_statement(std::shared_ptr<scope>& parent_scope);

        /**
         * break_statement
         * match a break statement
         */
        std::shared_ptr<stmt> break_statement();

        /**
         * continue_statement
         * match a continue statement
         */
        std::shared_ptr<stmt> continue_statement();

        /**
         * pass_statement
         * match a pass statement
         */
        std::shared_ptr<stmt> pass_statement();

        /**
         * return_statement
         * match a return statement
         */
        std::shared_ptr<stmt> return_statement();

        /**
         * parse_block
         * parses a series a statement to form a block
         */
        block_stmt block_statement(std::shared_ptr<scope>& parent_scope);

        /**
         * expression_statement
         * macthes a expression followed by a new line
         */
        std::shared_ptr<stmt> expression_statement();

        /**
         * parse_expression
         * matches an expression
         */
        std::shared_ptr<expr> parse_expression();

        /**
         * assignment
         * matches an assignment expression
         * an assigment can be a construction assignment where a variable is given a value
         * or a deconstruction assignment where the content of a variable is split apart and
         * some of that content put into another variable.
         */
        std::shared_ptr<expr> assignment();

        /**
         * logical_or
         * matches a logical or expression
         */
        std::shared_ptr<expr> logical_or();

        /**
         * logical_and
         * matches a logical and expression
         */
        std::shared_ptr<expr> logical_and();

        /**
         * logical_not
         * matches a logical not expression
         */
        std::shared_ptr<expr> logical_not();

        /**
         * comparison
         * matches comparison expersions
         */
        std::shared_ptr<expr> comparison();

        /**
         * bitwise_or
         * matches a bitwise or expression
         */
        std::shared_ptr<expr> bitwise_or();

        /**
         * bitwise_xor
         * matches a bitwise xor expression
         */
        std::shared_ptr<expr> bitwise_xor();

        /**
         * bitwise_and
         * matches a bitwise and expression
         */
        std::shared_ptr<expr> bitwise_and();

        /**
         * bit_shift
         * matches bit shift expressions
         */
        std::shared_ptr<expr> bit_shift();

        /**
         * term
         * matches addition and substraction expressions
         */
        std::shared_ptr<expr> term();

        /**
         * factor
         * matches multiplication, division and modulus expressions
         */
        std::shared_ptr<expr> factor();

        /**
         * reference
         * matches a reference operator
         */
        std::shared_ptr<expr> reference();

        /**
         * dereference
         * matches a dereference operator
         */
        std::shared_ptr<expr> dereference();

        /**
         * cast
         * matches a type casting expression
         */
        std::shared_ptr<expr> cast();

        /**
         * sign
         * matches a sign inversion expression
         */
        std::shared_ptr<expr> sign();

        /**
         * bitwise_not
         * matches a bitwise not expression
         */
        std::shared_ptr<expr> bitwise_not();

        /**
         * dot
         * matches a dot expression
         */
        std::shared_ptr<expr> dot();

        /**
         * subscript
         * matches a subscript expression
         */
        std::shared_ptr<expr> subscript();

        /**
         * function_call
         * matches a function call expression
         */
        std::shared_ptr<expr> function_call();

        /**
         * grouped
         * matches a grouped expression
         */
        std::shared_ptr<expr> grouped();

        /**
         * primary
         * matches a primary expression
         */
        std::shared_ptr<expr> primary();

        /**
         * parse_call_expression
         * if an expression starts by identifier followed by an opening parenthesis
         * this function parses the remainder of the token stream until it reads a call expression.
         */
        std::shared_ptr<expr> parse_call_expression();

        /**
         * parse_tuple_expression
         * if we encounter an opening parenthesis followed by an expression then by a comma,
         * then we know we have a tuple instead of a grouped expression. this function parses the remainder of the tuple.
         */
        std::shared_ptr<expr> parse_tuple_expression(std::shared_ptr<token>& left_paren, token& first_token, std::shared_ptr<expr>& first_element);

        /**
         * parse_list_expression
         * if we encounter an opening bracket, we have a list
         */
        std::shared_ptr<expr> parse_list_expression(std::shared_ptr<token>& left_bracket);

        /**
         * parse_map_expression
         * if we encounter an opening brace, we have a map
         */
        std::shared_ptr<expr> parse_map_expression(std::shared_ptr<token>& left_brace);

        /**
         * parse_function_call_type_instance
         * parses the type instance to of a function call return type
         */
        type_instance parse_type_instance();

        /*
         * vector of token to parse
         */
        std::vector<std::shared_ptr<token> > m_tokens;

        /*
         * the source file currently being parsed
         */
        std::string m_source_path;

        /*
         * the error handler to call in case of an error
         */
        error& m_error_handler;

        /*
         * the current token being parsed
         */
        std::size_t m_current;

        /*
         * a program is the head of the AST
         * it contains a list of declarations that make up the program
         */
        program m_program;

        /*
         * the name of the current namespace that contains top declarations
         */
        std::string m_namespace;

        /*
         * the colon token can follow a primary expression to provide a type instance
         * but inside maps, it can follow a primary expression when used as a key value
         * to disambigue both, we require that type instance not be provided alongside expressions used as map keys
         */
        bool m_inside_map;

        /**
         * lookback
         * returns the token that comes before the current token being worked on
         */
        std::shared_ptr<token>& lookback();

        /**
         * peek
         * returns the token that comes after the current token being worked on
         */
        std::shared_ptr<token>& peek();

        /**
         * is_at_end
         * return true if we have reached the last token
         */
        bool is_at_end();

        /**
         * peek_next
         * return the token that comes after the token that comes after the current token
         */
        std::shared_ptr<token>& peek_next();

        /**
         * is_past_end
         * returns true if we are one token away from the end of the token stream
         */
        bool is_past_end();

        /**
         * check
         * given a token type, this function returns true if the current token is of the given token type
         */
        bool check(const token_type type);

        /**
         * check_previous
         * given a token type, this function returns true if the previous token is of the given token type
         */
        bool check_previous(const token_type type);

        /**
         * check_next
         * given a token type, this function returns true if the next token is of the given token type
         */
        bool check_next(const token_type type);

        /**
         * advance
         * moves to the next token
         */
        std::shared_ptr<token>& advance();

        /**
         * match
         * returns true if the current token is of the given token type
         * and moves to the next token
         */
        bool match(const token_type type);

        /**
         * consume
         * if the next token is of the given token type, return it and advance
         * else, raise an error.
         * all errors raise by this function are fatal.
         */
        std::shared_ptr<token>& consume(token_type type, const std::string& message);

        /**
         * parsing_error
         * contructs and returns a parse_error exception
         */
        parse_error parsing_error(bool fatal, std::shared_ptr<token>& tok, const std::string& message);
        parse_error parsing_error(bool fatal, const token& tok, const std::string& message);

        /**
         * synchronize
         * for non-fatal errors, instead of terminating the parsing process, we use this function
         * to decide where to resume parsing from.
         */
        void synchronize();
    };
}

#endif
