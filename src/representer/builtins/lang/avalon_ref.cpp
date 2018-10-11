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

/* Lexer */
#include "lexer/token.hpp"

/* AST */
#include "representer/ast/decl/type.hpp"

/* Builtins */
#include "representer/builtins/lang/avalon_ref.hpp"


namespace avalon {
    /**
     * the default constructor expects the dependent type instance parameter
     */
    avalon_ref::avalon_ref() : m_ref_type(std::make_shared<type>(ref_type_tok, VALID)), m_ref_instance(ref_type_tok, m_ref_type, "*") {
        // add the type parameter
        m_ref_type -> add_param(type_param_a_tok);

        // set the category of the type instance
        m_ref_instance.set_category(USER);
        m_ref_instance.is_parametrized(true);
        m_ref_instance.is_reference(true);
    }

    /**
     * get_type
     * returns the ref type
     */
    std::shared_ptr<type>& avalon_ref::get_type() {
        return m_ref_type;
    }

    /**
     * get_type_instance
     * returns the ref type instance
     */
    type_instance avalon_ref::get_type_instance() {
        return m_ref_instance;
    }

    type_instance avalon_ref::get_type_instance(type_instance new_param) {
        type_instance ref_instance(m_ref_instance);
        ref_instance.is_parametrized(new_param.is_parametrized());
        new_param.set_old_token(type_param_a_tok);
        ref_instance.add_param(new_param);
        return ref_instance;
    }
}
