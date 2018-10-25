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
#include <cstddef>
#include <memory>
#include <vector>
#include <string>

/* AST */
/* Declarations */
#include "representer/ast/decl/statement.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/type.hpp"
/* Statements */
#include "representer/ast/stmt/expression_stmt.hpp"
#include "representer/ast/stmt/return_stmt.hpp"
#include "representer/ast/stmt/while_stmt.hpp"
#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/stmt/if_stmt.hpp"
#include "representer/ast/stmt/stmt.hpp"
/* Expressions */
#include "representer/ast/expr/assignment_expression.hpp"
#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/grouped_expression.hpp"
#include "representer/ast/expr/binary_expression.hpp"
#include "representer/ast/expr/unary_expression.hpp"
#include "representer/ast/expr/match_expression.hpp"
#include "representer/ast/expr/tuple_expression.hpp"
#include "representer/ast/expr/list_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/cast_expression.hpp"
#include "representer/ast/expr/map_expression.hpp"
#include "representer/ast/expr/expr.hpp"

/* Symtable */
#include "representer/symtable/scope.hpp"

/* Exceptions */
#include "checker/exceptions/invalid_function.hpp"
#include "checker/exceptions/invalid_type.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* Checker */
#include "checker/decl/type/type_checker.hpp"

/* Generator */
#include "checker/decl/function/function_generator.hpp"


namespace avalon {
    /*
    static void replace_instance(type_instance& dest, type_instance& orig) {
        std::vector<type_instance>& orig_params = orig.get_params();

        // update the name
        const std::string& orig_name = orig.get_name();
        dest.set_name(orig_name);

        // we update the token
        const token& orig_tok = orig.get_token();
        dest.set_token(orig_tok);

        // we update the category
        const type_instance_category& orig_category = orig.get_category();
        dest.set_category(orig_category);

        // we update the namespace
        const std::string& ns_name = orig.get_namespace();
        dest.set_namespace(ns_name);

        // we update the type
        std::shared_ptr<type>& orig_type = orig.get_type();
        dest.set_type(orig_type);

        // we update the elements count (in the case of lists and map)
        std::size_t orig_count = orig.get_count();
        dest.set_count(orig_count);

        // we update the parametrization status
        dest.is_parametrized(orig.is_parametrized());

        // we set parameters            
        for(auto& orig_param : orig_params)
            dest.add_param(orig_param);
    }
    */

    /**
     * the default constructor expects nothing
     */
    function_generator::function_generator(function& fun_decl, std::vector<type_instance>& constraint_instances, std::shared_ptr<scope>& l_scope, const std::string& ns_name) : m_fun_decl(fun_decl), m_instances(constraint_instances), m_scope(l_scope), m_namespace(ns_name) {
    }

    /**
     * generate
     * given a function declaration, a vector parameters' type instances and the return type instance,
     * this function generates a function were all constraints have been replaced by the appropriate type instances
     */
    void function_generator::generate(std::vector<type_instance>& param_instances, type_instance& ret_instance) {        
        std::vector<token>& constraints = m_fun_decl.get_constraints();
        std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params = m_fun_decl.get_params();
        type_instance& fun_ret_instance = m_fun_decl.get_return_type_instance();
        block_stmt& body = m_fun_decl.get_body();        

        // if the vector of constraints instances supplied by the user is not empty,
        // we make sure that it has the same size as the vector of constraints
        if(m_instances.empty() == false) {
            if(m_instances.size() != constraints.size()) {
                throw invalid_function(m_fun_decl.get_token(), "The supplied list of type instances for constraints resolution doesn't have the same size as the list of constraints the function expects.");
            }
            else {
                // if they are the same size, we proceed to fill the constraints map
                auto cons_ins_it = m_instances.begin(), cons_ins_end = m_instances.end();
                auto cons_it = constraints.begin(), cons_end = constraints.end();
                for(; cons_ins_it != cons_ins_end && cons_it != cons_end; ++cons_ins_it, ++cons_it) {
                    try {
                        std::pair<bool,bool> res = type_instance_checker::complex_check(* cons_ins_it, m_scope, m_namespace, constraints);
                        if(res.second == true) {
                            cons_ins_it -> is_parametrized(true);
                        }
                    } catch(invalid_type err) {
                        throw err;
                    }
                    m_constraint_instances.emplace(cons_it -> get_lexeme(), * cons_ins_it);
                }
            }
        }

        // we work with the function parameters
        generate_params(params, param_instances);

        // we work with the function return type
        generate_return(fun_ret_instance, ret_instance);

        // we work with the function body
        generate_body(body);
    }

