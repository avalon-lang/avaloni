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
#include <utility>
#include <memory>
#include <vector>

/* Error */
#include "error/error.hpp"

/* AST */
#include "representer/ast/decl/import.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/program.hpp"

/* Built-in types */
#include "representer/builtins/lang/avalon_string.hpp"
#include "representer/builtins/lang/avalon_qubit8.hpp"
#include "representer/builtins/lang/avalon_qubit4.hpp"
#include "representer/builtins/lang/avalon_qubit2.hpp"
#include "representer/builtins/lang/avalon_qubit.hpp"
#include "representer/builtins/lang/avalon_maybe.hpp"
#include "representer/builtins/lang/avalon_float.hpp"
#include "representer/builtins/lang/avalon_gate.hpp"
#include "representer/builtins/lang/avalon_void.hpp"
#include "representer/builtins/lang/avalon_bool.hpp"
#include "representer/builtins/lang/avalon_bit2.hpp"
#include "representer/builtins/lang/avalon_bit4.hpp"
#include "representer/builtins/lang/avalon_bit8.hpp"
#include "representer/builtins/lang/avalon_int.hpp"
#include "representer/builtins/lang/avalon_bit.hpp"

/* Builtin OS interface programs */
#include "representer/builtins/io/avalon_io.hpp"

/* Builtin mathematical functions */
#include "representer/builtins/math/avalon_trig.hpp"

/* Symbol table */
#include "representer/symtable/gtable.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/symtable/fqn.hpp"

/* Checker */
#include "checker/decl/function/header_checker.hpp"

/* Parser */
#include "parser/parser.hpp"

/* Lexer */
#include "lexer/token.hpp"
#include "lexer/lexer.hpp"

/* Utilities */
#include "utils/parse_util.hpp"

/* Importer */
#include "importer/importer.hpp"

/* Exceptions */
#include "representer/exceptions/symbol_already_declared.hpp"
#include "checker/exceptions/invalid_function.hpp"
#include "utils/exceptions/invalid_directory.hpp"
#include "utils/exceptions/file_not_found.hpp"


namespace avalon {
/**
 * the contructor expects:
 * - the error handler to be used to show errors
 * - the token where the error occured
 * - whether the error is fatal or not
 * - the message to show the user
 */
import_error::import_error(error& error_handler, token tok, bool fatal, const std::string& message) : std::runtime_error(message), m_error_handler(error_handler), m_tok(tok), m_fatal(fatal) {
}
    
    /**
     * what
     * get the error message
     */
    const char * import_error::what() const noexcept {
        return std::runtime_error::what();
    }

    /**
     * is_fatal
     * returns true if the error is fatal, false otherwise
     */
    bool import_error::is_fatal() {
        return m_fatal;
    }

    /**
     * show
     * calls the error handler error reporting function to display the error
     */
    void import_error::show() {
        m_error_handler.log(m_tok . get_source_path(), m_tok . get_line(), m_tok . get_column(), what());
    }


/**
 * the constructor expects the main program that's root to all dependencies and the compiler to compile those dependencies
 */
importer::importer(program& prog, std::vector<std::string>& search_paths, error& error_handler) : m_program(prog), m_search_paths(search_paths), m_error_handler(error_handler) {
}

    /**
     * import_all
     * the importer entry point
     * it constructs the gtable with all the imported files parsed
     */
    gtable& importer::import_all() {
        // we generate all the dependencies that we will ultimately build the program
        generate_deps(m_program);

        // we sort the dependencies, making sure no cycles are allowed among them
        sort_deps();

        // we import public declarations from one program to another
        run_imports();

        // last, we return the gtable
        return m_gtable;
    }

