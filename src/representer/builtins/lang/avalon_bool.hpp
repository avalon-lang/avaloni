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

#ifndef AVALON_BUILTINS_LANG_BOOL_HPP_
#define AVALON_BUILTINS_LANG_BOOL_HPP_

#include <memory>

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/decl/type.hpp"
#include "representer/ast/program.hpp"


namespace avalon {
    class avalon_bool {
    public:
        /**
         * the default constructor expects nothing
         */
        avalon_bool();

        /**
         * get_type
         * returns the bool type
         */
        std::shared_ptr<type>& get_type();

        /**
         * get_type_instance
         * returns the bool type instance
         */
        type_instance& get_type_instance();

        /**
         * get_program
         * returns a program that defines the bool type and all functions that operate on bools
         */
        program& get_program();

    private:
        /*
         * the bool type
         */
        std::shared_ptr<type> m_bool_type;

        /*
         * the type instance built from the bool type
         */
        type_instance m_bool_instance;

        /*
         * the program with boolean functions and type
         */
        program m_bool_prog;
    };
}

#endif