    /*
     * generate_params
     * generates a new set of the function parameters replacing the constraints with the given type instances
     */
    void function_generator::generate_params(std::vector<std::pair<std::string, std::shared_ptr<variable> > >& params, std::vector<type_instance>& param_instances) {
        auto param_it = params.begin(), param_end = params.end();
        auto param_ins_it = param_instances.begin(), param_ins_end = param_instances.end();
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        for(; param_it != param_end && param_ins_it != param_ins_end; ++param_it, ++param_ins_it) {
            type_instance& param_instance = param_it -> second -> get_type_instance();
            if(param_instance.depends_on(constraints)) {
                build_instance(param_instance, * param_ins_it);
            }
        }
    }

    /*
     * generate_return
     * generates a new return type instance replacing the constraints with the given type instances
     */
    void function_generator::generate_return(type_instance& fun_ret_instance, type_instance& new_ret_instance) {
        std::vector<token>& constraints = m_fun_decl.get_constraints();        
        if(fun_ret_instance.depends_on(constraints)) {
            build_instance(fun_ret_instance, new_ret_instance);            
        }
    }

    /*
     * generate_body
     * generates a new return type instance replacing the constraints with the given type instances
     */
    void function_generator::generate_body(block_stmt& body) {        
        // we make sure that all constraints have associated types
        std::vector<token>& constraints = m_fun_decl.get_constraints();
        if(m_instances.size() != constraints.size()) {
            for(auto& constraint : constraints) {
                if(m_constraint_instances.count(constraint.get_lexeme()) == 0)
                    throw invalid_type(constraint, "Constraint <" + constraint.get_lexeme() + "> lacks a type instance. All constraints must have their types instances set before entering the function body.");
            }
        }

        // we here, all we need to do now is look for constraints in the function body and perform the replacement
        generate_block(body);
    }

    /*
     * generate_variable
     * generate new type instances in a variable declaration, replace constraints with supplied type instances
     */
    void function_generator::generate_variable(std::shared_ptr<decl>& declaration) {
        std::shared_ptr<variable> const & var_decl = std::static_pointer_cast<variable>(declaration);
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        // if the variable declaration has a type instance, we see if it is parametrized
        if(var_decl -> has_type_instance()) {
            type_instance var_instance = var_decl -> get_type_instance();
            // we validate that the type is correct
            try {
                std::pair<bool,bool> res = type_instance_checker::complex_check(var_instance, m_scope, m_namespace, constraints);
                if(res.second == true)
                    var_instance.is_parametrized(true);
            } catch(invalid_type err) {
                throw err;
            }
            
            token gen_tok(MUL, "*", 0, 0, "__bit__");
            type_instance gen_instance(gen_tok, "*");
            build_instance(var_instance, gen_instance);
            var_decl -> set_type_instance(var_instance);
        }

        // if the variable declaration was initialized, we work on the initializer expression
        std::shared_ptr<expr>& var_val = var_decl -> get_value();
        if(var_val != nullptr) {
            generate_expression(var_val);
        }
    }

    /*
     * generate_statement
     * generate new type instances in a statement declaration, replace constraints with supplied type instances
     */
    void function_generator::generate_statement(std::shared_ptr<decl>& declaration) {
        std::shared_ptr<statement> const & a_declaration = std::static_pointer_cast<statement>(declaration);
        std::shared_ptr<stmt>& a_statement = a_declaration -> get_statement();
        
        if(a_statement -> is_while()) {
            generate_while(a_statement);
        }
        else if(a_statement -> is_if()) {
            generate_if(a_statement);
        }
        else if(a_statement -> is_break()) {
            return;
        }
        else if(a_statement -> is_continue()) {
            return;
        }
        else if(a_statement -> is_pass()) {
            return;
        }
        else if(a_statement -> is_return()) {
            generate_return(a_statement);
        }
        else if(a_statement -> is_block()) {
            generate_block(a_statement);
        }
        else if(a_statement -> is_expression()) {
            generate_expression(a_statement);
        }
        else {
            throw std::runtime_error("[compiler error] unexpected statement type in statement checker.");
        }
    }

