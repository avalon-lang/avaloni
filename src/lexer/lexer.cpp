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

#include <cstddef>
#include <utility>
#include <memory>
#include <vector>
#include <stack>
#include <map>

#include "error/error.hpp"
#include "lexer/token_type.hpp"
#include "lexer/token.hpp"
#include "lexer/lexer.hpp"


namespace avalon {
/**
 * lex_error
 */

/**
 * the lex_error expects:
 * - the error handler
 * - the line where the error occured
 * - the column of the token that is erroneous
 * - whether the error is fatal (this will result in the lexer to stop)
 * - the message to show
 */
lex_error::lex_error(
    error& error_handler,
    std::size_t line,
    std::size_t column,
    bool fatal,
    const std::string& message
) : std::runtime_error(message), m_error_handler(error_handler), m_line(line), m_column(column), m_fatal(fatal) {
}
    
    /**
     * what
     * return a C string containing the message
     */
    const char * lex_error::what() const noexcept {
        return std::runtime_error::what();
    }

    /**
     * show
     * print the error on stdin
     */
    void lex_error::show() {
        m_error_handler.log(m_line, m_column, what());
    }

    /**
     * is_fatal
     * returns a boolean whether this error fatal
     */
    bool lex_error::is_fatal() {
        return m_fatal;
    }


/**
 * lexer
 */
/**
 * the lexer expects:
 * - a string with the source text
 * - an error handler to call in case of an error
 */
lexer::lexer(
    const std::string& source_path,
    const std::string& source,
    error& error_handler
) : m_source_path(source_path),
    m_source(source),
    m_error_handler(error_handler),
    m_start(0),
    m_current(0),
    m_line(1),
    m_column(1),
    m_parens_levels(0),
    m_braces_levels(0),
    m_brackets_levels(0),
    m_is_new_logical_line(false),
    m_in_new_logical_line(false),
    m_ignore_new_line(false),
    m_is_first_indentation(false),
    m_is_first_indentation_space(false),
    m_indentation_space_count(0),
    m_indentation_space_count_line(0)
{
    // uber declarations
    m_keywords["import"]       = IMPORT;
    m_keywords["namespace"]    = NAMESPACE;

    // declaration vibility modifiers
    m_keywords["public"]       = PUBLIC;
    m_keywords["private"]      = PRIVATE;

    // type declaration
    m_keywords["type"]         = TYPE;

    // reference and derefence
    m_keywords["ref"]          = REF;
    m_keywords["dref"]         = DREF;

    // function declaration
    m_keywords["def"]          = FUNCTION;

    // variable declaration
    m_keywords["var"]          = VAR;
    m_keywords["val"]          = VAL;

    // cast operator
    m_keywords["cast"]         = CAST,

    // control flow
    m_keywords["switch"]       = SWITCH;
    m_keywords["case"]         = CASE;
    m_keywords["default"]      = DEFAULT;
    m_keywords["if"]           = IF;
    m_keywords["elif"]         = ELIF;
    m_keywords["else"]         = ELSE;
    m_keywords["for"]          = FOR;
    m_keywords["empty"]        = EMPTY;
    m_keywords["while"]        = WHILE;
    m_keywords["continue"]     = CONTINUE;
    m_keywords["break"]        = BREAK;
    m_keywords["pass"]         = PASS;
    m_keywords["lsh"]          = LEFT_SHIFT;
    m_keywords["rsh"]          = RIGHT_SHIFT;
    m_keywords["and"]          = LOGICAL_AND;
    m_keywords["band"]         = BITWISE_AND;
    m_keywords["or"]           = LOGICAL_OR;
    m_keywords["bor"]          = BITWISE_OR;
    m_keywords["xor"]          = BITWISE_XOR;
    m_keywords["not"]          = LOGICAL_NOT;
    m_keywords["in"]           = IN;
    m_keywords["not in"]       = NOT_IN;
    m_keywords["is"]           = IS;
    m_keywords["is not"]       = IS_NOT;
    m_keywords["next in"]      = NEXT_IN;
    m_keywords["prev in"]      = PREV_IN;

    // the rest
    m_keywords["return"]       = RETURN;
}
    
