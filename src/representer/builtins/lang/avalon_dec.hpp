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

#ifndef AVALON_BUILTINS_LANG_DEC_HPP_
#define AVALON_BUILTINS_LANG_DEC_HPP_

#include <memory>

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/decl/type.hpp"
#include "representer/ast/program.hpp"


namespace avalon {
    class avalon_dec {
    public:
        /**
         * the default constructor expects nothing
         */
        avalon_dec();

        /**
         * get_type
         * returns the dec type
         */
        std::shared_ptr<type>& get_type();

        /**
         * get_type_instance
         * returns the dec type instance
         */
        type_instance& get_type_instance();

        /**
         * get_program
         * returns a program that defines the dec type and all functions that operate on decs
         */
        program& get_program();

    private:
        /*
         * the dec type
         */
        std::shared_ptr<type> m_dec_type;

        /*
         * the type instance built from the dec type
         */
        type_instance m_dec_instance;

        /*
         * the program with decean functions and type
         */
        program m_dec_prog;
    };
}

#endif
