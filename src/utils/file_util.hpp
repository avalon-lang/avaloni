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

#ifndef AVALON_UTILS_FILE_HPP_
#define AVALON_UTILS_FILE_HPP_

#include <vector>
#include <string>

#include <boost/filesystem.hpp>


namespace avalon {
    class file_util {
    public:
        /**
         * the constructor expects the possible search paths where to check for files
         */
        file_util(std::vector<std::string>& search_paths);

        /**
         * source_exists
         * given a source path, this function returns true if the path exists in
         * one of the search paths.
         */
        bool source_exists(const std::string& source_path);

        /**
         * get_source_path
         * given a source file, get the absolute path to it.
         * returns a string containing said absolute path.
         * in case the file was not found, throw "file_not_found" exception.
         */
        std::string get_source_path(const std::string& source_path);

    private:
        /**
         * a vector of possible search paths
         */
        std::vector<std::string> m_search_paths;
    };
}

#endif