    /**
     * lex
     * this function is the function called by the parser to get tokens
     * it returns a vector of tokens to be consumed by the parser
     * throws a "lex_error" exception if the lexer failed:
     * - an unrecognized lexeme
     * - invalid indentation/dedentation
     * - incomplete lexeme
     * - etc.
     */
    std::vector<std::shared_ptr<token> >& lexer::lex() {
        // we push zero into the indentation stack
        // this is a control value that tells us if we have the right indentation/dedentation sequences
        m_indentation_stack.push(0);

        while (!is_at_end()) {
            // We are at the beginning of the next lexeme.
            m_start = m_current;

            try {
                tokenize();
            } catch(lex_error err) {
                if(err.is_fatal())
                    throw err;
                else
                    err.show();
            }
        }

        // we expect the last token before EOF to be a new line
        // if it is not present, we add it
        if(m_tokens.empty() == false) {
            std::shared_ptr<token>& last_tok = m_tokens.at(m_tokens.size() - 1);
            if (last_tok -> get_type() != NEWLINE)
                add_token(NEWLINE);
        }

        // we emit DEDENT tokens for the indentations found on the stack after we've exhausted all tokens
        while(m_indentation_stack.top() > 0) {
            m_indentation_stack.pop();
            add_token(DEDENT);
        }

        std::shared_ptr<token> avl_eof = std::make_shared<token>(AVL_EOF, "AVL_EOF", m_line, m_column, m_source_path);
        m_tokens.push_back(avl_eof);
        return m_tokens;
    }

