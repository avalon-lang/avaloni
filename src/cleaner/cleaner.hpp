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

#ifndef AVALON_CLEANER_HPP_
#define AVALON_CLEANER_HPP_

#include <stdexcept>
#include <string>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/import.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/program.hpp"

/* Symbol table */
#include "representer/symtable/gtable.hpp"


namespace avalon {
    class clean_error : public std::runtime_error {
    public:
        /**
         * the contructor expects:
         * - the error handler to be used to show errors
         * - the token where the error occured
         * - the message to show the user
         */
        clean_error(error& error_handler, token tok, const std::string& message);

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


    class cleaner {
    public:
        /**
         * the constructor expects:
         * - the global symbol table with programs to clean
         * - the error handler to use in order to display errors
         */
        cleaner(gtable& gtab, error& error_handler);

        /**
         * clean
         * the cleaner entry point
         * in the cleaner we inform the user of the unused declarations
         * returns a new global symbol table with new programs without parametrized types (on any variable or type constructors)
         */
        gtable&  clean();

    private:
        /**
         * clean_program
         * given a program, make sure all declarations in it are used
         */
        program clean_program(const std::string& fqn_name, program& prog, bool& contains_main);

        /**
         * clean_import
         * given an import declaration, make sure it is used
         * NOTE: right now, we can't actually know if declarations from an import are used but I'm considering adding that later
         */
        std::shared_ptr<decl> clean_import(std::shared_ptr<import> const & import_decl);

        /**
         * clean_namespace
         * given a namespace declaration, clean all its content
         */
        std::shared_ptr<decl> clean_namespace(std::shared_ptr<ns> const & ns_decl, std::shared_ptr<scope>& l_scope, bool& contains_main);

        /**
         * clean_declaration
         * given a declaration, issue an error if it is not used
         * returns true if the declaration if the main function
         */
        bool clean_declaration(std::shared_ptr<decl>& n_decl, bool is_global);

        /**
         * clean_type
         * issue an error if this type wasn't used anywhere
         */
        void clean_type(std::shared_ptr<decl>& n_decl);
        
        /**
         * clean_function
         * issue an error if this function wasn't used anywhere
         * returns true if the given function is the main function
         */
        bool clean_function(std::shared_ptr<decl>& n_decl);
        void clean_header(std::shared_ptr<function> const & specialization);
        void clean_body(std::shared_ptr<function> const & specialization);

        /**
         * clean_variable
         * issue an error if this variable wasn't used anywhere
         */
        void clean_variable(std::shared_ptr<decl>& n_decl);
        void clean_variable(std::shared_ptr<variable>& variable_decl);

        /**
         * clean_block
         * make sure all declarations inside a block are used
         */
        void clean_block(block_stmt& blc);

        /*
         * the error handler to call in case of an error
         */
        error m_error_handler;

        /*
         * the old global symbol table with all the programs to clean
         */
        gtable m_old_gtable;

        /*
         * the new global symbol table with cleaned programs
         */
        gtable m_new_gtable;

        /*
         * true if the program with the main function has been found
         */
        bool m_main_found;

        /**
         * cleaning_error
         * contructs and returns a clean_error exception
         */
        clean_error cleaning_error(const token& tok, const std::string& message);
    };
}

#endif