    /**
     * generate_deps
     * given a program, this function gets all the program imports,
     * validates the imports and generates the dependency map.
     */
    void importer::generate_deps(program& prog) {
        std::string prog_fqn_name = prog.get_fqn().get_name();

        // if the program already exists in the global symbol table, we return
        if(m_gtable.program_exists(prog_fqn_name))
            return;
        else
            m_gtable.add_program(prog);

        // add the program fqn to the dependency graph
        std::vector<std::string> deps;
        m_deps.emplace(prog_fqn_name, deps);

        // add the program fqn to the dependency visitation map
        m_dep_states.emplace(prog_fqn_name, NOT_VISITED);

        // go over the declarations looking for import declarations
        std::vector<std::shared_ptr<decl> >& decls = prog.get_declarations();
        for(auto& decl : decls) {
            // if the declaration is not an import declaration, we go to the next
            if(decl -> is_import() == false)
                continue;

            std::shared_ptr<import> import_decl = std::static_pointer_cast<import>(decl);
            const std::string& import_fqn_name = import_decl -> get_fqn_name();
            fqn import_fqn = fqn::from_name(import_fqn_name);

            // now, we do the actual work of importing dependencies
            // first, we parse the file introduced by the dependency
            program import_prog;
            try {
                import_prog = parse_util::parse(import_fqn.get_path(), m_search_paths);
            } catch(file_not_found err) {
                throw importing_error(true, import_decl -> get_token(), "Failed to import <" + import_fqn_name + ">. Does the file associated with it exist?");
            } catch(lex_error err) {
                throw err;
            } catch(parse_error err) {
                throw err;
            }

            // then, if the parsing was successful, we add the dependency to the depedency graph
            m_deps.at(prog_fqn_name).push_back(import_fqn_name);
            generate_deps(import_prog);
        }
    }

    /**
     * sort_deps
     * sort the dependency graph that makes up the program,
     * making sure no cycles are allowed among dependencies
     */
    void importer::sort_deps() {
        // first we add builtin programs
        add_builtin_programs();

        // go over all the the dependencies and sort them
        for(auto& dep : m_deps)
            if(m_dep_states.at(dep.first) == NOT_VISITED)
                sort_deps_util(dep.first, dep.second);
    }

    /**
     * add_builtin_programs
     * adds builtin programs to the dependency queue
     */
    void importer::add_builtin_programs() {
        // string declarations
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        m_sorted_deps.push(string_prog.get_fqn().get_name());
        m_gtable.add_program(string_prog);

        // maybe declarations
        avalon_maybe avl_maybe;
        program& maybe_prog = avl_maybe.get_program();
        m_sorted_deps.push(maybe_prog.get_fqn().get_name());
        m_gtable.add_program(maybe_prog);

        // void declarations
        avalon_void avl_void;
        program& void_prog = avl_void.get_program();
        m_sorted_deps.push(void_prog.get_fqn().get_name());
        m_gtable.add_program(void_prog);

        // bool declarations
        avalon_bool avl_bool;
        program& bool_prog = avl_bool.get_program();
        m_sorted_deps.push(bool_prog.get_fqn().get_name());
        m_gtable.add_program(bool_prog);

        // floating point declarations
        avalon_float avl_float;
        program& float_prog = avl_float.get_program();
        m_sorted_deps.push(float_prog.get_fqn().get_name());
        m_gtable.add_program(float_prog);

        // gate declarations
        avalon_gate avl_gate;
        program& gate_prog = avl_gate.get_program();
        m_sorted_deps.push(gate_prog.get_fqn().get_name());
        m_gtable.add_program(gate_prog);

        // trigonometry declarations
        avalon_trig avl_trig;
        program& trig_prog = avl_trig.get_program();
        m_sorted_deps.push(trig_prog.get_fqn().get_name());
        m_gtable.add_program(trig_prog);

        // bit declarations
        avalon_bit avl_bit;
        program& bit_prog = avl_bit.get_program();
        m_sorted_deps.push(bit_prog.get_fqn().get_name());
        m_gtable.add_program(bit_prog);

        // bit2 declarations
        avalon_bit2 avl_bit2;
        program& bit2_prog = avl_bit2.get_program();
        m_sorted_deps.push(bit2_prog.get_fqn().get_name());
        m_gtable.add_program(bit2_prog);

        // bit4 declarations
        avalon_bit4 avl_bit4;
        program& bit4_prog = avl_bit4.get_program();
        m_sorted_deps.push(bit4_prog.get_fqn().get_name());
        m_gtable.add_program(bit4_prog);

        // bit8 declarations
        avalon_bit8 avl_bit8;
        program& bit8_prog = avl_bit8.get_program();
        m_sorted_deps.push(bit8_prog.get_fqn().get_name());
        m_gtable.add_program(bit8_prog);

        // qubit declarations
        avalon_qubit avl_qubit;
        program& qubit_prog = avl_qubit.get_program();
        m_sorted_deps.push(qubit_prog.get_fqn().get_name());
        m_gtable.add_program(qubit_prog);

        // qubit2 declarations
        avalon_qubit2 avl_qubit2;
        program& qubit2_prog = avl_qubit2.get_program();
        m_sorted_deps.push(qubit2_prog.get_fqn().get_name());
        m_gtable.add_program(qubit2_prog);

        // qubit4 declarations
        avalon_qubit4 avl_qubit4;
        program& qubit4_prog = avl_qubit4.get_program();
        m_sorted_deps.push(qubit4_prog.get_fqn().get_name());
        m_gtable.add_program(qubit4_prog);

        // qubit8 declarations
        avalon_qubit8 avl_qubit8;
        program& qubit8_prog = avl_qubit8.get_program();
        m_sorted_deps.push(qubit8_prog.get_fqn().get_name());
        m_gtable.add_program(qubit8_prog);

        // integer declarations
        avalon_int avl_int;
        program& int_prog = avl_int.get_program();
        m_sorted_deps.push(int_prog.get_fqn().get_name());
        m_gtable.add_program(int_prog);

        // IO declarations
        avalon_io avl_io;
        program& io_prog = avl_io.get_program();
        m_sorted_deps.push(io_prog.get_fqn().get_name());
        m_gtable.add_program(io_prog);
    }