    /**
     * tokenize
     * this function is called repeatedly to tokenize the next stream of text.
     */
    void lexer::tokenize() {
        char c = advance();

        switch(c) {
            case '.': add_token(DOT); break;
            case '!':
                if(match('=')) {
                    add_token(NOT_EQUAL);
                }
                else {
                    if(m_is_new_logical_line) {
                        m_is_new_logical_line = false;
                        
                        while(m_indentation_stack.top() > 0) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                    }

                    add_token(LOGICAL_NOT);
                    m_in_new_logical_line = true;
                }
                break;
            case '~':
                if(m_is_new_logical_line) {
                    m_is_new_logical_line = false;
                    
                    while(m_indentation_stack.top() > 0) {
                        m_indentation_stack.pop();
                        add_token(DEDENT);
                    }
                }

                add_token(BITWISE_NOT);
                m_in_new_logical_line = true;
                break;
            case '^': add_token(BITWISE_XOR); break;
            case '+':
                if(m_is_new_logical_line) {
                    m_is_new_logical_line = false;
                    
                    while(m_indentation_stack.top() > 0) {
                        m_indentation_stack.pop();
                        add_token(DEDENT);
                    }
                }
                
                add_token(PLUS);
                m_in_new_logical_line = true;
                break;
            case '*':
                if(match('*')) {
                    add_token(POW);
                }
                else {
                    add_token(MUL);
                }
                break;
            case '/': add_token(DIV); break;
            case '%': add_token(MOD); break;
            case '\'': add_token(QUOTE); break;
            case ',': add_token(COMMA); break;
            case ':': add_token(COLON); break;
            case '=':
                if(match('=')) {
                    if(match('='))
                        add_token(MATCH);
                    else
                        add_token(EQUAL_EQUAL);
                }
                else if(match('!')) {
                    if(match('='))
                        add_token(NOT_MATCH);
                    else
                        throw lexing_error(true, "Expected a not match operator.");
                }
                else {
                    add_token(EQUAL);
                }
                break;
            case '<':
                if(match('=')) {
                    add_token(LESS_EQUAL);
                }
                else if(match('<')) {
                    add_token(LEFT_SHIFT);
                }
                else if(match('>')) {
                    add_token(NOT_EQUAL);
                }
                else {
                    add_token(LESS);
                }
                break;
            case '>':
                if(match('=')) {
                    add_token(GREATER_EQUAL);
                }
                else if(match('>')) {
                    add_token(RIGHT_SHIFT);
                }
                else {
                    add_token(GREATER);
                }
                break;
            case '-':
                if(match('-')) { // single line comment
                    single_comment();
                }
                else if(match('[')) { // multiple lines comment
                    multi_comment();
                }
                else if(match('>')) {
                    add_token(RETURN_TYPE);
                }
                else if(match('(')) {
                    add_token(NS_LEFT_PAREN);
                }
                else {
                    if(m_is_new_logical_line) {
                        m_is_new_logical_line = false;
                        
                        while(m_indentation_stack.top() > 0) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                    }
                    
                    add_token(MINUS);
                    m_in_new_logical_line = true;
                }
                break;
            case '|':
                if(match('|')) {
                    add_token(LOGICAL_OR);
                }
                else {
                    add_token(VERTICAL_BAR); // same as BITWISE_OR
                }
                break;
            case '_': {
                char next_char = peek();
                if(is_alpha_numeric(next_char)) {
                    if(m_is_new_logical_line) {
                        m_is_new_logical_line = false;

                        while(m_indentation_stack.top() > 0) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                    }

                    identifier();
                    m_in_new_logical_line = true;
                }
                else {
                    add_token(UNDERSCORE);
                }                
                break;
            }
            case '&':
                if(match('&')) {
                    add_token(LOGICAL_AND);
                }
                else {
                    add_token(BITWISE_AND);
                }
                break;                
            case '(':
                add_token(LEFT_PAREN);
                m_parens_levels++;
                break;
            case ')':
                if(match('-')) {
                    if(m_is_new_logical_line) {
                        m_is_new_logical_line = false;
                        
                        while(m_indentation_stack.top() > 0) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                    }

                    add_token(NS_RIGHT_PAREN);
                    m_in_new_logical_line = true;                    
                }
                else {
                    if(m_parens_levels > 0)
                        m_parens_levels--;
                    else
                        throw lexing_error(true, "Dangling closing parenthesis. No corresponing opening parenthesis was found for it.");
                    add_token(RIGHT_PAREN);
                }                
                break;
            case '{':
                add_token(LEFT_BRACE);
                m_braces_levels++;
                break;
            case '}':
                if(m_braces_levels > 0)
                    m_braces_levels--;
                else
                    throw lexing_error(true, "Dangling closing brace. No corresponing opening brace was found for it.");

                add_token(RIGHT_BRACE);
                break;
            case '[':
                add_token(LEFT_BRACKET);
                m_brackets_levels++;
                break;
            case ']':
                if(m_brackets_levels > 0)
                    m_brackets_levels--;
                else
                    throw lexing_error(true, "Dangling closing bracket. No corresponing opening bracket was found for it.");

                add_token(RIGHT_BRACKET);
                break;
            case '"':
                if(m_is_new_logical_line) {
                    m_is_new_logical_line = false;

                    while(m_indentation_stack.top() > 0) {
                        m_indentation_stack.pop();
                        add_token(DEDENT);
                    }
                }

                lex_string();
                m_in_new_logical_line = true;
            case ' ':
                whitespace();
                break;
            case '\r': // we ignore carriage returns and white space
                break;
            case '\t':
                tabulation();
                break;
            case '\\':
                // we ignore every character that comes after the backslash (??? shoul we inform the user that this is not fine?)
                if(peek() != '\n') {
                    //Avalon.error(line, "Expected a new line after the backslash character.");
                    while(peek() != '\n') advance();
                }
                
                m_ignore_new_line = true;
                break;
            case '\n':
                // if we are inside parenthesis or braces or brackets, we don't emit a NEWLINE token
                if((m_parens_levels > 0) | (m_braces_levels > 0) | (m_brackets_levels > 0)) {
                    m_line++;
                    m_column = 1;
                    break;
                }

                // we have a backslash before the new line, we also don't emit a NEWLINE token
                if(m_ignore_new_line) {
                    m_ignore_new_line = false;
                    m_line++;
                    break;
                }

                // we only acknowledge a logical new line if we are inside one
                if(m_in_new_logical_line) {
                    add_token(NEWLINE);
                    m_is_new_logical_line = true;
                    m_in_new_logical_line = false; // we reset this flag as we've consumed it
                }
                m_column = 1;
                m_line++;
                break;
            default:
                if (is_digit(c)) {
                    if(m_is_new_logical_line) {
                        m_is_new_logical_line = false;
                        
                        while(m_indentation_stack.top() > 0) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                    }
                    
                    lex_number();
                    m_in_new_logical_line = true;
                }
                else if (is_alpha(c)) {
                    if(m_is_new_logical_line) {
                        m_is_new_logical_line = false;

                        while(m_indentation_stack.top() > 0) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                    }

                    identifier();
                    m_in_new_logical_line = true;
                }
                else {
                    throw lexing_error(true, std::string("Unexpected character \"") + c + std::string("\""));
                }
                break;
        }
    }

