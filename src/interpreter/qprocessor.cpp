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

#include <algorithm>
#include <utility>
#include <memory>
#include <vector>
#include <tuple>

#include "qpp.h"

/* Error */
#include "error/error.hpp"

/* AST */
#include "representer/ast/expr/literal_expression.hpp"

/* Quantum processor */
#include "interpreter/qprocessor.hpp"


namespace avalon {
/**
 * the contructor expects:
 * - the error handler to be used to show errors
 * - the message to show the user
 */
qprocessor_error::qprocessor_error(error& error_handler, const std::string& message) : std::runtime_error(message), m_error_handler(error_handler) {
}

    /**
     * what
     * get the error message
     */
    const char * qprocessor_error::what() const noexcept {
        return std::runtime_error::what();
    }

    /**
     * show
     * calls the error handler error reporting function to display the error
     */
    void qprocessor_error::show() {
        m_error_handler.log(what());
    }


/**
 * The default constructor expects the error handler in case of errors
 */
qprocessor::qprocessor(error& error_handler) : m_error_handler(error_handler) {
}

    /**
     * add_ket
     * add a new ket to the processor registers.
     * returns the start and end index of the ket in the register.
     */
    std::pair<qpp::idx, qpp::idx> qprocessor::add_ket(std::shared_ptr<literal_expression> lit_expr, qpp::ket l_ket, qpp::idx length) {
        // make sure the ket has a non-size length
        if(length == 0)
            throw qprocessing_error("[compiler error] the length of the ket to add to the processor cannot be zero.");

        // add the literal expression to the processor
        m_literals.push_back(lit_expr);

        // update the internal ket
        if(m_ket.size() == 0) {
            m_ket = l_ket;
            m_length = length;
            return std::make_pair(0, m_length - 1);
        }
        else {
            m_ket = qpp::kron(m_ket, l_ket);
            qpp::idx old_length = m_length;
            m_length += length;
            return std::make_pair(old_length, m_length - 1);
        }
    }

    /**
     * apply
     * applies a unitary gate to kets from the given starting index to the given ending index
     */
    void qprocessor::apply(qpp::cmat& unitary, qpp::idx start_index, qpp::idx end_index) {
        // we perform bound checking on register indices
        if(end_index < start_index)
            throw qprocessing_error("[compiler error] the ending register index must be greater or equal to the starting register index.");

        // apply the gate to all the kets from the starting index to the ending index
        qpp::idx current = start_index;
        while(current < end_index + 1) {
            m_ket = qpp::apply(m_ket, unitary, {current});
            current += 1;
        }
    }

    /**
     * capply
     * applies a controlled unitary gate to the target ket given the control index
     */
    void qprocessor::capply(qpp::cmat& unitary, qpp::idx control_index, qpp::idx target_index) {
        m_ket = qpp::applyCTRL(m_ket, unitary, {control_index}, {target_index});
    }

    /**
     * measure
     * measure the qubit(s) at the specified indices then returns a vector of measurement results for each index
     */
    std::vector<qpp::idx> qprocessor::measure(qpp::idx start_index, qpp::idx end_index) {
        // we perform bound checking on register indices
        if(end_index < start_index)
            throw qprocessing_error("[compiler error] the ending register index must be greater or equal to the starting register index.");

        // we make sure the ending index is less than the length of the ket
        if(end_index > m_length)
            throw qprocessing_error("[compiler error] the ending register index is not within the register count upper bound.");
        
        // we build a vector of indices to be measured
        std::vector<qpp::idx> indices;
        qpp::idx current = start_index;
        while(current < end_index + 1) {
            indices.push_back(current);
            current += 1;
        }

        // perform the measurement
        auto measurement = qpp::measure_seq(m_ket, indices);
        m_ket = std::get<2>(measurement);

        // update the length
        m_length -= end_index - start_index + 1;

        // update the literals that are represented into this processor
        for(auto& lit : m_literals) {
            qpp::idx lit_start_index = lit -> get_start_index();
            qpp::idx lit_end_index = lit -> get_end_index();

            if(lit -> was_measured() || (lit_start_index == start_index && lit_end_index == end_index))
                continue;

            if(lit_start_index > end_index) {
                lit -> set_start_index(lit_start_index - (end_index - start_index + 1));
                lit -> set_end_index(lit_end_index - (end_index - start_index + 1));
            }
        }

        return std::get<0>(measurement);
    }

    /**
     * qprocessing_error
     * contructs and returns a qprocessor_error exception
     */
    qprocessor_error qprocessor::qprocessing_error(const std::string& message) {
        return qprocessor_error(m_error_handler, message);
    }
}