    /**
     * sort_deps_util
     * perform a topological sort of the dependency graph.
     *
     * REMARK: Initially, I thought about using the algorithm provided by boost::graph but I could not understand the thing.
     * When I do, I might just use it.
     */
    void importer::sort_deps_util(const std::string& dep, std::vector<std::string>& sub_deps) {
        // mark the current dependency as being visited
        m_dep_states[dep] = VISITING;

        // go over the current dependency sub-dependencies and sort them
        for(auto& sub_dep : sub_deps) {
            // if not visited, we carry on
            if(m_dep_states.at(sub_dep) == NOT_VISITED)
                sort_deps_util(sub_dep, m_deps.at(sub_dep));
            // if currently visiting, then we have a cycle
            else if(m_dep_states.at(sub_dep) == VISITING)
                throw std::runtime_error("There is a cycle between importer <" + dep + "> and importee <" + sub_dep + ">.");
        }

        // we add our dependency to the sort list(queue)
        m_sorted_deps.push(dep);

        // mark the current dependency as visited
        m_dep_states[dep] = VISITED;
    }

    /**
     * run_imports
     * performs imports of declarations from one program to another
     */
    void importer::run_imports() {
        // we then add user defined declarations
        while(!m_sorted_deps.empty()) {
            const std::string& fqn_name = m_sorted_deps.front();
            program& prog = m_gtable.get_program(fqn_name);
            run_imports_util(prog);
            m_sorted_deps.pop();
        }
    }

    /**
     * run_imports_util
     * goes through all top declarations in a program, finds import declarations and imports those declarations into the current program's scope
     */
    void importer::run_imports_util(program& prog) {
        // 1. we add builtin programs to this program as import statements
        if(prog.is_builtin() == false)
            run_builtin_imports(prog);

        // 2. we then import declarations from import statements into the current program
        std::vector<std::shared_ptr<decl> >& declarations = prog.get_declarations();
        for(auto& declaration : declarations) {
            if(declaration -> is_import()) {
                const std::shared_ptr<import>& import_decl = std::static_pointer_cast<import>(declaration);
                program& imported_prog = m_gtable.get_program(import_decl -> get_fqn_name());
                import_declarations(imported_prog, prog, false);
            }
        }

        // 3. we last perform a self-import (essentally we add defined declarations from the current program into its scope)
        if(prog.is_builtin() == false)
            import_declarations(prog, prog, true);
    }

