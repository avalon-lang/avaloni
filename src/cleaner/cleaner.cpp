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
#include <memory>
#include <vector>
#include <string>

/* Error */
#include "error/error.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/statement.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/import.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/stmt/stmt.hpp"
#include "representer/symtable/fqn.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/program.hpp"

/* Symbol table */
#include "representer/symtable/gtable.hpp"

/* Cleaner */
#include "cleaner/cleaner.hpp"


namespace avalon {
/**
 * the contructor expects:
 * - the error handler to be used to show errors
 * - the token where the error occured
 * - the message to show the user
 */
clean_error::clean_error(error& error_handler, token tok, const std::string& message) : std::runtime_error(message), m_error_handler(error_handler), m_tok(tok) {
}
    
    /**
     * what
     * get the error message
     */
    const char * clean_error::what() const noexcept {
        return std::runtime_error::what();
    }

    /**
     * show
     * calls the error handler error reporting function to display the error
     */
    void clean_error::show() {
        m_error_handler.log(m_tok . get_source_path(), m_tok . get_line(), m_tok . get_column(), what());
    }

/**
 * the constructor expects:
 * - the global symbol table with programs to clean
 * - the error handler to use in order to display errors
 */
cleaner::cleaner(gtable& gtab, error& error_handler) : m_error_handler(error_handler), m_old_gtable(gtab), m_main_found(false) {
}

    /**
     * clean
     * the cleaner entry point
     * in the cleaner we inform the user of the unused declarations
     * returns a new global symbol table with new programs without parametrized types (on any variable or type constructors)
     */
    gtable& cleaner::clean() {
        std::unordered_map<std::string, program>& g_progs = m_old_gtable.get_programs();
        bool contains_main = false;

        for(auto& g_prog : g_progs) {
            program new_g_prog = clean_program(g_prog.first, g_prog.second, contains_main);
            m_new_gtable.add_program(new_g_prog);
            if(m_main_found == false && contains_main == true) {
                m_new_gtable.set_main_fqn(new_g_prog.get_fqn().get_name());
                m_main_found = true;
            }
        }

        return m_new_gtable;
    }

    /**
     * clean_program
     * given a program, make sure all declarations in it are used
     */
    program cleaner::clean_program(const std::string& fqn_name, program& prog, bool& contains_main) {
        std::vector<std::shared_ptr<decl> >& p_decls = prog.get_declarations();
        
        // set up the new program
        program new_prog;
        std::shared_ptr<scope>& old_prog_scope = prog.get_scope();
        std::shared_ptr<scope> new_prog_scope = std::make_shared<scope>(* old_prog_scope);
        new_prog.set_scope(new_prog_scope);
        new_prog.is_builtin(prog.is_builtin());
        fqn& old_prog_fqn = prog.get_fqn();
        new_prog.set_fqn(old_prog_fqn);

        // fill it with cleaned declarations
        for(auto& p_decl : p_decls) {
            if(p_decl -> is_import()) {
                std::shared_ptr<import> const & import_decl = std::static_pointer_cast<import>(p_decl);
                std::shared_ptr<decl> new_import_decl = clean_import(import_decl);
                new_prog.add_declaration(new_import_decl);
            }
            else if(p_decl -> is_namespace()) {
                std::shared_ptr<ns> const & ns_decl = std::static_pointer_cast<ns>(p_decl);
                std::shared_ptr<decl> new_ns_decl = clean_namespace(ns_decl, new_prog_scope, contains_main);
                new_prog.add_declaration(new_ns_decl);
            }
            else {
                throw std::runtime_error("[compiler error] unexpected declaration in program. The only two expected declarations that must happen in a program are import and namespace declarations.");
            }
        }
        return new_prog;
    }

    /**
     * clean_import
     * given an import declaration, make sure it is used
     * NOTE: right now, we can't actually know if declarations from an import are used but I'm considering adding that later
     */
    std::shared_ptr<decl> cleaner::clean_import(std::shared_ptr<import> const & import_decl) {
        std::shared_ptr<import> new_import_decl = std::make_shared<import>(* import_decl);
        std::shared_ptr<decl> old_import_decl = new_import_decl;
        return old_import_decl;
    }

