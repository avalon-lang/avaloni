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
#include <memory>

/* AST */
#include "representer/ast/decl/statement.hpp"
#include "representer/ast/decl/variable.hpp"
#include "representer/ast/decl/decl.hpp"

/* Symtable */
#include "representer/symtable/scope.hpp"


namespace avalon {
    /**
     * copy
     * copies a declaration and returns the copied one
     */
    std::shared_ptr<decl> decl::copy(std::shared_ptr<scope>& parent_scope) {
        if(this == nullptr)
            return nullptr;

        if(this -> is_variable()) {
            std::shared_ptr<variable> const & var_decl = std::static_pointer_cast<variable>(shared_from_this());
            std::shared_ptr<variable> l_var_decl = std::make_shared<variable>(var_decl);
            std::shared_ptr<decl> new_declaration = l_var_decl;
            return new_declaration;
        }
        else if(this -> is_statement()) {
            std::shared_ptr<statement> const & stmt_decl = std::static_pointer_cast<statement>(shared_from_this());
            std::shared_ptr<statement> l_stmt_decl = std::make_shared<statement>(stmt_decl, parent_scope);
            std::shared_ptr<decl> new_declaration = l_stmt_decl;
            return new_declaration;
        }
        else {
            throw std::runtime_error("[compiler error] attempting to copy a declaration that's neither variable or a statement declaration.");
        }
    }
}