    /**
     * run_builtin_imports
     * imports built in declarations into the given program
     */
    void importer::run_builtin_imports(program& to) {
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");

        // maybe declarations
        avalon_maybe avl_maybe;
        program& maybe_prog = avl_maybe.get_program();
        std::shared_ptr<import> maybe_import = std::make_shared<import>(import_tok, maybe_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_maybe_import = maybe_import;
        to.add_declaration(final_maybe_import);

        // qubit declarations
        avalon_qubit avl_qubit;
        program& qubit_prog = avl_qubit.get_program();
        std::shared_ptr<import> qubit_import = std::make_shared<import>(import_tok, qubit_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_qubit_import = qubit_import;
        to.add_declaration(final_qubit_import);

        // qubit2 declarations
        avalon_qubit2 avl_qubit2;
        program& qubit2_prog = avl_qubit2.get_program();
        std::shared_ptr<import> qubit2_import = std::make_shared<import>(import_tok, qubit2_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_qubit2_import = qubit2_import;
        to.add_declaration(final_qubit2_import);

        // qubit4 declarations
        avalon_qubit4 avl_qubit4;
        program& qubit4_prog = avl_qubit4.get_program();
        std::shared_ptr<import> qubit4_import = std::make_shared<import>(import_tok, qubit4_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_qubit4_import = qubit4_import;
        to.add_declaration(final_qubit4_import);

        // qubit8 declarations
        avalon_qubit8 avl_qubit8;
        program& qubit8_prog = avl_qubit8.get_program();
        std::shared_ptr<import> qubit8_import = std::make_shared<import>(import_tok, qubit8_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_qubit8_import = qubit8_import;
        to.add_declaration(final_qubit8_import);

        // gate declarations
        avalon_gate avl_gate;
        program& gate_prog = avl_gate.get_program();
        std::shared_ptr<import> gate_import = std::make_shared<import>(import_tok, gate_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_gate_import = gate_import;
        to.add_declaration(final_gate_import);

        // trigonometry declarations
        avalon_trig avl_trig;
        program& trig_prog = avl_trig.get_program();
        std::shared_ptr<import> trig_import = std::make_shared<import>(import_tok, trig_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_trig_import = trig_import;
        to.add_declaration(final_trig_import);

        // void declarations
        avalon_void avl_void;
        program& void_prog = avl_void.get_program();
        std::shared_ptr<import> void_import = std::make_shared<import>(import_tok,void_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_void_import = void_import;
        to.add_declaration(final_void_import);

        // bit declarations
        avalon_bit avl_bit;
        program& bit_prog = avl_bit.get_program();
        std::shared_ptr<import> bit_import = std::make_shared<import>(import_tok, bit_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bit_import = bit_import;
        to.add_declaration(final_bit_import);

        // bit2 declarations
        avalon_bit2 avl_bit2;
        program& bit2_prog = avl_bit2.get_program();
        std::shared_ptr<import> bit2_import = std::make_shared<import>(import_tok, bit2_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bit2_import = bit2_import;
        to.add_declaration(final_bit2_import);

        // bit4 declarations
        avalon_bit4 avl_bit4;
        program& bit4_prog = avl_bit4.get_program();
        std::shared_ptr<import> bit4_import = std::make_shared<import>(import_tok, bit4_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bit4_import = bit4_import;
        to.add_declaration(final_bit4_import);

        // bit8 declarations
        avalon_bit8 avl_bit8;
        program& bit8_prog = avl_bit8.get_program();
        std::shared_ptr<import> bit8_import = std::make_shared<import>(import_tok, bit8_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bit8_import = bit8_import;
        to.add_declaration(final_bit8_import);

        // bool declarations
        avalon_bool avl_bool;
        program& bool_prog = avl_bool.get_program();
        std::shared_ptr<import> bool_import = std::make_shared<import>(import_tok, bool_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_bool_import = bool_import;
        to.add_declaration(final_bool_import);

        // floating point declarations
        avalon_float avl_float;
        program& float_prog = avl_float.get_program();
        std::shared_ptr<import> float_import = std::make_shared<import>(import_tok, float_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_float_import = float_import;
        to.add_declaration(final_float_import);

        // IO declarations
        avalon_io avl_io;
        program& io_prog = avl_io.get_program();
        std::shared_ptr<import> io_import = std::make_shared<import>(import_tok, io_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_io_import = io_import;
        to.add_declaration(final_io_import);

        // integer declarations
        avalon_int avl_int;
        program& int_prog = avl_int.get_program();
        std::shared_ptr<import> int_import = std::make_shared<import>(import_tok, int_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_int_import = int_import;
        to.add_declaration(final_int_import);

        // string declarations
        avalon_string avl_string;
        program& string_prog = avl_string.get_program();
        std::shared_ptr<import> string_import = std::make_shared<import>(import_tok, string_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_string_import = string_import;
        to.add_declaration(final_string_import);
    }

    /**
     * import_declarations
     * given two programs, import all the declarations in "from" program into "to" program's scope
     */
    void importer::import_declarations(program& from, program& to, bool include_privates) {
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
                if(l_declaration -> is_type()) {
                    std::shared_ptr<type> type_decl = std::static_pointer_cast<type>(l_declaration);
                    // if "include_privates" is true, it doesn't matter whether the declaration is public, we import it
                    // if "include_privates" is false, we only import the declaration if it is public
                    if(include_privates || type_decl -> is_public()) {
                        import_type(type_decl, to_scope, namespace_decl -> get_name());
                    }
                }
                else if(l_declaration -> is_function()) {
                    std::shared_ptr<function> function_decl = std::static_pointer_cast<function>(l_declaration);
                    // if "include_privates" is true, it doesn't matter whether the declaration is public, we import it
                    // if "include_privates" is false, we only import the declaration if it is public
                    if(include_privates || function_decl -> is_public()) {
                        try {
                            std::shared_ptr<scope>& f_scope = function_decl -> get_scope();
                            const std::string& ns_name = function_decl -> get_namespace();
                            header_checker::prepare_header(* function_decl, f_scope, ns_name);
                        } catch(invalid_function err) {
                            throw importing_error(true, err.get_token(), err.what());
                        }
                        import_function(function_decl, to_scope, namespace_decl -> get_name());
                    }
                }
                else if(l_declaration -> is_variable()) {
                    std::shared_ptr<variable> variable_decl = std::static_pointer_cast<variable>(l_declaration);
                    // if "include_privates" is true, it doesn't matter whether the declaration is public, we import it
                    // if "include_privates" is false, we only import the declaration if it is public
                    if(include_privates || variable_decl -> is_public()) {
                        import_variable(variable_decl, to_scope, namespace_decl -> get_name());
                    }
                }
            }
        }
    }

    /**
     * import_type
     * given a namespace name and a type declaration, insert the type into the given scope
     */
    void importer::import_type(std::shared_ptr<type>& type_decl, std::shared_ptr<scope>& scp, const std::string& namespace_name) {
        try {
            scp -> add_type(namespace_name, type_decl);
        } catch(symbol_already_declared err) {
            throw importing_error(true, type_decl -> get_token(), err.what());
        }
    }

    /**
     * import_function
     * given a namespace name and a function declaration, insert the function into the given scope
     */
    void importer::import_function(std::shared_ptr<function>& function_decl, std::shared_ptr<scope>& scp, const std::string& namespace_name) {
        try {
            scp -> add_function(namespace_name, function_decl);
        } catch(symbol_already_declared err) {
            throw importing_error(true, function_decl -> get_token(), err.what());
        }
    }

    /**
     * import_variable
     * given a namespace name and a variable declaration, insert the variable into the given scope
     */
    void importer::import_variable(std::shared_ptr<variable>& variable_decl, std::shared_ptr<scope>& scp, const std::string& namespace_name) {
        try {
            scp -> add_variable(namespace_name, variable_decl);
        } catch(symbol_already_declared err) {
            throw importing_error(true, variable_decl -> get_token(), err.what());
        }
    }

    /**
     * importing_error
     * contructs and returns an import_error exception
     */
    import_error importer::importing_error(bool fatal, const token& tok, const std::string& message) {
        return import_error(m_error_handler, tok, fatal, message);
    }
}
