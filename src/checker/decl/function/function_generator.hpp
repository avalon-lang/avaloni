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

#ifndef AVALON_CHECKER_DECL_FUNCTION_GENERATOR_HPP_
#define AVALON_CHECKER_DECL_FUNCTION_GENERATOR_HPP_

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/decl/decl.hpp"
#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/expr/expr.hpp"
#include "lexer/token.hpp"


namespace avalon {
    class function_generator {
    public:
        /**
         * the default constructor expects nothing
         */
        function_generator(function& fun_decl, std::vector<type_instance>& constraint_instances, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

        /**
         * generate
         * given a function declaration, a vector parameters' type instances and the return type instance,
         * this function generates a function were all constraints have been replaced by the appropriate type instances
         */
        void generate(std::vector<type_instance>& param_instances, type_instance& ret_instance);

    private:
        /*
         * the function for which to generate type instances
         */
        function& m_fun_decl;

        /*
         * a vector of constraint instances that is supplied by the user
         */
        std::vector<type_instance> m_instances;

        /*
         * we map the name of constraints to the first type instance that match them
         * this is useful to prevent users from mapping multiple type instances to the same constraint
         */
        std::unordered_map<std::string, type_instance> m_constraint_instances;

        /*
         * the scope where to look for types shoud we need them
         */
        std::shared_ptr<scope>& m_scope;

        /*
         * the current namespace where to find types we need
         */
        std::string m_namespace;

        /**
         * generate_params
         * generates a new set of the function parameters replacing the constraints with the given type instances
         */
        void generate_params(std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params, std::vector<type_instance>& param_instances);

        /**
         * generate_return
         * generates a new return type instance replacing the constraints with the given type instances
         */
        void generate_return(type_instance& fun_ret_instance, type_instance& new_ret_instance);

        /**
         * generate_body
         * generates a new return type instance replacing the constraints with the given type instances
         */
        void generate_body(block_stmt& body);

        /**
         * generate_variable
         * generate new type instances in a variable declaration, replacing constraints with supplied type instances
         */
        void generate_variable(std::shared_ptr<decl>& declaration);

        /**
         * generate_statement
         * generate new type instances in a statement declaration, replacing constraints with supplied type instances
         */
        void generate_statement(std::shared_ptr<decl>& declaration);
        void generate_while(std::shared_ptr<stmt>& a_statement);
        void generate_if(std::shared_ptr<stmt>& a_statement);
        void generate_return(std::shared_ptr<stmt>& a_statement);
        void generate_block(std::shared_ptr<stmt>& a_statement);
        void generate_block(block_stmt& b_stmt);
        void generate_expression(std::shared_ptr<stmt>& a_statement);

        /**
         * generate_expression
         * generate a new type instance attached to an expression, replacing constraints with supplied type instances
         */
        void generate_expression(std::shared_ptr<expr>& an_expression);
        void generate_tuple(std::shared_ptr<expr>& an_expression);
        void generate_list(std::shared_ptr<expr>& an_expression);
        void generate_map(std::shared_ptr<expr>& an_expression);
        void generate_call(std::shared_ptr<expr>& an_expression);
        void generate_identifier(std::shared_ptr<expr>& an_expression);
        void generate_cast(std::shared_ptr<expr>& an_expression);
        void generate_match(std::shared_ptr<expr>& an_expression);
        void generate_grouped(std::shared_ptr<expr>& an_expression);
        void generate_binary(std::shared_ptr<expr>& an_expression);
        void generate_unary(std::shared_ptr<expr>& an_expression);
        void generate_conditional(std::shared_ptr<expr>& an_expression);
        void generate_assignment(std::shared_ptr<expr>& an_expression);



        /**
         * build_instance
         * replaces the abstract type instances in dest with concrete type instances in orig
         * then update the constraints map above to indicate that we found a match
         */
        void build_instance(type_instance& dest, type_instance& orig);
    };
}

#endif
