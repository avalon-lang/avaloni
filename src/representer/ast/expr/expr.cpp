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

#include <stdexcept>
#include <memory>

/* AST */
#include "representer/ast/expr/dereference_expression.hpp"
#include "representer/ast/expr/conditional_expression.hpp"
#include "representer/ast/expr/assignment_expression.hpp"
#include "representer/ast/expr/underscore_expression.hpp"
#include "representer/ast/expr/identifier_expression.hpp"
#include "representer/ast/expr/reference_expression.hpp"
#include "representer/ast/expr/grouped_expression.hpp"
#include "representer/ast/expr/literal_expression.hpp"
#include "representer/ast/expr/binary_expression.hpp"
#include "representer/ast/expr/unary_expression.hpp"
#include "representer/ast/expr/match_expression.hpp"
#include "representer/ast/expr/tuple_expression.hpp"
#include "representer/ast/expr/list_expression.hpp"
#include "representer/ast/expr/call_expression.hpp"
#include "representer/ast/expr/cast_expression.hpp"
#include "representer/ast/expr/map_expression.hpp"
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * copy
     * creates a copy of an expression and returns it.
     * ideally, this should be the assignment copy operator but I suck at C++ so please educate me when you see this if you inclined to do so.
     */
    std::shared_ptr<expr> expr::copy() {
        if(this == nullptr)
            return nullptr;

        if(this -> is_underscore_expression()) {
            std::shared_ptr<underscore_expression> const & under_expr = std::static_pointer_cast<underscore_expression>(shared_from_this());
            std::shared_ptr<underscore_expression> l_under_expr = std::make_shared<underscore_expression>(under_expr);
            std::shared_ptr<expr> new_expression = l_under_expr;
            return new_expression;
        }
        else if(this -> is_reference_expression()) {
            std::shared_ptr<reference_expression> const & ref_expr = std::static_pointer_cast<reference_expression>(shared_from_this());
            std::shared_ptr<reference_expression> l_ref_expr = std::make_shared<reference_expression>(ref_expr);
            std::shared_ptr<expr> new_expression = l_ref_expr;
            return new_expression;
        }
        else if(this -> is_dereference_expression()) {
            std::shared_ptr<dereference_expression> const & dref_expr = std::static_pointer_cast<dereference_expression>(shared_from_this());
            std::shared_ptr<dereference_expression> l_dref_expr = std::make_shared<dereference_expression>(dref_expr);
            std::shared_ptr<expr> new_expression = l_dref_expr;
            return new_expression;
        }
        else if(this -> is_literal_expression()) {
            std::shared_ptr<literal_expression> const & lit_expr = std::static_pointer_cast<literal_expression>(shared_from_this());
            std::shared_ptr<literal_expression> l_lit_expr = std::make_shared<literal_expression>(lit_expr);
            std::shared_ptr<expr> new_expression = l_lit_expr;
            return new_expression;
        }
        else if(this -> is_grouped_expression()) {
            std::shared_ptr<grouped_expression> const & gr_expr = std::static_pointer_cast<grouped_expression>(shared_from_this());
            std::shared_ptr<grouped_expression> l_gr_expr = std::make_shared<grouped_expression>(gr_expr);
            std::shared_ptr<expr> new_expression = l_gr_expr;
            return new_expression;
        }
        else if(this -> is_tuple_expression()) {
            std::shared_ptr<tuple_expression> const & tup_expr = std::static_pointer_cast<tuple_expression>(shared_from_this());
            std::shared_ptr<tuple_expression> l_tup_expr = std::make_shared<tuple_expression>(tup_expr);
            std::shared_ptr<expr> new_expression = l_tup_expr;
            return new_expression;
        }
        else if(this -> is_list_expression()) {
            std::shared_ptr<list_expression> const & list_expr = std::static_pointer_cast<list_expression>(shared_from_this());
            std::shared_ptr<list_expression> l_list_expr = std::make_shared<list_expression>(list_expr);
            std::shared_ptr<expr> new_expression = l_list_expr;
            return new_expression;
        }
        else if(this -> is_map_expression()) {
            std::shared_ptr<map_expression> const & map_expr = std::static_pointer_cast<map_expression>(shared_from_this());
            std::shared_ptr<map_expression> l_map_expr = std::make_shared<map_expression>(map_expr);
            std::shared_ptr<expr> new_expression = l_map_expr;
            return new_expression;
        }
        else if(this -> is_identifier_expression()) {
            std::shared_ptr<identifier_expression> const & id_expr = std::static_pointer_cast<identifier_expression>(shared_from_this());
            std::shared_ptr<identifier_expression> l_id_expr = std::make_shared<identifier_expression>(id_expr);
            std::shared_ptr<expr> new_expression = l_id_expr;
            return new_expression;
        }
        else if(this -> is_call_expression()) {
            std::shared_ptr<call_expression> const & call_expr = std::static_pointer_cast<call_expression>(shared_from_this());
            std::shared_ptr<call_expression> l_call_expr = std::make_shared<call_expression>(call_expr);
            std::shared_ptr<expr> new_expression = l_call_expr;
            return new_expression;
        }
        else if(this -> is_cast_expression()) {
            std::shared_ptr<cast_expression> const & cast_expr = std::static_pointer_cast<cast_expression>(shared_from_this());
            std::shared_ptr<cast_expression> l_cast_expr = std::make_shared<cast_expression>(cast_expr);
            std::shared_ptr<expr> new_expression = l_cast_expr;
            return new_expression;
        }
        else if(this -> is_unary_expression()) {
            std::shared_ptr<unary_expression> const & un_expr = std::static_pointer_cast<unary_expression>(shared_from_this());
            std::shared_ptr<unary_expression> l_un_expr = std::make_shared<unary_expression>(un_expr);
            std::shared_ptr<expr> new_expression = l_un_expr;
            return new_expression;
        }
        else if(this -> is_binary_expression()) {
            std::shared_ptr<binary_expression> const & bin_expr = std::static_pointer_cast<binary_expression>(shared_from_this());
            std::shared_ptr<binary_expression> l_bin_expr = std::make_shared<binary_expression>(bin_expr);
            std::shared_ptr<expr> new_expression = l_bin_expr;
            return new_expression;
        }
        else if(this -> is_match_expression()) {
            std::shared_ptr<match_expression> const & match_expr = std::static_pointer_cast<match_expression>(shared_from_this());
            std::shared_ptr<match_expression> l_match_expr = std::make_shared<match_expression>(match_expr);
            std::shared_ptr<expr> new_expression = l_match_expr;
            return new_expression;
        }
        else if(this -> is_assignment_expression()) {
            std::shared_ptr<assignment_expression> const & assign_expr = std::static_pointer_cast<assignment_expression>(shared_from_this());
            std::shared_ptr<assignment_expression> l_assign_expr = std::make_shared<assignment_expression>(assign_expr);
            std::shared_ptr<expr> new_expression = l_assign_expr;
            return new_expression;
        }
        else if(this -> is_conditional_expression()) {
            std::shared_ptr<conditional_expression> const & cond_expr = std::static_pointer_cast<conditional_expression>(shared_from_this());
            std::shared_ptr<conditional_expression> l_cond_expr = std::make_shared<conditional_expression>(cond_expr);
            std::shared_ptr<expr> new_expression = l_cond_expr;
            return new_expression;
        }
        else {
            throw std::runtime_error("[compiler error] unexpected expression.");
        }
    }
}