    void function_generator::generate_while(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<while_stmt> const & w_stmt = std::static_pointer_cast<while_stmt>(a_statement);

        // we work on the while condition
        std::shared_ptr<expr>& condition = w_stmt -> get_condition();
        generate_expression(condition);

        // we work on the body of the while loop
        block_stmt& body = w_stmt -> get_block();
        generate_block(body);
    }

    void function_generator::generate_if(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<if_stmt> const & i_stmt = std::static_pointer_cast<if_stmt>(a_statement);

        /* we work on the if branch */
        // we work on the if condition
        std::shared_ptr<expr>& if_condition = i_stmt -> get_condition();
        generate_expression(if_condition);

        // we work on the body of the if conditional
        block_stmt& body = i_stmt -> get_block();
        generate_block(body);

        /* we work on elif branches */
        std::vector<elif_branch>& elf_branches = i_stmt -> get_elifs();
        for(auto& elf_branch : elf_branches) {
            // we work on the condition
            std::shared_ptr<expr>& elf_condition = elf_branch.get_condition();
            generate_expression(elf_condition);

            // we work on the body
            block_stmt& elf_body = elf_branch.get_block();
            generate_block(elf_body);
        }

        /* we work on the else branch */
        if(i_stmt -> has_else()) {
            else_branch els_branch = i_stmt -> get_else();

            // we work on the body
            block_stmt& els_body = els_branch.get_block();
            generate_block(els_body);

            // we replace the old branch
            i_stmt -> set_else(els_branch);
        }
    }

    void function_generator::generate_return(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<return_stmt> const & r_stmt = std::static_pointer_cast<return_stmt>(a_statement);

        // if an expression was supplied to the return statement, we work on it
        if(r_stmt -> has_expression()) {
            std::shared_ptr<expr>& ret_expr = r_stmt -> get_expression();
            generate_expression(ret_expr);
        }
    }

    void function_generator::generate_block(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<block_stmt> const & b_stmt = std::static_pointer_cast<block_stmt>(a_statement);
        generate_block(* b_stmt);
    }

    void function_generator::generate_block(block_stmt& b_stmt) {
        std::vector<std::shared_ptr<decl> >& body_decls = b_stmt.get_declarations();

        // we go over declarations and work on them
        for(auto& body_decl : body_decls) {
            if(body_decl -> is_variable()) {
                generate_variable(body_decl);
            }
            else if(body_decl -> is_statement()) {
                generate_statement(body_decl);
            }
            else {
                throw invalid_function(m_fun_decl.get_token(), "Unexpected declaration inside function body. Only variable and statement declarations are allowed inside a function body.");
            }
        }
    }

    void function_generator::generate_expression(std::shared_ptr<stmt>& a_statement) {
        std::shared_ptr<expression_stmt> const & e_stmt = std::static_pointer_cast<expression_stmt>(a_statement);
        std::shared_ptr<expr>& an_expression = e_stmt -> get_expression();
        generate_expression(an_expression);
    }

