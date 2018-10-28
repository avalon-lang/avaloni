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

#include <sstream>
#include <memory>

/* Lexer */
#include "lexer/token.hpp"

/* AST */
/* Declarations */
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/import.hpp"
#include "representer/ast/decl/ns.hpp"
#include "representer/ast/program.hpp"
/* Expressions */
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Symbol table */
#include "representer/symtable/scope.hpp"
#include "representer/symtable/fqn.hpp"

/* Builtin types */
#include "representer/builtins/lang/avalon_float.hpp"

/* Builtin mathematics programs */
#include "representer/builtins/math/avalon_trig.hpp"


namespace avalon {
    /**
     * the default constructor expects nothing
     */
    avalon_trig::avalon_trig() {
    }

    /**
     * get_program
     * returns a program that defines trigonometric functions
     */
    program& avalon_trig::get_program() {
        // float type
        avalon_float avl_float;
        program& float_prog = avl_float.get_program();
        type_instance float_instance = avl_float.get_type_instance();

        /* the program FQN */
        fqn l_fqn("__bifqn_trig__", "__bifqn_trig__");
        m_trig_prog.set_fqn(l_fqn);
        m_trig_prog.is_builtin(true);

        /* the program's scope */
        std::shared_ptr<scope> l_scope = std::make_shared<scope>();
        l_scope -> set_start_line(0);
        l_scope -> set_end_line(0);
        m_trig_prog.set_scope(l_scope);

        /* import int and string programs into io */
        token import_tok(IDENTIFIER, "import", 0, 0, "__bid__");
        // import float program
        std::shared_ptr<import> float_import = std::make_shared<import>(import_tok, float_prog.get_fqn().get_name());
        std::shared_ptr<decl> final_float_import = float_import;
        m_trig_prog.add_declaration(final_float_import);

        /* create the namespace for the program */
        std::shared_ptr<ns> l_namespace = std::make_shared<ns>(star_tok);
        l_namespace -> set_fqn(l_fqn);

        /* add global variables declarations to the namespace */
        // variables
        // PI variable definition
        token pi_tok(IDENTIFIER, "__PI__", 0, 0, "__biv__");
        std::shared_ptr<variable> pi_variable = std::make_shared<variable>(pi_tok, false, VALID);
        pi_variable -> set_fqn(l_fqn);
        pi_variable -> set_namespace(l_namespace -> get_name());
        pi_variable -> set_scope(l_scope);
        pi_variable -> set_type_instance(float_instance);
        // set initializer value
        double PI = 3.14159265358979323846;
        std::ostringstream sstr;
        sstr.precision(20);
        sstr << PI;
        std::string pi_str = sstr.str();
        token lit_tok(FLOATING_POINT, pi_str, 0, 0, "__bil__");
        std::shared_ptr<literal_expression> pi_val = std::make_shared<literal_expression>(lit_tok, FLOATING_POINT_EXPR, pi_str);
        pi_val -> set_type_instance(float_instance);
        std::shared_ptr<expr> pi_expr = pi_val;
        pi_variable -> set_value(pi_expr);
        // add the variable declaration to the namespace
        std::shared_ptr<decl> pi_variable_decl = pi_variable;
        l_namespace -> add_declaration(pi_variable_decl);

        /* add the namespace to the program */
        std::shared_ptr<decl> namespace_decl = l_namespace;
        m_trig_prog.add_declaration(namespace_decl);

        return m_trig_prog;
    }
}
