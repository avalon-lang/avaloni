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

#include <memory>
#include <vector>

/* Error */
#include "error/error.hpp"

/* Program */
#include "representer/symtable/gtable.hpp"
#include "representer/ast/program.hpp"

/* Scanner */
#include "scanner/scanner.hpp"

/* Lexer */
#include "lexer/token.hpp"
#include "lexer/lexer.hpp"

/* Parser */
#include "parser/parser.hpp"

/* Utils */
#include "utils/exceptions/file_not_found.hpp"
#include "utils/parse_util.hpp"
#include "utils/file_util.hpp"


namespace avalon {
    program parse_util::parse(const std::string& source_path, std::vector<std::string>& search_paths) {
        error error_handler(source_path);
        file_util futil(search_paths);

        // make sure the file given exists and the absolute path to it
        std::string source_abs_path;
        try {
            source_abs_path = futil.get_source_path(source_path);
        } catch(file_not_found err) {
            throw err;
        }

        // scan the file for content
        scanner scr(source_abs_path);
        std::string source;
        try {
            source = scr.scan();
        } catch(file_not_found err) {
            throw err;
        }

        // tokenize the file
        lexer lxr(source_path, source, error_handler);
        try {
            std::vector<std::shared_ptr<token> >& tokens = lxr.lex();
            
            // parse the file
            parser psr(tokens, source_path, error_handler);
            program prog;
            try {
                prog = psr.parse();
                return prog;
            } catch(parse_error err) {
                throw err;
            }
        } catch(lex_error err) {
            throw err;
        }
    }
}