    /**
     * generate_expression
     * generate a new type instance attached to an expression, replacing constraints with supplied type instances
     */
    void function_generator::generate_expression(std::shared_ptr<expr>& an_expression) {
        if(an_expression -> is_underscore_expression()) {
            return;
        }
        else if(an_expression -> is_literal_expression()) {
            return;
        }
        else if(an_expression -> is_reference_expression()) {
            return;
        }
        else if(an_expression -> is_dereference_expression()) {
            return;
        }
        else if(an_expression -> is_tuple_expression()) {
            generate_tuple(an_expression);
        }
        else if(an_expression -> is_list_expression()) {
            generate_list(an_expression);
        }
        else if(an_expression -> is_map_expression()) {
            generate_map(an_expression);
        }
        else if(an_expression -> is_call_expression()) {
            generate_call(an_expression);
        }
        else if(an_expression -> is_identifier_expression()) {
            generate_identifier(an_expression);
        }
        else if(an_expression -> is_cast_expression()) {
            generate_cast(an_expression);
        }
        else if(an_expression -> is_match_expression()) {
            generate_match(an_expression);
        }
        else if(an_expression -> is_grouped_expression()) {
            generate_grouped(an_expression);
        }
        else if(an_expression -> is_binary_expression()) {
            generate_binary(an_expression);
        }
        else if(an_expression -> is_unary_expression()) {
            generate_unary(an_expression);
        }
        else if(an_expression -> is_assignment_expression()) {
            generate_assignment(an_expression);
        }
        else {
            throw std::runtime_error("[compiler error] unexpected expression type in expression function generator.");
        }
    }

    void function_generator::generate_tuple(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<tuple_expression> const & tuple_expr = std::static_pointer_cast<tuple_expression>(an_expression);
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        // if the tuple has a type instance attached, we work on that
        if(tuple_expr -> has_type_instance()) {
            type_instance& instance = tuple_expr -> get_type_instance();
            std::pair<bool,bool> res = type_instance_checker::complex_check(instance, m_scope, m_namespace, constraints);
            if(res.second == true) {
                instance.is_parametrized(true);
            }
            if(instance.depends_on(constraints)) {
                token gen_tok(MUL, "*", 0, 0, "__bit__");
                type_instance gen_instance(gen_tok, "*");
                build_instance(instance, gen_instance);
            }
        }

        // if the tuple has some elements specified, we do the same to them
        std::vector<std::pair<std::string, std::shared_ptr<expr> > >& elements = tuple_expr -> get_elements();
        for(auto& element : elements)
            generate_expression(element.second);
    }

    void function_generator::generate_list(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<list_expression> const & list_expr = std::static_pointer_cast<list_expression>(an_expression);
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        // if the list has a type instance attached, we work on that
        if(list_expr -> has_type_instance()) {
            try {
                type_instance& instance = list_expr -> get_type_instance();
                std::pair<bool,bool> res = type_instance_checker::complex_check(instance, m_scope, m_namespace, constraints);
                if(res.second == true) {
                    instance.is_parametrized(true);
                }
                if(instance.depends_on(constraints)) {
                    token gen_tok(MUL, "*", 0, 0, "__bit__");
                    type_instance gen_instance(gen_tok, "*");
                    build_instance(instance, gen_instance);
                }
            } catch(invalid_type err) {
                throw err;
            }
        }

        // if the list has some elements specified, we do the same to them
        std::vector<std::shared_ptr<expr> >& elements = list_expr -> get_elements();
        for(auto& element : elements)
            generate_expression(element);
    }

    void function_generator::generate_map(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<map_expression> const & map_expr = std::static_pointer_cast<map_expression>(an_expression);
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        // if the map has a type instance attached, we work on that
        if(map_expr -> has_type_instance()) {
            try {
                type_instance& instance = map_expr -> get_type_instance();
                std::pair<bool,bool> res = type_instance_checker::complex_check(instance, m_scope, m_namespace, constraints);
                if(res.second == true) {
                    instance.is_parametrized(true);
                }
                if(instance.depends_on(constraints)) {
                    token gen_tok(MUL, "*", 0, 0, "__bit__");
                    type_instance gen_instance(gen_tok, "*");
                    build_instance(instance, gen_instance);
                }
            } catch(invalid_type err) {
                throw err;
            }
        }

        // if the map has some elements specified, we do the same to them
        std::vector<std::pair<std::shared_ptr<expr>, std::shared_ptr<expr> > >& elements = map_expr -> get_elements();
        for(auto& element : elements)
            generate_expression(element.second);
    }

