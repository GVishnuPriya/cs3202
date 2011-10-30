
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
        while(current_char() == ' ') {
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

        }

        if(isalpha(current_char())) {
            std::string value;
            value += current_char();
            while(isalnum(next_char())) {
                value += current_char();
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

        throw InvalidTokenError(current_char());
    }

    char current_char() {
        return _current_char;
    }

    char next_char() {
        if(_it == _end) {
            return EOF;
        }
        return _current_char = *++_it;
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
    EqualToken          _equal_token;
    EOFToken            _eof_token;
    NewLineToken        _new_line_token;
    OperatorToken       _plus_token;
    OperatorToken       _minus_token;
    OperatorToken       _multiply_token;

    std::unique_ptr<SimpleToken> _current_token;
};



}
}
