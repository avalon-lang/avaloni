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

#ifndef AVALON_INTERPRETER_BUILTINS_LANG_QUBIT_HPP_
#define AVALON_INTERPRETER_BUILTINS_LANG_QUBIT_HPP_

#include <memory>
#include <vector>

/* AST */
#include "representer/ast/decl/type.hpp"
#include "representer/ast/expr/expr.hpp"


namespace avalon {
    /**
     * qubit_apply
     * applies a quantum gate to the qubits stored at the given reference
     */
    std::shared_ptr<expr> qubit_apply(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * qubit_measure
     * performs a measure on the qubits stored at the given reference in the Z basis
     */
    std::shared_ptr<expr> qubit_measure(std::vector<std::shared_ptr<expr> >& arguments);

    /**
     * qubit_cast
     * Performs a measurement of the qubits stored at the given reference in the Z basis
     */
    std::shared_ptr<expr> qubit_cast(std::vector<std::shared_ptr<expr> >& arguments, type_instance& ret_instance);
}

#endif