    /**
     * single_comment
     * this function ignores a single comment and doesn't yield control back until
     * it reaches the end of the line.
     */
    void lexer::single_comment() {
        while(peek() != '\n' && !is_at_end()) advance();
        m_start = m_current;
    }

    /**
     * multi_comment
     * this function doesn't yiel control until it closes a multiline comment.
     * it also takes care to allow comments to nest.
     */
    void lexer::multi_comment() {
        std::size_t nesting_levels = 0;
        bool comment_terminated = false;
        std::size_t current_line = m_line;
        
        while(!is_at_end()) {
            // try to match a new nested comment
            if(peek() == '-' && peek_next() == '[') {
                nesting_levels++;
                advance();
                advance();
            }
            // try to match a closed comment
            else if(peek() == ']' && peek_next() == '-') {
                advance();
                advance();

                if(nesting_levels == 0) { // if we don't have any recorded nested comments, we get out of the loop
                    comment_terminated = true;
                    break;
                }
                else {
                    nesting_levels--;
                }
            }
            else {
                // increase line count when we get a new line
                if(peek() == '\n') {
                    m_column = 1;
                    m_line++;
                }

                advance();
            }
        }

        m_start = m_current;

        // if we reached the end of the file without closing a comment, show the user a nice error error message
        if (is_at_end() && comment_terminated == false)
            throw lexing_error(true, std::string("Unterminated comment starting at line ") + std::to_string(current_line) + std::string("."));
    }

