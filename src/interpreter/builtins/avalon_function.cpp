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

/* AST */
#include "representer/ast/decl/function.hpp"
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"

/* Implementations */
#include "interpreter/builtins/function_implementation.hpp"
#include "interpreter/builtins/avalon_function.hpp"

/* Quantum processor */
#include "interpreter/qprocessor.hpp"

/* Exceptions */
#include "interpreter/exceptions/invalid_call.hpp"


namespace avalon {
    /**
     * the constructor expects nothing
     */
    avalon_function::avalon_function(std::shared_ptr<function>& function_decl, std::shared_ptr<qprocessor>& qproc) : m_function_decl(function_decl), m_qproc(qproc) {
        /* boolean functions */
        and_implementation avl_and_implementation;
        or_implementation avl_or_implementation;
        not_implementation avl_not_implementation;

        /* arithmetic functions */
        pos_implementation avl_pos_implementation;
        neg_implementation avl_neg_implementation;
        add_implementation avl_add_implementation;
        sub_implementation avl_sub_implementation;
        mul_implementation avl_mul_implementation;
        div_implementation avl_div_implementation;
        mod_implementation avl_mod_implementation;
        pow_implementation avl_pow_implementation;

        /* io functions */
        print_implementation avl_print_implementation;
        println_implementation avl_println_implementation;
        readln_implementation avl_readln_implementation;

        /* casting functions */
        cast_implementation avl_cast_implementation(m_qproc);
        string_cast_implementation avl_string_cast_implementation;
        float_cast_implementation avl_float_cast_implementation;

        /* hashing functions */
        hash_implementation avl_hash_implementation;

        /* comparison functions */
        eq_implementation avl_eq_implementation;
        ne_implementation avl_ne_implementation;
        gt_implementation avl_gt_implementation;
        ge_implementation avl_ge_implementation;
        lt_implementation avl_lt_implementation;
        le_implementation avl_le_implementation;

        /* bitwise functions */
        band_implementation avl_band_implementation;
        bor_implementation avl_bor_implementation;
        xor_implementation avl_xor_implementation;
        bnot_implementation avl_bnot_implementation;

        /* quantum functions */
        apply_implementation avl_apply_implementation(m_qproc);
        measure_implementation avl_measure_implementation(m_qproc);

        m_implementations = {
            /* boolean functions */
            {"__and__", std::make_shared<and_implementation>(avl_and_implementation)},
            {"__or__", std::make_shared<or_implementation>(avl_or_implementation)},
            {"__not__", std::make_shared<not_implementation>(avl_not_implementation)},

            /* arithmetic functions */
            {"__pos__", std::make_shared<pos_implementation>(avl_pos_implementation)},
            {"__neg__", std::make_shared<neg_implementation>(avl_neg_implementation)},
            {"__add__", std::make_shared<add_implementation>(avl_add_implementation)},
            {"__sub__", std::make_shared<sub_implementation>(avl_sub_implementation)},
            {"__mul__", std::make_shared<mul_implementation>(avl_mul_implementation)},
            {"__div__", std::make_shared<div_implementation>(avl_div_implementation)},
            {"__mod__", std::make_shared<mod_implementation>(avl_mod_implementation)},
            {"__pow__", std::make_shared<pow_implementation>(avl_pow_implementation)},

            /* io functions */
            {"__print__", std::make_shared<print_implementation>(avl_print_implementation)},
            {"__println__", std::make_shared<println_implementation>(avl_println_implementation)},
            {"__readln__", std::make_shared<readln_implementation>(avl_readln_implementation)},

            /* casting functions */
            {"__cast__", std::make_shared<cast_implementation>(avl_cast_implementation)},
            {"string", std::make_shared<string_cast_implementation>(avl_string_cast_implementation)},
            {"float", std::make_shared<float_cast_implementation>(avl_float_cast_implementation)},

            /* hashing functions */
            {"__hash__", std::make_shared<hash_implementation>(avl_hash_implementation)},

            /* comparison functions */
            {"__eq__", std::make_shared<eq_implementation>(avl_eq_implementation)},
            {"__ne__", std::make_shared<ne_implementation>(avl_ne_implementation)},
            {"__gt__", std::make_shared<gt_implementation>(avl_gt_implementation)},
            {"__ge__", std::make_shared<ge_implementation>(avl_ge_implementation)},
            {"__lt__", std::make_shared<lt_implementation>(avl_lt_implementation)},
            {"__le__", std::make_shared<le_implementation>(avl_le_implementation)},

            /* bitwise functions */
            {"__band__", std::make_shared<band_implementation>(avl_band_implementation)},
            {"__bor__", std::make_shared<bor_implementation>(avl_bor_implementation)},
            {"__xor__", std::make_shared<xor_implementation>(avl_xor_implementation)},
            {"__bnot__", std::make_shared<bnot_implementation>(avl_bnot_implementation)},

            /* quantum functions */
            {"apply", std::make_shared<apply_implementation>(avl_apply_implementation)},
            {"measure", std::make_shared<measure_implementation>(avl_measure_implementation)},
        };
    }

    /**
     * run
     * given the function arguments find the appropritate implementation and run the function
     */
    std::shared_ptr<expr> avalon_function::run(std::vector<std::shared_ptr<expr> >& arguments) {
        const std::string& name = m_function_decl -> get_old_name();
        try {
            std::shared_ptr<function_implementation>& implementation = m_implementations.at(name);
            return (* implementation)(arguments);
        } catch(std::out_of_range err) {
            throw invalid_call("[compiler error] builtin function not implemented yet.");
        }
    }

    std::shared_ptr<expr> avalon_function::run(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
        const std::string& name = m_function_decl -> get_old_name();
        try {
            std::shared_ptr<function_implementation>& implementation = m_implementations.at(name);
            return (* implementation)(arguments, ret_instance);
        } catch(std::out_of_range err) {
            throw invalid_call("[compiler error] builtin function not implemented yet.");
        }
    }
}
