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

#include <unordered_map>
#include <stdexcept>
#include <string>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/import.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/program.hpp"
#include "representer/ast/decl/ns.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_list.hpp"
#include "representer/builtins/lang/avalon_void.hpp"

/* Symbol table */
#include "representer/symtable/gtable.hpp"
#include "representer/symtable/scope.hpp"

/* FQN */
#include "representer/symtable/fqn.hpp"

/* Generator */
#include "checker/decl/function/function_generator.hpp"

/* Checkers */
#include "checker/decl/function/function_checker.hpp"
#include "checker/checker.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_can_collide.hpp"
#include "representer/exceptions/symbol_not_found.hpp"
#include "checker/exceptions/invalid_function.hpp"
#include "checker/exceptions/invalid_type.hpp"


namespace avalon {
/**
 * the contructor expects:
 * - the error handler to be used to show errors
 * - the token where the error occured
 * - the message to show the user
 */
check_error::check_error(error& error_handler, token tok, bool fatal, const std::string& message) : std::runtime_error(message), m_error_handler(error_handler), m_tok(tok), m_fatal(fatal) {
}
    
    /**
     * what
     * get the error message
     */
    const char * check_error::what() const noexcept {
        return std::runtime_error::what();
    }

    /**
     * is_fatal
     * returns true if the error is fatal, false otherwise
     */
    bool check_error::is_fatal() {
        return m_fatal;
    }

    /**
     * show
     * calls the error handler error reporting function to display the error
     */
    void check_error::show() {
        m_error_handler.log(m_tok . get_source_path(), m_tok . get_line(), m_tok . get_column(), what());
    }
    

/**
 * the constructor expects:
 * - the global symbol table with programs to check
 * - the path to the source containing the program we are validating
 * - the error handler to use in order to display errors
 */
checker::checker(program& prog, gtable& gtab, const std::string& source_path, error& error_handler) : m_source_path(source_path), m_error_handler(error_handler), m_prog(prog), m_gtable(gtab) {
}

    /**
     * check
     * the checker entry point
     * it ensure that all the main program contains the main function
     * then check all declarations that the main function depends on
     */
    void checker::check() {
        // void type instance
        avalon_void avl_void;
        type_instance& void_instance = avl_void.get_type_instance();

        // string type instance
        avalon_string avl_string;
        type_instance& string_instance = avl_string.get_type_instance();

        // list type instance
        avalon_list avl_list(string_instance);
        type_instance list_instance = avl_list.get_type_instance();

        // main function name, parameters and return type
        const std::string& function_name = "__main__";
        std::vector<type_instance> function_params = {list_instance};
        type_instance& ret_instance = void_instance;

        // scope and namespace where we want to find the main program
        std::shared_ptr<scope>& l_scope = m_prog.get_scope();
        const std::string& ns_name = "*";

        // find the main function and check it
        check_main(function_name, function_params, ret_instance, l_scope, ns_name);
    }

    /**
     * check_main
     * given the main function name, parameters' type instances and the return type,
     * find the function that matches the same and check said function under those assumptions
     */
    void checker::check_main(const std::string& name, std::vector<type_instance>& param_instances, type_instance& ret_instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        std::shared_ptr<function> fun = nullptr;
        std::vector<token> standins;

        // we try to find the function
        try {
            fun = find_function(name, param_instances, ret_instance, l_scope, ns_name, standins);
        } catch(symbol_not_found err) {
            throw checking_error(true, "The <__main__> function could not be found.");
        } catch(symbol_can_collide err) {
            throw checking_error(true, "The <__main__> function could not be found.");
        } catch(invalid_type err) {
            throw checking_error(false, err.get_token(), "The <__main__> function could not be found. Reason: " + std::string(err.what()));
        }

        // if we found the function, we proceed to check it taking into account the new type instances of the parameters
        // we get a copy of our function
        function new_fun(star_tok);
        new_fun.shallow_copy(* fun);
        std::vector<type_instance> constraint_instances;

        // using the parameters given and the return type instance, we update all constraints if any
        function_generator generator(new_fun, constraint_instances, l_scope, ns_name);
        try {
            generator.generate(param_instances, ret_instance);
        } catch(invalid_type err) {
            throw checking_error(false, err.get_token(), err.what());
        } catch(invalid_function err) {
            throw checking_error(false, err.get_token(), err.what());
        }

        // update the main function scope origin
        std::shared_ptr<scope> f_scope = new_fun.get_scope();
        f_scope -> set_origin(new_fun.get_name());

        // we make sure the function only parameter is a value parameter
        const std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = new_fun.get_params();
        const std::shared_ptr<variable>& param_var = params[0].second;
        if(param_var -> is_mutable())
            throw checking_error(true, param_var -> get_token(), "The only parameter to <__main__> must be a value parameter.");

        // we are here, it means we have a function that doesn't depend on constraints anymore, we perform proper semantic analysis
        function_checker f_checker;
        try {
            f_checker.check(new_fun, ns_name);
        } catch(invalid_function err) {
            throw checking_error(true, err.get_token(), err.what());
        }

        // add the specialization to the root function
        fun -> add_specialization(new_fun);
        fun -> is_used(true);
    }

    /**
     * checking_error
     * contructs and returns an check_error exception
     */
    check_error checker::checking_error(bool fatal, const std::string& message) {
        return check_error(m_error_handler, star_tok, fatal, message);
    }

    check_error checker::checking_error(bool fatal, const token& tok, const std::string& message) {
        return check_error(m_error_handler, tok, fatal, message);
    }
}
