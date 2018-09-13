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

#ifndef AVALON_CHECKER_HPP_
#define AVALON_CHECKER_HPP_

#include <stdexcept>
#include <string>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/program.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/type.hpp"

/* Symbol table */
#include "representer/symtable/scope.hpp"
#include "representer/symtable/gtable.hpp"


namespace avalon {
    class check_error : public std::runtime_error {
    public:
        /**
         * the contructor expects:
         * - the error handler to be used to show errors
         * - the token where the error occured
         * - the message to show the user
         */
        check_error(error& error_handler, token tok, bool fatal, const std::string& message);

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
         * fatal is used decide whether checking should terminate when an error occurs
         */
        bool m_fatal;
    };

    class checker {
    public:
        /**
         * the constructor expects:
         * - the global symbol table with programs to check
         * - the path to the source containing the program we are validating
         * - the error handler to use in order to display errors
         */
        checker(program& prog, gtable& gtab, const std::string& source_path, error& error_handler);

        /**
         * check
         * the checker entry point
         * it ensure that all the main program contains the main function
         * then check all declarations that the main function depends on
         */
        void check();

        /**
         * check_main
         * given the main function name, parameters' type instances and the return type,
         * find the function that matches the same and check said function under those assumptions
         */
        void check_main(const std::string& name, std::vector<type_instance>& param_instances, type_instance& ret_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

    private:
        /*
         * the source file currently being checked
         */
        std::string m_source_path;

        /*
         * the error handler to call in case of an error
         */
        error m_error_handler;

        /*
         * the program that contains the main function
         */
        program m_prog;

        /*
         * HIR global symbol table with all the programs to check
         */
        gtable m_gtable;

        /**
         * checking_error
         * contructs and returns a check_error exception
         */
        check_error checking_error(bool fatal, const std::string& message);
        check_error checking_error(bool fatal, const token& tok, const std::string& message);
    };
}

#endif
