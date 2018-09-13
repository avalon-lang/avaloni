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

#ifndef AVALON_SYMTABLE_FQN_HPP_
#define AVALON_SYMTABLE_FQN_HPP_


namespace avalon {
    class fqn {
    public:
        /**
         * fqn
         * default constructor expects nothing
         */
        fqn();

        /**
         * parametrized constructor expects both the name and path equivalent of the fully qualified name
         */
        fqn(const std::string& name, const std::string& path);

        /* from_name
         * returns an instance of a fully qualified name when given the qualified name.
         * it validates the name before setting it.
         * validation involves resolving the path that correponds to the name and
         * verifying that the path exists and is available for reading.
         */
        static fqn from_name(const std::string& name);

        /**
         * from_path
         * returns an instance of a fully qualified name when given a path to the source.
         * it also verify that the path exists and is available for reading.
         */
        static fqn from_path(const std::string& path);

        /**
         * get_fqn
         * returns the fqn name
         */
        std::string get_name();
        std::string get_name() const;

        /**
         * get_path
         * returns a valid path derived from the fqn
         */
        std::string get_path();
        std::string get_path() const;

        /**
         * is_builtin
         * returns true if this FQN is built into the compiler, hence doesn't not correspond to a filesystem path
         */
        bool is_builtin();
        bool is_builtin() const;

    private:
        /**
         * a string representation of a fqn
         */
        std::string m_name;

        /**
         * the path equivalent of a fqn
         */
        std::string m_path;
    };
    
    /**
     * operator==
     * compares two fqns for equality
     */
    inline bool operator==(const fqn& lhs, const fqn& rhs) {
        if (lhs.get_name() == rhs.get_name())
            return true;
        else
            return false;
    }
    
    /**
     * operator!=
     * compares two fqns for inequality
     */
    inline bool operator!=(const fqn& lhs, const fqn& rhs) {
        return !(lhs == rhs);
    }
}

#endif
