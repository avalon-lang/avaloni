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

#ifndef AVALON_ERROR_HPP_
#define AVALON_ERROR_HPP_


namespace avalon {
    class error {
    public:
        /**
         * the error expects the source file path that contains the error
         */
        error(const std::string& source_path);

        /**
         * set_source_path
         * updates the source path as compilation units can change
         * this function is mainly used by the parser to update the source path before it passes
         * control to a new parsing unit when doing imports.
         */
        void set_source_path(const std::string& source_path);

        /**
         * get_source_path
         * returns the current source path
         */
        std::string& get_source_path();

        /**
         * log
         * this function expects the details including the source paht where the error occured and
         * the message from the unit that raised the error. Since tokens do carry the source path, we use the same here.
         */
        void log(const std::string& source_path, const int line, const int column, const std::string& message);
        void log(const std::string& source_path, const int line, const int column, const std::string& message) const;

        /**
         * log
         * this function expects the details of where the error occured and
         * the message from the unit that raised the error.
         */
        void log(const int line, const int column, const std::string& message);
        void log(const int line, const int column, const std::string& message) const;

        /**
         * log
         * this function expects the message to show.
         * the function is called by units that encounter an error before working on the file to compile
         */
        void log(const std::string& message);
        void log(const std::string& message) const;

    private:
        /**
         * the path to the source file that contains the error
         */
        std::string m_source_path;
    };
}

#endif
