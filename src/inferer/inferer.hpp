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

#ifndef AVALON_INFERER_HPP_
#define AVALON_INFERER_HPP_

#include <memory>
#include <string>

#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/binary_expression.hpp"
#include "representer/ast/expr/unary_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/cast_expression.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"
#include "representer/ast/expr/expr.hpp"
#include "representer/ast/decl/type.hpp"


namespace avalon {
    class inferer {
    public:
        /**
         * the default constructor expects nothing
         */
        inferer();

        /**
         * infer
         * given an expression, this function infers the type instance of said expression and returns it
         */
        type_instance infer(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_default_constructor
         * infers the type instance of a default constructor expression
         */
        type_instance infer_default_constructor(std::shared_ptr<call_expression> const & call_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_record_constructor
         * infers the type instance of a record constructor expression
         */
        type_instance infer_record_constructor(std::shared_ptr<call_expression> const & call_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_function_call
         * infers the type instance of a function call expression
         */
        type_instance infer_function_call(function& new_fun, std::shared_ptr<call_expression> const & call_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_variable
         * infers the type instance of a variable expression
         */
        type_instance infer_variable(std::shared_ptr<identifier_expression> const & id_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_constructor
         * infers the type instance of an identifier constructor expression
         */
        type_instance infer_constructor(std::shared_ptr<identifier_expression> const & id_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_cast
         * infers the type instance of a cast expression
         */
        type_instance infer_cast(function& cast_fun, std::shared_ptr<cast_expression> const & cast_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_unary
         * infers the type instance of a unary expression
         */
        type_instance infer_unary(function& unary_fun, std::shared_ptr<unary_expression> const & unary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_unary
         * infers the type instance of a binary expression
         */
        type_instance infer_binary(function& binary_fun, std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_functional_binary
         * infers the type instance of a binary expression that immediately decays into a function call
         */
        type_instance infer_functional_binary(binary_expression_type& expr_type, function& binary_fun, std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_getattr_binary
         * infers the type instance of a binary expression arising from the dot operator
         */
        type_instance infer_dot_binary(std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_subscript_binary
         * infers the type instance of a binary expression arising from the subscript operator
         */
        type_instance infer_subscript_binary(std::shared_ptr<binary_expression> const & binary_expr, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        type_instance infer_namespace_binary(const std::string& sub_ns_name, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_variable_binary(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_variable_attribute(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_tuple_attribute(std::shared_ptr<expr>& lval_val, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_custom_attribute(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_variable_subscript(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_tuple_subscript(type_instance& var_instance, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_list_subscript(type_instance& var_instance, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_map_subscript(type_instance& var_instance, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        type_instance infer_custom_subscript(std::shared_ptr<expr>& lval, std::shared_ptr<expr>& rval, std::shared_ptr<scope>& l_scope, const std::string& ns_name);

    private:
        /**
         * infer_underscore
         * infers the type instance of an underscopre expression, which is just a type instance without any type
         */
        type_instance infer_underscore(std::shared_ptr<expr>& an_expression);

        /**
         * infer_reference
         * infers the type instance of a reference
         */
        type_instance infer_reference(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_reference
         * infers the type instance of a dereference
         */
        type_instance infer_dereference(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_literal
         * infers the type instance of a literal
         */
        type_instance infer_literal(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_tuple
         * infers the type instance of a tuple
         */
        type_instance infer_tuple(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_list
         * infers the type instance of a list
         */
        type_instance infer_list(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_map
         * infers the type instance of a map
         */
        type_instance infer_map(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_call
         * infers the type instance of a call expression
         */
        type_instance infer_call(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_identifier
         * infers the type instance of an identifier expression
         */
        type_instance infer_identifier(std::shared_ptr<expr> & an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_grouping
         * infers the type instance of a grouped expression
         */
        type_instance infer_grouping(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_cast
         * infers the type instance of a cast expression
         */
        type_instance infer_cast(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_unary
         * infers the type instance of a unary expression
         */
        type_instance infer_unary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_binary
         * infers the type instance of a binary expression
         */
        type_instance infer_binary(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_match
         * infers the type instance of a match expression
         */
        type_instance infer_match(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);

        /**
         * infer_assignment
         * infers the type instance of an assigment expression
         */
        type_instance infer_assignment(std::shared_ptr<expr>& an_expression, std::shared_ptr<scope> l_scope, const std::string& ns_name);
    };
}

#endif