    void function_generator::generate_call(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(an_expression);
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        // if the call has a type instance attached, we work on that
        if(call_expr -> has_type_instance()) {            
            try {
                type_instance& instance = call_expr -> get_type_instance();
                std::pair<bool,bool> res = type_instance_checker::complex_check(instance, m_scope, m_namespace, constraints);
                if(res.second == true) {
                    instance.is_parametrized(true);
                }
                if(instance.depends_on(constraints)) {
                    token gen_tok(MUL, "*", 0, 0, "__bit__");
                    type_instance gen_instance(gen_tok, "*");
                    build_instance(instance, gen_instance);
                }
            } catch(invalid_type err) {
                throw err;
            }
        }

        // if the call expression has arguments, we work on them
        std::vector<std::pair<token, std::shared_ptr<expr> > >& args = call_expr -> get_arguments();
        for(auto& arg : args)
            generate_expression(arg.second);

        // if the call expression has a return type specified, we work on it   
        if(call_expr -> has_return_type_instance()) {
            try {
                type_instance& ret_instance = call_expr -> get_return_type_instance();
                std::pair<bool,bool> res = type_instance_checker::complex_check(ret_instance, m_scope, m_namespace, constraints);
                if(res.second == true) {
                    ret_instance.is_parametrized(true);
                }
                if(ret_instance.depends_on(constraints)) {
                    token gen_tok(MUL, "*", 0, 0, "__bit__");
                    type_instance gen_instance(gen_tok, "*");
                    build_instance(ret_instance, gen_instance);
                }
            } catch(invalid_type err) {
                throw err;
            }
        }        
    }

    void function_generator::generate_identifier(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<identifier_expression> const & identifier_expr = std::static_pointer_cast<identifier_expression>(an_expression);
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        // if the identifier has a type instance attached, we work on that
        if(identifier_expr -> has_type_instance()) {            
            try {
                type_instance& instance = identifier_expr -> get_type_instance();
                std::pair<bool,bool> res = type_instance_checker::complex_check(instance, m_scope, m_namespace, constraints);
                if(res.second == true) {
                    instance.is_parametrized(true);
                }
                if(instance.depends_on(constraints)) {
                    token gen_tok(MUL, "*", 0, 0, "__bit__");
                    type_instance gen_instance(gen_tok, "*");
                    build_instance(instance, gen_instance);
                }
            } catch(invalid_type err) {
                throw err;
            }
        }
    }

    void function_generator::generate_cast(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<cast_expression> const & cast_expr = std::static_pointer_cast<cast_expression>(an_expression);
        std::vector<token>& constraints = m_fun_decl.get_constraints();

        // work on the type instance to cast to
        try {
            type_instance& instance = cast_expr -> get_cast_type_instance();
            std::pair<bool,bool> res = type_instance_checker::complex_check(instance, m_scope, m_namespace, constraints);
            if(res.second == true) {
                instance.is_parametrized(true);
            }
            if(instance.depends_on(constraints)) {
                token gen_tok(MUL, "*", 0, 0, "__bit__");
                type_instance gen_instance(gen_tok, "*");
                build_instance(instance, gen_instance);
            }
        } catch(invalid_type err) {
            throw err;
        }

        // work on the expression to cast
        std::shared_ptr<expr>& cast_val = cast_expr -> get_val();
        generate_expression(cast_val);
    }

    void function_generator::generate_match(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<match_expression> const & match_expr = std::static_pointer_cast<match_expression>(an_expression);

        // work on the left side of the match expression
        std::shared_ptr<expr>& lval = match_expr -> get_lval();
        generate_expression(lval);

        // work on the right side of the match expression
        std::shared_ptr<expr>& rval = match_expr -> get_rval();
        generate_expression(rval);
    }

    void function_generator::generate_grouped(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<grouped_expression> const & grouped_expr = std::static_pointer_cast<grouped_expression>(an_expression);

        // work on the expression that's grouped
        std::shared_ptr<expr>& val = grouped_expr -> get_value();
        generate_expression(val);
    }

