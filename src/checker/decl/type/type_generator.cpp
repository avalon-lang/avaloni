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
#include <string>

/* AST */
#include "representer/ast/decl/type.hpp"

/* Lexer */
#include "lexer/token.hpp"

/* Checker */
#include "checker/decl/type/type_checker.hpp"

/* Generator */
#include "checker/decl/type/type_generator.hpp"

/* Exceptions */
#include "checker/exceptions/invalid_type.hpp"


namespace avalon {
    static default_constructor generate_constructor(default_constructor& def_cons, type_instance& instance, std::shared_ptr<type>& new_type) {
        // we get the data we need from the old constructor
        token tok = def_cons.get_token();
        std::vector<type_instance>& cons_params = def_cons.get_params();
        std::vector<type_instance>& ins_params = instance.get_params();
        
        // we create the new constructor
        default_constructor new_cons(tok, new_type);        

        // we add parameters if any, replacing type constraints with new type instances from the one we have
        for(auto& cons_param : cons_params) {
            if(cons_param.is_abstract()) {
                for(auto& ins_param : ins_params) {
                    if(ins_param.get_old_token() == cons_param.get_token()) {
                        new_cons.add_param(ins_param);
                    }
                }
            }
            else {
                new_cons.add_param(cons_param);
            }
        }

        // set a new name for the constructor
        std::string new_name = new_cons.get_mangled_name();
        new_cons.set_name(new_name);

        return new_cons;
    }

    static record_constructor generate_constructor(record_constructor& rec_cons, type_instance& instance, std::shared_ptr<type>& new_type) {
        // we get the data we need from the old constructor
        token tok = rec_cons.get_token();
        std::map<token,type_instance>& cons_params = rec_cons.get_params();
        std::vector<type_instance>& ins_params = instance.get_params();
        
        // we create the new constructor
        record_constructor new_cons(tok, new_type);

        // we add parameters if any, replacing type constraints with new type instances from the one we have
        for(auto& cons_param : cons_params) {
            if(cons_param.second.is_abstract()) {
                for(auto& ins_param : ins_params) {
                    if(ins_param.get_old_token() == cons_param.second.get_token()) {
                        new_cons.add_param(cons_param.first, ins_param);
                    }
                }
            }
            else {
                new_cons.add_param(cons_param.first, cons_param.second);
            }
        }

        // set a new name for the constructor
        std::string new_name = new_cons.get_mangled_name();
        new_cons.set_name(new_name);

        return new_cons;
    }

    std::shared_ptr<type> type_generator::generate(type_instance& instance) {
        token tok = instance.get_token();

        // if the type instance is a reference type instance, we generate a type from the reference parameter
        if(instance.is_reference()) {
            std::vector<type_instance>& params = instance.get_params();
            return type_generator::generate(params[0]);
        }

        // first we make sure our type instance is complete
        if(instance.is_complete() == false)
            throw invalid_type(tok, "The type instance <" + mangle_type_instance(instance) + "> must be complete before generating a type declaration from it.");

        // we get the data we need
        std::string new_name = instance.get_mangled_name();
        std::shared_ptr<type>& type_decl = instance.get_type();

        // we create the type
        std::shared_ptr<type> new_type = std::make_shared<type>(tok, VALID);
        new_type -> set_name(new_name);
        new_type -> set_fqn(type_decl -> get_fqn());
        new_type -> set_namespace(type_decl -> get_namespace());
        new_type -> set_builder_instance(instance);

        // we generate new default constructors
        std::vector<default_constructor> def_cons = type_decl -> get_default_constructors();
        for(auto& old_cons : def_cons) {
            default_constructor new_cons = generate_constructor(old_cons, instance, new_type);
            new_type -> add_constructor(new_cons);
        }

        // we generate new record constructors
        std::vector<record_constructor> rec_cons = type_decl -> get_record_constructors();
        for(auto& old_cons : rec_cons) {
            record_constructor new_cons = generate_constructor(old_cons, instance, new_type);
            new_type -> add_constructor(new_cons);
        }

        return new_type;
    }
}
