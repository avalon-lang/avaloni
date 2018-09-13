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

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>

#include "utils/exceptions/file_not_found.hpp"
#include "scanner/scanner.hpp"


namespace avalon {
scanner::scanner(const std::string& source_path) : m_source_path(source_path) {        
}
    
    /**
     * scan
     * this function reads the content of a file and returns it (via return or via argument)
     * throws a "file_not_found" exception if the file to scan was not found.
     */
    std::string scanner::scan() {
        std::ifstream source_file(m_source_path.c_str(), std::ios::in | std::ios::ate);

        // make sure the file is available for reading
        // we can't do this with boost because a file is only available for reading when we actually open it
        if (!source_file.good()) {
            throw file_not_found("Failed to open source file <" + m_source_path + ">. Does the file exist?");
        }

        // read the entire source file content into the source string
        // 1. get the file size and position cursor at the beginning
        std::ifstream::pos_type file_size = source_file.tellg();
        source_file.seekg(0, std::ios::beg);

        // 2. read the file into a char (one byte) vector
        std::vector<char> bytes(file_size);
        source_file.read(&bytes[0], file_size);

        // 3. put the bytes read into the string
        std::string source(&bytes[0], file_size);

        // 4. return the source string
        return source;
    }
}