    /**
     * clean_namespace
     * given a namespace declaration, clean all its content
     */
    std::shared_ptr<decl> cleaner::clean_namespace(std::shared_ptr<ns> const & ns_decl, std::shared_ptr<scope>& l_scope, bool& contains_main) {
        std::vector<std::shared_ptr<decl> >& n_decls = ns_decl -> get_declarations();
        for(auto& n_decl : n_decls) {
            bool is_main_function = clean_declaration(n_decl, true);
            if(is_main_function)
                contains_main = true;
        }
        std::shared_ptr<ns> new_ns_decl = std::make_shared<ns>(* ns_decl);
        new_ns_decl -> set_scope(l_scope);
        std::shared_ptr<decl> end_ns_decl = new_ns_decl;
        return end_ns_decl;
    }

    /**
     * clean_declaration
     * given a declaration, issue an error if it is not used
     * returns true if the declaration if the main function
     */
    bool cleaner::clean_declaration(std::shared_ptr<decl>& n_decl, bool is_global) {
        if(n_decl -> is_type()) {
            clean_type(n_decl);
        }
        else if(n_decl -> is_function()) {
            return clean_function(n_decl);
        }
        else if(n_decl -> is_variable()) {
            clean_variable(n_decl);
        }
        else if(n_decl -> is_statement()) {
            // we disallow the presence of statements in the global scope
            if(is_global) {
                std::shared_ptr<statement> const & s_decl = std::static_pointer_cast<statement>(n_decl);
                std::shared_ptr<stmt>& s_stmt = s_decl -> get_statement();
                throw cleaning_error(s_stmt -> stmt_token(), "Statements are not allowed as global declarations and will not be executed.");
            }
        }
        else {
            throw std::runtime_error("[compiler error] unexpected declaration inside namespace.");
        }

        return false;
    }

    /**
     * clean_type
     * issue an error if this type wasn't used anywhere
     */
    void cleaner::clean_type(std::shared_ptr<decl>& n_decl) {
        /*
        std::shared_ptr<type> const & type_decl = std::static_pointer_cast<type>(n_decl);
        if(type_decl -> is_used() == false) {
            clean_error err = cleaning_error(type_decl -> get_token(), "The type declaration <" + mangle_type(* type_decl) + "> is not used anywhere and may be removed.");
            err.show();
        }
        */
    }
    
    /**
     * clean_function
     * issue an error if this function wasn't used anywhere
     * returns true if the given function is the main function
     */
    bool cleaner::clean_function(std::shared_ptr<decl>& n_decl) {
        std::shared_ptr<function> const & function_decl = std::static_pointer_cast<function>(n_decl);

        /*
        // if the function is not used, we issue an error and leave
        if(function_decl -> is_used() == false) {
            clean_error err = cleaning_error(function_decl -> get_token(), "The function declaration <" + mangle_function(* function_decl) + "> is not used anywhere and may be removed.");
            err.show();
        }
        */

        // on the other hand, if it is used, we check all of its specializations body
        std::unordered_map<std::string, std::shared_ptr<function> >& specializations = function_decl -> get_specializations();
        for(auto& specialization : specializations) {
            /* clean_header(specialization.second); */
            clean_body(specialization.second);
        }

        if(function_decl -> get_name() == "__main__")
            return true;
        else
            return false;
    }

    void cleaner::clean_header(std::shared_ptr<function> const & specialization) {
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = specialization -> get_params();
        for(auto& param : params)
            clean_variable(param.second);
    }

    void cleaner::clean_body(std::shared_ptr<function> const & specialization) {
        block_stmt& body = specialization -> get_body();
        clean_block(body);
    }

    /**
     * clean_variable
     * issue an error if this variable wasn't used anywhere
     */
    void cleaner::clean_variable(std::shared_ptr<decl>& n_decl) {
        std::shared_ptr<variable> variable_decl = std::static_pointer_cast<variable>(n_decl);
        clean_variable(variable_decl);
    }

    void cleaner::clean_variable(std::shared_ptr<variable>& variable_decl) {
        if(variable_decl -> is_used() == false && variable_decl -> is_global() ==  false) {
            clean_error err = cleaning_error(variable_decl -> get_token(), "The variable declaration <" + mangle_variable(* variable_decl) + "> is not used anywhere and may be removed.");
            err.show();
        }
    }

    /**
     * clean_block
     * make sure all declarations inside a block are used
     */
    void cleaner::clean_block(block_stmt& blc) {
        std::vector<std::shared_ptr<decl> >& b_decls = blc.get_declarations();
        for(auto& b_decl : b_decls)
            clean_declaration(b_decl, false);
    }

    /**
     * cleaning_error
     * contructs and returns a clean_error exception
     */
    clean_error cleaner::cleaning_error(const token& tok, const std::string& message) {
        return clean_error(m_error_handler, tok, message);
    }
}
