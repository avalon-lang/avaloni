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

#ifndef AVALON_LEXER_HPP_
#define AVALON_LEXER_HPP_

#include <utility>
#include <memory>
#include <vector>
#include <map>
#include <stack>

#include "error/error.hpp"
#include "lexer/token.hpp"
#include "lexer/token_type.hpp"


namespace avalon {
    class lex_error : public std::runtime_error {
    public:
        /**
         * the lex_error expects:
         * - the error handler
         * - the line where the error occured
         * - the column of the token that is erroneous
         * - whether the error is fatal (this will result in the lexer to stop)
         * - the message to show
         */
        lex_error(error& error_handler, std::size_t line, std::size_t column, bool fatal, const std::string& message);

        /**
         * what
         * return a C string containing the message
         */
        virtual const char * what() const noexcept;

        /**
         * show
         * print the error on stdin
         */
        void show();

        /**
         * is_fatal
         * returns a boolean whether this error fatal
         */
        bool is_fatal();

    private:
        /**
         * the error handler in charge displaying error messages
         */
        error m_error_handler;

        /**
         * the line where the error occured
         */
        std::size_t m_line;

        /**
         * the column where the error occured
         */
        std::size_t m_column;

        /**
         * whether the error is fatal
         */
        bool m_fatal;
    };


    class lexer {
    public:
        /**
         * the lexer expects:
         * - a string with the path to source file
         * - a string with the source text
         * - an error handler to call in case of an error
         */
        lexer(const std::string& source_path, const std::string& source, error& error_handler);

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
        std::vector<std::shared_ptr<token> >& lex();

    private:
        /**
         * m_source_path
         * location of the source file we are lexing
         */
        std::string m_source_path;

        /**
         * m_source
         * the source text to tokenize
         */
        std::string m_source;

        /**
         * m_error_handler
         * a reference to the error handler
         */
        error m_error_handler;

        /**
         * m_tokens
         * a vector of the tokens that make up the source file
         */
        std::vector<std::shared_ptr<token> > m_tokens;

        /**
         * m_keywords
         * a map that links each reserved (key)word to the kind of token it is.
         * this map allows us to dinstinguish between an identifier and a reserved word.
         */
        std::map<std::string, token_type> m_keywords;

        /**
         * source motion state
         * these variables keep track of where we are in the source
         *
         * m_start      : indicates the start of the current lexeme
         * m_current    : indicates where we currently are in the source
         * m_line       : indicates we are currently lexing
         * m_column     : indicates how far we are from the start of the current line
         */
        std::size_t m_start;
        std::size_t m_current;
        std::size_t m_line;
        std::size_t m_column;

        /**
         * indentation tracking
         * the variables that follow allow us to properly keep track of
         * how well indented the source code is and if we should ignore any found indentation.
         *
         *
         * m_indentation_stack
         * keeps track of how deeply nested the current line(block) is from the start of the line.
         */
        std::stack<std::size_t> m_indentation_stack;

        /**
         * since indentation is ignored within parenthesis, braces and brackets;
         * we must keep track of how deep within said braces we are so we can know
         * when to re-enable indentation.
         *
         * m_parens_levels      : how deep we are within parentheses
         * m_braces_levels      : how deep we are within curly braces
         * m_brackets_levels    : how deep we are within square braces
         */
        std::size_t m_parens_levels;
        std::size_t m_braces_levels;
        std::size_t m_brackets_levels;

        /**
         * we dinstinguish between logical lines and physical lines.
         * a line that ends with a backlash (and not within a character or a string)
         * is considered to signal the end of a phyical line but to be part of a single
         * logical line that starts on the next physical line.
         *
         * so we must know whether we have a new logical line in order to know if
         * a new line is significant.
         * 
         * m_is_new_logical_line    : a flag that indicates whether we have a new logical line
         * m_in_new_logical_line    : a flag that indicates wheter we are currently lexing a single
                                     logical like that may be spread over multiple physical lines.
         * m_ignore_new_line        : when we meet a new line, this flag is set to true if we intend
                                      to mark the coming line as part of a single logical line
         */
         bool m_is_new_logical_line;
         bool m_in_new_logical_line;
         bool m_ignore_new_line;

