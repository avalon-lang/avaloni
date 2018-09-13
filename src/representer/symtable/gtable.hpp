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

#ifndef AVALON_SYMTABLE_GTABLE_HPP_
#define AVALON_SYMTABLE_GTABLE_HPP_

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <queue>

#include "representer/ast/program.hpp"


namespace avalon {
    class gtable {
    public:
        /**
         * the constructor expects nothing
         */
        gtable();

        /**
         * add_program
         * given a program, add it to the global table along with the fqn where it can be found
         */
        void add_program(program& prog);

        /**
         * get_program
         * given an fqn name, return the program within it
         */
        program& get_program(const std::string& fqn_name);

        /**
         * get_programs
         * returns an unordered map of all programs available in this table
         */
        std::unordered_map<std::string, program>& get_programs();

        /**
         * get_builtins
         * returns a vector of builtin programs that are to be imported automatically
         */
        std::vector<std::string>& get_builtins();

        /**
         * program_exists
         * given an fqn name, return true if it contains a program
         */
        bool program_exists(const std::string& fqn_name);

        /**
         * set_main_fqn
         * set the fqn to the program with the main function
         */
        void set_main_fqn(const std::string& fqn_name);

        /**
         * returns true if the program with the main fqn has been set
         */
        bool has_main_fqn();

        /**
         * get_main_program
         * returns the main program if one has been set, else it throws a runtime error
         */
        program& get_main_program();

    private:
        /*
         * map between fqn(fully qualified name) and the programs within them
         */        
        std::unordered_map<std::string, program> m_programs;

        /*
         * a vector of builtin programs that are to be imported automatically
         */
        std::vector<std::string> m_builtins;

        /*
         * fqn to the program with the main function
         */
        std::string m_main_fqn;
    };
}

#endif
