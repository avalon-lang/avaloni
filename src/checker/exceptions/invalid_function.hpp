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

#ifndef AVALON_CHECKER_EXCEPTIONS_INVALID_FUNCTION_HPP_
#define AVALON_CHECKER_EXCEPTIONS_INVALID_FUNCTION_HPP_

#include <stdexcept>

#include "lexer/token.hpp"


namespace avalon {
    class invalid_function : public std::runtime_error {
    public:
        /**
         * the constructor expects the function name token
         * and the message about what exactly went wrong.
         */
        invalid_function(const token& tok, const std::string& message);

        /**
         * what
         * override the what function defined by runtime_error
         */
        virtual const char * what() const noexcept;

        /**
         * get_token
         * returns the token to the function name in the source code
         */
        const token& get_token();

    private:
        /**
         * when a function fails function checking,
         * we need the token to the function name in the source code
         */
        token m_tok;
    };
}

#endif