         /**
          * we do not allow indentation to at the very beginning of the source code.
          * we also require that all indentations found in the source file be a multiple of
          * the very first indentation found.
          * also, since spaces or tabs can be used for indentation, we require that only one of them be used
          * throughout a source.
          *
          * m_is_first_indentation          : signals whether we have our first indentation.
          * m_is_first_indentation_space    : signals whehter the first indentation found is a space or a tab.
          * m_indentation_space_count       : tabs are transformed into logical spaces.
                                              spaces remain as such. either way, we must know the number of empty spaces
                                              used at the first indentation so we can enforce the multiplicity rule.
         * m_indentation_space_count_line   : for error reporting purposes, we keep track of the line where the first
                                              indentation was found. this is so we can let users know how many spaces (tabs)
                                              their first indentation is made of.
         */
         bool m_is_first_indentation;
         bool m_is_first_indentation_space;
         std::size_t m_indentation_space_count;
         std::size_t m_indentation_space_count_line;

         /**
          * tokenize
          * this function is called repeatedly to tokenize the next stream of text.
          */
         void tokenize();

         /**
          * single_comment
          * this function ignores a single comment and doesn't yield control back until
          * it reaches the end of the line.
          */
         void single_comment();

         /**
          * multi_comment
          * this function doesn't yiel control until it closes a multiline comment.
          * it also takes care to allow comments to nest.
          */
        void multi_comment();

        /**
         * whitespace
         * handles the presence of whitespace especially deciding if the whitespace creates an indentation
         */
        void whitespace();

        /**
         * tabulation
         * handles the presence of tabs, decicing if a tab creates an indentation
         */
        void tabulation();

        /**
         * lex_string
         * this function recognizes a unicode string.
         * at the moment, we only support the ASCII part of the BMP.
         */
        void lex_string();

        /**
         * lex_number
         * this function recognizes an integral number or a decimal number.
         * it relies on a series of other functions in order to properly tokenize
         * the number into it's different components.
         *
         * a number is of the form:
         *
         * <integral_part>.<decimal_part><type>
         */
        void lex_number();

        /**
         * identifier
         * this functions allows us to determine whether we have an identifier or a keyword.
         */
        void identifier();

        /**
         * is_digit
         * determines whether a character is a digit (0-9)
         */
        bool is_digit(char ch);

        /**
         * is_number
         * determines whether a character is a valid digit in one of
         * the bases we support (2, 8, 10 and 16)
         */
        bool is_number(char ch);

        /**
         * is_alpha
         * determines whether we have a non-digital character
         */
        bool is_alpha(char ch);

        /**
         * is_alpha_numeric
         * determines whether the character we have is a digit or any other character
         */
        bool is_alpha_numeric(char ch);

        /**
         * advance
         * consumes the current character by increasing the current character pointer
         * and returning the consumed (current) character.
         */
        char advance();

        /**
         * advance
         * consumes multiple characters at once and returns the consumed character.
         */
        char advance(std::size_t how_far);

        /**
         * match
         * determines if the current character matches the one we expect
         */
        bool match(char expected);

        /**
         * peek
         * looks one character ahead of the current character and returns it.
         * if it determines that we are at the end of the file, it returns '\0'.
         */
        char peek();

        /**
         * peek
         * performs an arbitrary look ahead.
         * returns '\0' if we are at the end of the file.
         */
        char peek(std::size_t how_far);

        /**
         * peek_next
         * looks two characters ahread of the current character and returns it.
         * if it determines that we are at the end of the file, it returns '\0'.
         */
        char peek_next();

        /**
         * peek_previous
         * looks one character back and return the character at that position.
         * if it determines that we are at the beginning of the file, it return '\0'.
         */
        char peek_previous();

        /**
         * is_at_end
         * checks if the the current character is the last one in the source
         */
        bool is_at_end();

        /**
         * is_past_end
         * checks if the character after the current one is the last one in the source
         */
        bool is_past_end();

        /**
         * add_token
         * given a token type, create the corresponding token and add it to the vector of tokens
         */
        void add_token(token_type tok_type);

        /**
         * add_token
         * given a number token, cast it to the token superclass and add it to the vector of tokens
         */
        void add_token(std::shared_ptr<number> tok);

        /**
         * add_token
         * given a string token, cast it to the token superclass and add it to the vector of tokens
         */
        void add_token(std::shared_ptr<avl_string> tok);

        /**
         * error
         * returns a "lex_error" exception instance
         */
        lex_error lexing_error(bool fatal, const std::string& message);
    };
}

#endif
