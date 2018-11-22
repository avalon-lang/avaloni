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

#ifndef AVALON_INTERPRETER_QPROCESSOR_HPP_
#define AVALON_INTERPRETER_QPROCESSOR_HPP_

#include <cstddef>
#include <memory>
#include <vector>

#include "qpp.h"

/* AST */
#include "representer/ast/expr/literal_expression.hpp"

/* Error */
#include "error/error.hpp"


namespace avalon {
    class qprocessor_error : public std::runtime_error {
    public:
        /**
         * the contructor expects:
         * - the error handler to be used to show errors
         * - the message to show the user
         */
        qprocessor_error(error& error_handler, const std::string& message);

        /**
         * what
         * get the error message
         */
        virtual const char * what() const noexcept;

        /**
         * show
         * calls the error handler error reporting function to display the error
         */
        void show();

    private:
        /**
         * the error handler used to show errors to the user
         */
        error m_error_handler;
    };

    class qprocessor {
    public:
        /**
         * The default constructor expects the error handler in case of errors
         */
        qprocessor(error& error_handler);

        /**
         * add_ket
         * add a new ket to the processor registers.
         * returns the start and end index of the ket in the register.
         */
        std::pair<qpp::idx, qpp::idx> add_ket(std::shared_ptr<literal_expression> lit_expr, qpp::ket l_ket, qpp::idx length);

        /**
         * apply
         * applies a unitary gate to kets from the given starting index to the given ending index
         */
        void apply(qpp::cmat& unitary, qpp::idx start_index, qpp::idx end_index);

        /**
         * capply
         * applies a controlled unitary gate to the target ket given the control index
         */
        void capply(qpp::cmat& unitary, qpp::idx control_index, qpp::idx target_index);

        /**
         * measure
         * measure the qubit(s) at the specified indices then returns a vector of measurement results for each index
         */
        std::vector<qpp::idx> measure(qpp::idx start_index, qpp::idx end_index);

        /**
         * qprocessing_error
         * contructs and returns a qprocessor_error exception
         */
        qprocessor_error qprocessing_error(const std::string& message);

    private:
        /*
         * the error handler to call in case of an error
         */
        error m_error_handler;

        /*
         * a super ket with all the possible kets we need
         */
        qpp::ket m_ket;

        /*
         * the length of the ket - not the size of the corresponding vector
         */
        std::size_t m_length;

        /*
         * vector of literals containing the kets of interest
         */
        std::vector<std::shared_ptr<literal_expression> > m_literals;
    };
}

#endif