    /**
     * whitespace
     * handles the presence of whitespace especially deciding if the whitespace creates an indentation
     */
    void lexer::whitespace() {
        // we have a new logical line
        if(m_is_new_logical_line) {
            // we check if tabs are not already in use as the indentation marker
            // if it is, we have an error
            if(m_is_first_indentation == true && m_is_first_indentation_space == false) {
                throw lexing_error(true, "Unexpected blank space: a tabulation is already in use as indentation marker.");
                // we go till the the first non white space character and continue lexing
                while(peek() == ' ') advance();
                return;
            }
            // so blank spaces are used as indentation
            else {
                std::size_t spaces_found = 1; // if we are here then a blank space was found alreay
                while(peek() == ' ') {
                    spaces_found++;
                    advance();
                }

                // we are here so we have 4 possibilities
                // 1. we got a tab: this is an error since tabs and spaces cannot be mixed
                if(match('\t'))
                    throw lexing_error(true, "Unexpected tabulation. Cannot mix tabulations and blank spaces as indentation markers.");

                // 2. we got a new line or carriage return: in this case we ignore this entire line
                if(peek() == '\n')
                    return;

                if(peek() == '\r')
                    return;

                // 3. if we got a comment coming up, we leave
                if(peek() == '-') {
                    if(peek_next() == '-') {
                        return;
                    }
                    else if(peek_next() == '/') {
                        return;
                    }
                }

                // 4. we probably got a token
                // we check if we should emit a INDENT or a DENDENT or do nothing
                std::size_t top = m_indentation_stack.top();

                // we clear the new line flag
                m_is_new_logical_line = false;
                
                // if there is nothing on the top of the stack
                // then this is the very first indentation
                // we emit an INDENT token, push the current blank spaces count on the indentation stack and mark this as first indentation
                if(top == 0) {
                    m_is_first_indentation = true;
                    m_is_first_indentation_space = true;
                    m_indentation_space_count = spaces_found;
                    m_indentation_space_count_line = m_line;
                    m_indentation_stack.push(1); // we push 1 as a logical tab (we don't push physical blank spaces)
                    add_token(INDENT);
                    return;
                }
                // if the top is not zero, then we have previous indentation
                // then here we have 3 choices:
                // 1. the top of the stack has the exact number of current tabs: we do nothing
                // 2. the top of the stack has a greater number of tabs: we keep popping the top of the stack until we reach a top of equal tabs count, this while emitting DEDENT tokens
                // 3. the top of the stack has a lesser number of tabs: we emit an INDENT token
                else {
                    // first, we set our logical tab
                    // if we got an uneven amount of spaces, we exit with an error
                    std::size_t logical_tab = 0;

                    if(spaces_found > m_indentation_space_count) {
                        if(spaces_found % m_indentation_space_count != 0)
                            throw lexing_error(true, std::string("Unexpected indentation. This line begins with more or less spaces than required. All indentations must be multiples of (") + std::to_string(m_indentation_space_count) + std::string(") as it is the first indentation found on line ") + std::to_string(m_indentation_space_count_line) + std::string("."));
                        else
                            logical_tab = spaces_found / m_indentation_space_count;
                    }
                    else if(spaces_found < m_indentation_space_count) {
                        if(m_indentation_space_count % spaces_found != 0)
                            throw lexing_error(true, std::string("Unexpected indentation. This line begins with more or less spaces than required. All indentations must be multiples of (") + std::to_string(m_indentation_space_count) + std::string(") as it is the first indentation found on line ") + std::to_string(m_indentation_space_count_line) + std::string("."));
                        else
                            logical_tab = m_indentation_space_count / spaces_found;
                    }
                    else {
                        logical_tab = 1; // the only other possibility is that `spaces_found == indentation_space_count`
                    }
                    

                    if(top == logical_tab) {
                        return;
                    }
                    else if(top > logical_tab) {
                        while(m_indentation_stack.top() > logical_tab) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                        return;
                    }
                    else {
                        // even if logical_tab > top, it must be a 1 increment to be considered valid
                        if (logical_tab - top > 1) {
                            throw lexing_error(true, "Indentation too steep. New indentation must be a 1 increment of the previous indentation.");
                        }
                        else {
                            m_indentation_stack.push(logical_tab);
                            add_token(INDENT);
                            return;
                        }
                    }
                }
            }
        }
        // if we are not at the beginning of a new logical line, we ignore any blank spaces found
        else {
            // we don't allow indentation at the beginning of the file
            if(m_tokens.size() == 0) {
                throw lexing_error(true, "Indentation not allowed at the beginning of the source.");
                // we ignore the rest of the white space after this
                while(peek() == ' ') advance();
                return;
            }
            else {
                return;
            }
        }
    }

