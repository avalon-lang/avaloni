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
#include <string>

#include "representer/ast/stmt/block_stmt.hpp"
#include "representer/ast/decl/function.hpp"
#include "representer/symtable/scope.hpp"

/* Exceptions */
#include "checker/exceptions/invalid_expression.hpp"
#include "checker/exceptions/invalid_statement.hpp"
#include "checker/exceptions/invalid_variable.hpp"
#include "checker/exceptions/invalid_block.hpp"

/* Checker */
#include "checker/decl/function/body_checker.hpp"
#include "checker/stmt/block_checker.hpp"


namespace avalon {
    /**
     * check_body
     * this function ensures that the function's body is valid
     */
    void body_checker::check_body(function& function_decl, std::shared_ptr<scope>& l_scope, const std::string& ns_name) {
        block_stmt& body = function_decl.get_body();
        type_instance& ret_instance = function_decl.get_return_type_instance();

        // checking the function is the same as checking the block statement that make the body
        block_checker checker(ret_instance);
        try {
            checker.check(body, l_scope, ns_name);
        } catch(invalid_variable err) {
            // if a variable declaration fails checking inside the block
            throw err;
        } catch(invalid_block err) {
            // if the block contains anything other that variable and statement declarations
            throw err;
        } catch(invalid_expression err) {
            // if any expression sucks
            throw err;
        } catch(invalid_statement err) {
            throw err;
        }
    }
}
