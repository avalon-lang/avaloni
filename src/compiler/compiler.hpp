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

#ifndef AVALON_COMPILER_HPP_
#define AVALON_COMPILER_HPP_

#include <memory>
#include <vector>
#include <string>

/* Error */
#include "error/error.hpp"

/* Program */
#include "representer/symtable/gtable.hpp"
#include "representer/ast/program.hpp"

/* Lexer */
#include "lexer/token.hpp"


namespace avalon {
    namespace fs = boost::filesystem;
    
    class compiler {
    public:
        /**
         * the constructor expects nothing
         */
        compiler();

        /**
         * add_search_paths
         * when compiling, we need to find the location specified by fully qualified names in
         * import declarations.
         * this function allows users to add search paths to search paths global.
         */
        void add_search_path(const std::string& path);
        void add_search_path(const fs::path& p);

        /**
         * scan
         * calls the scanner to get the text to scan
         */
        std::string scan(const std::string& source_path);

        /**
         * lex
         * calls the lexer to tokenize the source code
         */
        std::vector<std::shared_ptr<token> > lex(const std::string& source_path);

        /**
         * parse
         * calls the parser to generate an AST
         */
        program parse(const std::string& source_path);

        /**
         * import
         * calls the importer to parse all dependencies and make sure there are cyclic dependencies
         */
        void import(const std::string& source_path);

        /**
         * check
         * calls the checker to validate the entire program
         */
        void check(const std::string& source_path);

        /**
         * clean
         * calls the cleaner to ensure all declarations are used
         */
        void clean(const std::string& source_path);

        /**
         * interpret
         * calls the interpreter to run the program
         */
        void interpret(const std::string& source_path, std::vector<std::string>& argumemts);

    private:
        /**
         * a vector of possible search paths
         */
        std::vector<std::string> m_search_paths;
    };
}

#endif
