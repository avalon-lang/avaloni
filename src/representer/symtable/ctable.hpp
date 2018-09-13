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

#ifndef AVALON_SYMTABLE_CTABLE_HPP_
#define AVALON_SYMTABLE_CTABLE_HPP_

#include <unordered_map>
#include <cstddef>
#include <utility>
#include <string>
#include <map>

#include "representer/ast/decl/type.hpp"


namespace avalon {
    enum constructor_type {
        DEFAULT_CONS,
        RECORD_CONS
    };



    class csymbols {
    public:
        /**
         * the constructor expects nothing
         */
        csymbols();

        /**
         * insert_default_constructor
         * insert a new default constructor into this symbol table
         */
        void insert_default_constructor(default_constructor& def_cons);

        /**
         * insert_record_constructor
         * insert a new record constructor into this symbol table
         */
        void insert_record_constructor(record_constructor& rec_cons);

        /**
         * get_default_constructor
         * given a constructor name and arity, return the matching default constructor if it exists
         */
        default_constructor& get_default_constructor(const std::string& name, std::size_t arity);

        /**
         * get_record_constructor
         * given a constructor name and arity, return the matching record constructor if it exists
         */
        record_constructor& get_record_constructor(const std::string& name, std::size_t arity);

        /**
         * default_constructor_exists
         * given a constructor name and its arity, find if an existing default constructor matches the same
         */
        bool default_constructor_exists(const std::string& name, std::size_t arity);

        /**
         * record_constructor_exists
         * given a constructor name and its arity, find if an existing record constructor matches the same
         */
        bool record_constructor_exists(const std::string& name, std::size_t arity);

        /**
         * default_constructor_exists
         * given a constructor name, find if an existing default constructor matches the same
         */
        bool default_constructor_exists(const std::string& name);

        /**
         * record_constructor_exists
         * given a constructor name, find if an existing record constructor matches the same
         */
        bool record_constructor_exists(const std::string& name);

    private:
        /*
         * map of all the constructors in the current scope
         */
        std::map<std::pair<std::string,std::size_t>, default_constructor> m_def_constructors;
        std::map<std::pair<std::string,std::size_t>, record_constructor> m_rec_constructors;
    };

    class ctable {
    public:
        /**
         * the constructor expects nothing
         */
        ctable();

        /**
         * insert_default_constructor
         * insert a new default constructor into this symbol table
         */
        void insert_default_constructor(const std::string& ns_name, default_constructor& def_cons);

        /**
         * insert_record_constructor
         * insert a new record constructor into this symbol table
         */
        void insert_record_constructor(const std::string& ns_name, record_constructor& rec_cons);

        /**
         * get_default_constructor
         * given a constructor name and arity, return the matching default constructor if it exists
         */
        default_constructor& get_default_constructor(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        /**
         * get_record_constructor
         * given a constructor name and arity, return the matching record constructor if it exists
         */
        record_constructor& get_record_constructor(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        /**
         * default_constructor_exists
         * given a constructor name and its arity, find if an existing default constructor matches the same
         */
        bool default_constructor_exists(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        /**
         * record_constructor_exists
         * given a constructor name and its arity, find if an existing record constructor matches the same
         */
        bool record_constructor_exists(const std::string& ns_name, const std::string& cons_name, std::size_t arity);

        /**
         * default_constructor_exists
         * given a constructor name, find if an existing default constructor matches the same
         */
        bool default_constructor_exists(const std::string& ns_name, const std::string& cons_name);

        /**
         * record_constructor_exists
         * given a constructor name, find if an existing record constructor matches the same
         */
        bool record_constructor_exists(const std::string& ns_name, const std::string& cons_name);

    private:
        /*
         * a map between namespaces and their respective symbol tables
         */
        std::unordered_map<std::string, csymbols> m_symbols;
    };
}

#endif
