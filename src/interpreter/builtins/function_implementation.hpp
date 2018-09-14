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

#ifndef AVALON_INTERPRETER_BUILTINS_FUNCTION_IMPLEMENTATION_HPP_
#define AVALON_INTERPRETER_BUILTINS_FUNCTION_IMPLEMENTATION_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"

/* Implementations */
#include "interpreter/builtins/lang/avalon_arithmetic.hpp"
#include "interpreter/builtins/lang/avalon_comparison.hpp"
#include "interpreter/builtins/lang/avalon_bool.hpp"
#include "interpreter/builtins/lang/avalon_cast.hpp"
#include "interpreter/builtins/lang/avalon_hash.hpp"
#include "interpreter/builtins/lang/avalon_bit.hpp"
#include "interpreter/builtins/io/avalon_io.hpp"


namespace avalon {
    struct function_implementation {
    public:
        /**
         * call operator
         * given the function arguments (and return type if available), find the appropriate function, run it and return the results
         */
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) = 0;
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) = 0;
    };

    /**
     * and_implementation
     * implements the builtin __and__ function
     */
    struct and_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return bool_and(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return bool_and(arguments);
        }
    };

    /**
     * or_implementation
     * implements the builtin __or__ function
     */
    struct or_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return bool_or(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return bool_or(arguments);
        }
    };

    /**
     * not_implementation
     * implements the builtin __not__ function
     */
    struct not_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return bool_not(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return bool_not(arguments);
        }
    };

    /**
     * pos_implementation
     * implements the builtin __pos__ function
     */
    struct pos_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_pos(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_pos(arguments);
        }
    };

    /**
     * neg_implementation
     * implements the builtin __neg__ function
     */
    struct neg_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_neg(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_neg(arguments);
        }
    };

    /**
     * add_implementation
     * implements the builtin __add__ function
     */
    struct add_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_add(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_add(arguments);
        }
    };

    /**
     * sub_implementation
     * implements the builtin __sub__ function
     */
    struct sub_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_sub(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_sub(arguments);
        }
    };

    /**
     * mul_implementation
     * implements the builtin __mul__ function
     */
    struct mul_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_mul(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_mul(arguments);
        }
    };

    /**
     * div_implementation
     * implements the builtin __div__ function
     */
    struct div_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_div(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_div(arguments);
        }
    };

    /**
     * mod_implementation
     * implements the builtin __mod__ function
     */
    struct mod_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_mod(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_mod(arguments);
        }
    };

    /**
     * pow_implementation
     * implements the builtin __pow__ function
     */
    struct pow_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_pow(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_pow(arguments);
        }
    };

    /**
     * print_implementation
     * implements the builtin __print__ function
     */
    struct print_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_print(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_print(arguments);
        }
    };

    /**
     * println_implementation
     * implements the builtin __println__ function
     */
    struct println_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_println(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_println(arguments);
        }
    };

    /**
     * cast_implementation
     * implements the builtin __cast__ function
     */
    struct cast_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_cast(arguments, star_instance);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_cast(arguments, ret_instance);
        }
    };

    /**
     * string_cast_implementation
     * implements the builtin string function
     */
    struct string_cast_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_string_cast(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_string_cast(arguments);
        }
    };

    /**
     * float_cast_implementation
     * implements the builtin float function
     */
    struct float_cast_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_float_cast(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_float_cast(arguments);
        }
    };

    /**
     * hash_implementation
     * implements the builtin __hash__ function
     */
    struct hash_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_hash(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_hash(arguments);
        }
    };

    /**
     * eq_implementation
     * implements the builtin __eq__ function
     */
    struct eq_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_eq(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_eq(arguments);
        }
    };

    /**
     * ne_implementation
     * implements the builtin __ne__ function
     */
    struct ne_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_ne(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_ne(arguments);
        }
    };

    /**
     * gt_implementation
     * implements the builtin __gt__ function
     */
    struct gt_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_gt(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_gt(arguments);
        }
    };

    /**
     * ge_implementation
     * implements the builtin __ge__ function
     */
    struct ge_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_ge(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_ge(arguments);
        }
    };

    /**
     * lt_implementation
     * implements the builtin __lt__ function
     */
    struct lt_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_lt(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_lt(arguments);
        }
    };

    /**
     * le_implementation
     * implements the builtin __le__ function
     */
    struct le_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return avl_le(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return avl_le(arguments);
        }
    };

    /**
     * band_implementation
     * implements the builtin __band__ function
     */
    struct band_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return bit_and(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return bit_and(arguments);
        }
    };

    /**
     * bor_implementation
     * implements the builtin __bor__ function
     */
    struct bor_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return bit_or(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return bit_or(arguments);
        }
    };

    /**
     * xor_implementation
     * implements the builtin __xor__ function
     */
    struct xor_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return bit_xor(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return bit_xor(arguments);
        }
    };

    /**
     * bnot_implementation
     * implements the builtin __bnot__ function
     */
    struct bnot_implementation : function_implementation {
        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments) {
            return bit_not(arguments);
        }

        virtual std::shared_ptr<expr> operator()(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance) {
            return bit_not(arguments);
        }
    };
}

#endif
