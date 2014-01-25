/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdlib.h>
#include <memory>
#include "impl/parser/tokenizer.h"

namespace simple {
namespace parser {

template <typename Iterator>
class IteratorTokenizer : public SimpleTokenizer {
  public:
    IteratorTokenizer(Iterator begin, Iterator end) :
        _it(begin), _end(end), _eos_reached(false),
        _plus_token('+'), _minus_token('-'), _multiply_token('*')
    { 
        _current_char = *_it;
    }

    template <typename Token>
    Token* next_token_as() {
        return token_cast<Token>(next_token());
    }

    SimpleToken* next_token() {
        while(current_char() == ' ' || current_char() == '\t') {
            next_char();
        }

        switch(current_char()) {
            case EOF:
            case '\0':
                next_char();
                return &_eof_token;

            break;
            case '\n':
                next_char();
                return &_new_line_token;

            break;
            case '\r':
                if(next_char() == '\n') {
                    next_char();
                }
                return &_new_line_token;

            break;
            case '{':
                next_char();
                return &_open_brace_token;

            break;
            case '}':
                next_char();
                return &_close_brace_token;

            break;
            case '(':
                next_char();
                return &_open_bracket_token;

            break;
            case ')':
                next_char();
                return &_close_bracket_token;

            break;
            case ';':
                next_char();
                return &_semi_colon_token;

            break;
            case ',':
                next_char();
                return &_comma_token;

            break;
            case '.':
                next_char();
                return &_dot_token;

            case '=':
                next_char();
                return &_equal_token;

            break;
            case '+':
                next_char();
                return &_plus_token;

            break;
            case '-':
                next_char();
                return &_minus_token;

            break;
            case '*':
                next_char();
                return &_multiply_token;
            break;
            case '"':
                std::string value;
                while(next_char() != '\"') {
                    value += current_char();
                }
                next_char(); // eat closing '"'
                return set_current_token(new LiteralToken(std::move(value)));
            break;

            break;

        }

        if(isalpha(current_char()) || current_char() == '_') {
            std::string value;
            value += current_char();
            next_char();
            while(isalnum(current_char()) || current_char() == '_') {
                value += current_char();
                next_char();
            }
            
            if(value == "_") {
                return &_wild_card_token;
            }

            return set_current_token(new IdentifierToken(std::move(value)));
        }

        if(isdigit(current_char())) {
            std::string value;
            value += current_char();
            while(isdigit(next_char())) {
                value += current_char();
            }
            return set_current_token(new IntegerToken(atoi(value.c_str())));
        }

        throw ParseError("Invalid token \"" + std::to_string(current_char()) + "\"");
    }

    char current_char() {
        return _current_char;
    }

    char next_char() {
        if(_it == _end) {
            return EOF;
        }

        if(++_it == _end) {
            return _current_char = EOF;
        } else {
            return _current_char = *_it;
        }
    }

    SimpleToken* set_current_token(SimpleToken *token) {
        _current_token.reset(token);
        return _current_token.get();
    }

    ~IteratorTokenizer() { }
  private:
    Iterator _it;
    Iterator _end;

    char _current_char;
    bool _eos_reached;

    OpenBraceToken      _open_brace_token;
    CloseBraceToken     _close_brace_token;
    OpenBracketToken    _open_bracket_token;
    CloseBracketToken   _close_bracket_token;
    SemiColonToken      _semi_colon_token;
    CommaToken          _comma_token;
    DotToken            _dot_token;
    EqualToken          _equal_token;
    EOFToken            _eof_token;
    NewLineToken        _new_line_token;
    WildCardToken       _wild_card_token;
    OperatorToken       _plus_token;
    OperatorToken       _minus_token;
    OperatorToken       _multiply_token;

    std::unique_ptr<SimpleToken> _current_token;
};



}
}