    /**
     * tabulation
     * handles the presence of tabs, decicing if a tab creates an indentation
     */
    void lexer::tabulation() {
        // we have a new logical line
        if(m_is_new_logical_line) {
            // we check if one whitespace character is not already in use as the indentation marker
            // if it is, we have an error
            if(m_is_first_indentation == true && m_is_first_indentation_space == true) {
                throw lexing_error(true, "Unexpected tabulation: a blank space is already in use as indentation marker.");
                // we go till the the first non tab character and continue lexing
                while(peek() == '\t') advance();
                return;
            }
            // so tabs are used as indentation
            else {
                // first, we attempt to see if we reach a new line character before encountering any other character
                // we also keep track of how many tabs we found
                std::size_t tabs_found = 1; // if we are here then a tab was found alreay
                while(peek() == '\t') {
                    tabs_found++;
                    advance();
                }

                // we are here so we have 4 possibilities
                // 1. we got a space: this is an error since tabs and spaces cannot be mixed
                if(match(' '))
                    throw lexing_error(true, "Unexpected blank space. Cannot mix tabulations and blank spaces as indentation markers.");

                // 2. we got a new line or carriage return: in this case we ignore this entire line
                // we also return from the switch but we increase the line count before then
                // we don't emit a INDENT token because this is an empty line that contains no statement to indent
                if(peek() == '\n')
                    return;

                if(peek() == '\r')
                    return;

                // 3. if we got a comment coming up, we leave
                if(peek() == '-') {
                    if(peek_next() == '-') {
                        return;
                    }
                    else if(peek_next() == '/') {
                        return;
                    }
                }

                // 4. we probably got a token
                // we check if we should emit a INDENT or a DENDENT or do nothing
                std::size_t top = m_indentation_stack.top();

                // we clear the new line flag
                m_is_new_logical_line = false;
                
                // if there is nothing on the top of the stack
                // then this is the very first indentation
                // we emit an INDENT token, push the current tabs count on the indentation stack and mark this as first indentation
                if(top == 0) {
                    m_is_first_indentation = true;
                    m_is_first_indentation_space = false;
                    m_indentation_stack.push(tabs_found);
                    add_token(INDENT);
                    return;
                }
                // if the top is not zero, then we have previous indentation
                // then here we have 3 choices:
                // 1. the top of the stack has the exact number of current tabs: we do nothing
                // 2. the top of the stack has a greater number of tabs: we keep popping the top of the stack until we reach a top of equal tabs count, this while emitting DEDENT tokens
                // 3. the top of the stack has a lesser number of tabs: we emit an INDENT token
                else {
                    if(top == tabs_found) {
                        return;
                    }
                    else if(top > tabs_found) {
                        while(m_indentation_stack.top() > tabs_found) {
                            m_indentation_stack.pop();
                            add_token(DEDENT);
                        }
                        return;
                    }
                    else if(top < tabs_found) {
                        // we require that new tabs be single increments of the previous tab
                        if (tabs_found - top > 1) {
                            throw lexing_error(true, "Indentation too steep. New indentation must be a single increment of the previous identation.");
                        }
                        else {
                            m_indentation_stack.push(tabs_found);
                            add_token(INDENT);
                            return;
                        }
                    }
                }
            }
        }
        // if we are not at the beginning of a new logical line, we ignore any tabs found
        else {
            // we don't allow indentation at the beginning of the file
            if(m_tokens.size() == 0) {
                throw lexing_error(true, "Indentation not allowed at the beginning of the source.");
                // we go till the the first non tab character and continue lexing
                while(peek() == '\t') advance();
                return;
            }
            else {
                return;
            }
        }
    }

    /**
     * lex_string
     * this function recognizes a unicode string.
     * at the moment, we only support the ASCII part of the BMP.
     */
    void lexer::lex_string() {
        // read the string
        char prev_char = peek();
        while((peek() != '"' || (peek() == '"' && prev_char == '\\')) && !is_at_end()) {
            prev_char = advance();
            if(prev_char == '\n')
                throw lexing_error(true, "New lines are not allowed inside strings.");
        }

        if(is_at_end())
            throw lexing_error(true, "Unterminated string.");

        // make sure we close with a double quote
        if(peek() != '"')
            throw lexing_error(true, "Expected a double quotation mark to close a string.");
        else
            advance();

        // trim surrounding quotes and set the lexeme as char content
        std::string lexeme = m_source.substr(m_start + 1, m_current - m_start - 2);
        std::shared_ptr<avl_string> str = std::make_shared<avl_string>(STRING, lexeme, m_line, m_column, m_source_path);
        add_token(str);
    }