    void function_generator::generate_binary(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<binary_expression> const & binary_expr = std::static_pointer_cast<binary_expression>(an_expression);

        // work on the left side of the binary expression
        std::shared_ptr<expr>& lval = binary_expr -> get_lval();
        generate_expression(lval);

        // work on the right side of the binary expression
        std::shared_ptr<expr>& rval = binary_expr -> get_rval();
        generate_expression(rval);
    }

    void function_generator::generate_unary(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<unary_expression> const & unary_expr = std::static_pointer_cast<unary_expression>(an_expression);

        // work on the expression that is the subject of the unary operator
        std::shared_ptr<expr>& val = unary_expr -> get_val();
        generate_expression(val);
    }

    void function_generator::generate_assignment(std::shared_ptr<expr>& an_expression) {
        std::shared_ptr<assignment_expression> const & assign_expr = std::static_pointer_cast<assignment_expression>(an_expression);

        // work on the left side of the assignment expression
        std::shared_ptr<expr>& lval = assign_expr -> get_lval();
        generate_expression(lval);

        // work on the right side of the assignment expression
        std::shared_ptr<expr>& rval = assign_expr -> get_rval();
        generate_expression(rval);
    }


    /**
     * build_instance
     * replaces the abstract type instances in dest with concrete type instances in orig
     * then update the constraints map above to indicate that we found a match
     */
    void function_generator::build_instance(type_instance& dest, type_instance& orig) {
        std::vector<type_instance>& dest_params = dest.get_params();
        std::vector<type_instance>& orig_params = orig.get_params();

        // if the destination type instance is abstract, we replace it with the origin type instance
        // we make sure to only work on abstract types that are the result of a constraint being specified
        if(dest.is_abstract() && dest.get_token() != star_tok) {
            // first, we look into the constraint map if this destination type instance is not already there
            try {
                type_instance& exist_instance = m_constraint_instances.at(dest.get_name());
                // if the orig is not an abstract type instance, we make sure it matches exactly the one in the constraint map
                if(orig.is_abstract() == false) {                    
                    try {
                        type_instance_checker::complex_check(orig, m_scope, m_namespace);
                    } catch(invalid_type err) {
                        throw err;
                    }

                    if(type_instance_strong_compare(exist_instance, orig) ==  false) {
                        throw invalid_type(orig.get_token(), "Expected type instance <" + mangle_type_instance(exist_instance) + "> but got type instance <" + mangle_type_instance(orig) + ">.");
                    }

                    // perform the replacement
                    dest.copy(orig);
                    //replace_instance(dest, orig);
                }
                // if the origin type instance is abstract, we don't mind and make the replacement based on what we have in the constraint map
                else {
                    dest.copy(exist_instance);
                    //replace_instance(dest, exist_instance);
                }
            } catch(std::out_of_range err) {
                // if the origin type instance is incomplete (parametrized or abstract), reject it
                if(orig.is_complete() == false) {
                    throw invalid_type(orig.get_token(), "The type instance <" + mangle_type_instance(orig) + "> must not be incomplete.");
                }

                // typecheck the origin type instances
                try {
                    type_instance_checker::complex_check(orig, m_scope, m_namespace);
                } catch(invalid_type err) {
                    throw err;
                }

                // perform the replacement
                dest = dest.copy(orig);
                //replace_instance(dest, orig);

                // we update the map with this new constraint type instance
                m_constraint_instances.emplace(dest.get_old_token().get_lexeme(), orig);
            }
        }
        // if the destination type instance is not abstract, we check if it is parametrized (which means it depends on an abstract type instance)
        else {
            if(dest.is_parametrized()) {
                if(orig.is_star() == false) {
                    auto dest_it = dest_params.begin(), dest_end = dest_params.end();
                    auto orig_it = orig_params.begin(), orig_end = orig_params.end();
                    for(; dest_it != dest_end && orig_it != orig_end; ++dest_it, ++orig_it) {
                        build_instance(* dest_it, * orig_it);
                    }
                }
                else {
                    for(auto& dest_param : dest_params)
                        build_instance(dest_param, orig);
                }
            }
        }

        // typecheck the built type instance
        try {
            type_instance_checker::complex_check(dest, m_scope, m_namespace);
        } catch(invalid_type err) {
            throw err;
        }
    }
}
