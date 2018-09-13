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

#ifndef AVALON_CHECKER_DECL_TYPE_CHECKER_HPP_
#define AVALON_CHECKER_DECL_TYPE_CHECKER_HPP_

#include <utility>
#include <memory>
#include <string>

#include "representer/symtable/scope.hpp"
#include "representer/ast/decl/type.hpp"


namespace avalon {
    class type_instance_checker {
    public:
        /**
         * simple_check
         * given a type instance, the scope were it was found, the namespace of the object that holds it and a list of possible standins,
         * this function checks to see if that type instance exists in that scope in the given namespace or among the standins.
         */
        static std::pair<bool,bool> simple_check(type_instance& instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& standins);

        /**
         * complex_check
         * given a type instance, the scope were it was found, the namespace of the object that holds it and a list of possible standins,
         * these functions check to see if that type instance has a type builder that live in that scope in one the possible namespaces:
         * - the holder object namespace
         * - the global namespace
         * - the type instance attached namespace
         */
        static std::pair<bool,bool> complex_check(type_instance& instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name, const std::vector<token>& standins);
        static std::pair<bool,bool> complex_check(type_instance& instance, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
    };


    class constructor_checker {
    public:
        /**
         * check
         * given a default or record constructor,
         * this function checks whether they are valid by making sure
         * that all type instances they depend on are built from valid types.
         */
        static void check(default_constructor& def_constructor, std::shared_ptr<type>& type_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
        static void check(record_constructor& rec_constructor, std::shared_ptr<type>& type_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
    
    private:
        /**
         * a default constructor expects nothing
         */
        constructor_checker();        
    };


    class type_checker {
    public:
        /**
         * the default constructor expects nothing
         */
        type_checker();

        /**
         * check
         * - makes sure all type parameters are valid
         * - make sure all constructors are valid
         */
        void check(std::shared_ptr<type>& type_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name);
    };
}

#endif
