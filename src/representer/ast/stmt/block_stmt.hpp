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

#ifndef AVALON_AST_STMT_BLOCK_HPP_
#define AVALON_AST_STMT_BLOCK_HPP_

#include <memory>
#include <vector>

#include "representer/ast/stmt/stmt.hpp"
#include "representer/ast/decl/decl.hpp"
#include "lexer/token.hpp"


namespace avalon {
    class block_stmt : public stmt {
    public:
        /**
         * the constructor expects nothing
         */
        block_stmt();

        /**
         * copy constructor
         */
        block_stmt(const block_stmt& blc, std::shared_ptr<scope>& parent_scope);

        /**
         * add_declaration
         * add a declaration to this block
         */
        void add_declaration(std::shared_ptr<decl>& declaration);

        /**
         * get_declarations
         * return a vector of declarations that make up this block
         */
        std::vector<std::shared_ptr<decl> >& get_declarations();
        const std::vector<std::shared_ptr<decl> >& get_declarations() const;

        /**
         * is_empty
         * returns true if this block contains no declarations
         */
        bool is_empty();

        /**
         * token
         * returns this statement token
         */
        virtual const token& stmt_token() const {
            return star_tok;
        }

    private:
        /**
         * vector of declarations that form the block
         */
        std::vector<std::shared_ptr<decl> > m_declarations;
    };
}

#endif
