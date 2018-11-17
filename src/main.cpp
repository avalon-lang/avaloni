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

#include <iostream>
#include <cstdlib>
#include <memory>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>

#include "compiler/compiler.hpp"
#include "utils/exceptions/invalid_directory.hpp"


void compile(const std::string& source_path, std::vector<std::string>& argumemts);


int main(int argc, char * argv[])
{   if (argc < 2) {
        std::cout << "Usage: avalon program [args]" << std::endl;
    }
    else {
        std::string source_path(argv[1]);

        // get ourselves the argumemts given by the user
        std::vector<std::string> argumemts;
        for(int i = 2; i < argc; ++i)
            argumemts.emplace_back(argv[i]);

        // compile the source
        compile(source_path, argumemts);
    }

    return 0;
}

void compile(const std::string& source_path, std::vector<std::string>& argumemts) {
    using namespace avalon;
    namespace fs = boost::filesystem;

    compiler comp;

    /* 1. configure the compiler according to options given */
    // add the current directory to the search path
    comp.add_search_path(fs::current_path());

    // we add the source path directory to the search path
    // we leave it to later stages of the compiler to decide if the file is valid
    fs::path s(source_path);
    if(fs::exists(s) == true && fs::is_directory(s) == false) {
        fs::path source_dir = s.parent_path();
        try {
            comp.add_search_path(source_dir);
        } catch(const invalid_directory& err) {
        }
    }

    // add the avalon home directory to the search path - if it exists
    if(const char* avalon_home = std::getenv("AVALON_HOME")) {
        fs::path p(avalon_home);
        if(fs::exists(p) == true && fs::is_directory(p) == true)
            comp.add_search_path(p);
    }

    /* 2. call the intepreter */
    comp.interpret(source_path, argumemts);
}
