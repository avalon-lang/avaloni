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

#ifndef AVALON_BUILTINS_LANG_MAYBE_HPP_
#define AVALON_BUILTINS_LANG_MAYBE_HPP_

#include <memory>

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/decl/type.hpp"
#include "representer/ast/program.hpp"


namespace avalon {
    class avalon_maybe {
    public:
        /**
         * the default constructor expects nothing
         */
        avalon_maybe();

        /**
         * get_type
         * returns the maybe type
         */
        std::shared_ptr<type>& get_type();

        /**
         * get_type_instance
         * returns the maybe type instance
         */
        type_instance get_type_instance();
        type_instance get_type_instance(type_instance param);

        /**
         * get_program
         * returns a program that defines the maybe type and all functions that operate on maybes
         */
        program& get_program();

    private:
        /*
         * the maybe type
         */
        std::shared_ptr<type> m_maybe_type;

        /*
         * the type instance built from the maybe type
         */
        type_instance m_maybe_instance;

        /*
         * the program with maybeean functions and type
         */
        program m_maybe_prog;
    };
}

#endif