    /**
     * lex_number
     * this function recognizes an integral number or a decimal number.
     * it relies on a series of other functions in order to properly tokenize
     * the number into it's different components.
     *
     * a number is of the form:
     *
     * 0<base><integral_part>.<decimal_part><type>
     */
    void lexer::lex_number() {
        int lexeme_start = m_start;        
        std::string integral_part = "";
        std::string decimal_part = "";
        std::string data_type = "";
        std::shared_ptr<number> num = std::make_shared<number>(INTEGER, "", m_line, m_column, m_source_path);

        // we first try to see if we have a base specified
        if(is_number(peek_previous()) && is_alpha(peek())) {
            // we make sure that the first character is `0`
            if(peek_previous() != '0')
                throw lexing_error(true, "Expected <0> before specifying the base.");

            // update the base
            if(peek() == 'b')
                num -> update_type(BITS);
            else if(peek() == 'q')
                num -> update_type(QUBITS);
            else
                throw lexing_error(true, "Only bit and qubit 'bases' are supported at the moment.");

            // consume the base
            advance();

            // reset the start the current lexeme
            m_start = m_current;
        }

        while(is_number(peek()) || peek() == '\'') advance();
        integral_part += m_source.substr(m_start, m_current - m_start);
        num -> set_integral(integral_part);
        m_start = m_current;

        // if we encounter a dot, then we have a floating point number but only if the current number type is INTEGER
        if(peek() == '.' && num -> get_type() == INTEGER) {
            // we consume the dot
            advance();
            ++m_start;
            
            // we make sure we have at least one number following up
            if(!is_number(peek()))
                throw lexing_error(true, "Expected at least one digit in the decimal part.");

            // get the decimal part
            while(is_number(peek()) || peek() == '\'') advance();
            decimal_part += m_source.substr(m_start, m_current - m_start);
            num -> set_decimal(decimal_part);
            m_start = m_current; // update m_start in anticipation of the real number type

            // parse the type
            if(is_alpha(peek())) {
                advance();
                // we now get the number type specifier
                data_type += m_source.substr(m_start, m_current - m_start);
                num -> set_data_type(data_type);
                m_start = m_current;
                if(data_type == "f")
                    num -> update_type(FLOATING_POINT);
                else if(data_type == "d")
                    num -> update_type(DECIMAL);
                else
                    throw lexing_error(true, "Unexpected real number type. Expected 'f' (for floats) or 'd' (for decimals).");
            }
            // if we have no type, we let the user know that one must be provided in order to distinguish between floats and decimals
            else {
                throw lexing_error(true, "Expected at real number type. Please choose between float and decimal.");
            }
        }
        else if(peek() == '.' && (num -> get_type() == BITS || num -> get_type() == QUBITS)) {
            throw lexing_error(true, "Unexpected dot <.> when lexing a bit or qubit string.");
        }

        std::string lexeme = m_source.substr(lexeme_start, m_current - lexeme_start);
        num -> update_lexeme(lexeme);
        num -> update_line(m_line);
        num -> update_column(m_column);

        // add the number to the list of tokens
        add_token(num);
    }

    /**
     * identifier
     * this functions allows us to determine whether we have an identifier or a keyword.
     */
    void lexer::identifier() {
        while(is_alpha_numeric(peek())) advance();

        // see if the identifier is a reserved word
        std::string identifier = m_source.substr(m_start, m_current - m_start);
        
        // try to see if we got 'not in' or 'is not' as keyword
        if(identifier == std::string("not")     ||
           identifier == std::string("is")      ||
           identifier == std::string("next")    ||
           identifier == std::string("prev")
        ) {
            std::string next_token = "";
            unsigned int how_far = 1;

            // append the existing identifier to the expected next token
            next_token += identifier;
            next_token += " "; // we add a blank line waiting for 'is' or 'not'

            // skip over all blank spaces
            while(peek() == ' ') advance();

            char next_char;
            do {
                next_char = peek(how_far);
                if(next_char != ' ')
                    next_token += next_char;
                how_far++;
            } while(is_alpha_numeric(next_char));

            // if we get what we need, we stop peeking and advance to the next possible token and update the identifier found
            try {
                m_keywords.at(next_token);
                advance(how_far);
                identifier = next_token;
            } catch(std::out_of_range exc) {
                // do nothing
            }
        }

        token_type type;
        try {
            type = m_keywords.at(identifier);
        } catch(std::out_of_range exc) {
            type = IDENTIFIER;
        }
        add_token(type);
    }

    /**
     * is_digit
     * determines whether a character is a digit (0-9)
     */
    bool lexer::is_digit(char c) {
        return c >= '0' && c <= '9';
    }

    /**
     * is_alpha
     * determines whether we have a non-digital character
     */
    bool lexer::is_alpha(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    /**
     * is_alpha_numeric
     * determines whether the character we have is a digit or any other character
     */
    bool lexer::is_alpha_numeric(char c) {
        return is_alpha(c) || is_digit(c);
    }

    /**
     * is_number
     * determines whether a character is a valid digit in one of
     * the bases we support (2, 8, 10 and 16)
     */
    bool lexer::is_number(char c) {
        return (c >= 'A' && c <= 'F') || is_digit(c);
    }

    /**
     * advance
     * consumes the current character by increasing the current character pointer
     * and returning the consumed (current) character.
     */
    char lexer::advance() {
        m_current++;
        m_column++;

        return m_source.at(m_current - 1);
    }

    /**
     * advance
     * consumes multiple characters at once and returns the consumed character.
     */
    char lexer::advance(std::size_t how_far) {
        m_current = m_current + (how_far - 1);
        m_column = m_column + (how_far - 1);
        return m_source.at(m_current - 1);
    }

    /**
     * match
     * determines if the current character matches the one we expect
     */
    bool lexer::match(char expected) {
        if (is_at_end()) return false;
        if (m_source.at(m_current) != expected) return false;

        m_current++;
        m_column++;
        return true;
    }

    /**
     * peek
     * looks one character ahead of the current character and returns it.
     * if it determines that we are at the end of the file, it returns '\0'.
     */
    char lexer::peek() {
        if (m_current >= m_source.length()) return '\0';
        return m_source.at(m_current);
    }

    /**
     * peek
     * performs an arbitrary look ahead.
     * returns '\0' if we are at the end of the file.
     */
    char lexer::peek(std::size_t how_far) {
        if (m_current + (how_far - 1) >= m_source.length()) return '\0';
        return m_source.at(m_current + (how_far - 1));
    }

    /**
     * peek_next
     * looks two characters ahread of the current character and returns it.
     * if it determines that we are at the end of the file, it returns '\0'.
     */
    char lexer::peek_next() {
        if (m_current + 1 >= m_source.length()) return '\0';
        return m_source.at(m_current + 1);
    }

    /**
     * peek_previous
     * looks one character back and return the character at that position.
     * if it determines that we are at the beginning of the file, it return '\0'.
     */
    char lexer::peek_previous() {
        if (m_current == 0) return '\0';
        return m_source.at(m_current - 1);
    }

    /**
     * is_at_end
     * checks if the the current character is the last one in the source
     */
    bool lexer::is_at_end() {
        return m_current >= m_source.length();
    }

    /**
     * is_past_end
     * checks if the character after the current one is the last one in the source
     */
    bool lexer::is_past_end() {
        return m_current + 1 >= m_source.length();
    }

    /**
     * add_token
     * given a token type, create the corresponding token and add it to the vector of tokens
     */
    void lexer::add_token(token_type tok_type) {
        std::string lexeme = m_source.substr(m_start, m_current - m_start);
        std::shared_ptr<token> tok = std::make_shared<token>(tok_type, lexeme, m_line, m_column, m_source_path);
        m_tokens.push_back(tok);
    }

    /**
     * add_token
     * given a number token, cast it to the token superclass and add it to the vector of tokens
     */
    void lexer::add_token(std::shared_ptr<number> number) {
        std::shared_ptr<token> tok = number;
        m_tokens.push_back(tok);
    }

    /**
     * add_token
     * given string token, cast it to the token superclass and add it to the vector of tokens
     */
    void lexer::add_token(std::shared_ptr<avl_string> str) {
        std::shared_ptr<token> tok = str;
        m_tokens.push_back(tok);
    }

    /**
     * error
     * returns a "lex_error" exception instance
     */
    lex_error lexer::lexing_error(bool fatal, const std::string& message) {
        return lex_error(m_error_handler, m_line, m_column, fatal, message);
    }
}
