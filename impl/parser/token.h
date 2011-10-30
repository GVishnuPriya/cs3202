
#pragma once

#include <exception>
#include <string>

namespace simple {
namespace parser {

class NoMoreTokenError : public std::exception { };


class InvalidTokenError : public std::exception { 
  public:
    InvalidTokenError() :
        _token("Invalid Token")
    { }
    
    InvalidTokenError(char token) : 
        _token("Invalid Token: ") 
    { 
        _token += token;
    }

    InvalidTokenError(std::string message) :
        _token(message)
    {

    }

    virtual const char* what() const throw() {
        return _token.c_str();
    }

    virtual ~InvalidTokenError() throw() { }
  private:
    std::string _token;

};

class TokenType {
  public:
    TokenType(std::string name) : _name(name) { }

    bool operator ==(const TokenType& other) const {
        return this == &other;
    }

    std::string get_name() {
        return _name;
    }
  private:
    TokenType(const TokenType& other) = delete;

    std::string _name;
};

class SimpleToken { 
  public:
    virtual TokenType& get_type() = 0;
    virtual ~SimpleToken() { }
};

template <typename Token>
Token* token_cast(SimpleToken *token) {
    if(token->get_type() == Token::type) {
        return static_cast<Token*>(token);
    } else {
        std::string message = "Invalid token_cast from ";
        message += token->get_type().get_name();
        message += " to ";
        message += Token::type.get_name();
        throw InvalidTokenError(message);
    }
}

template <typename Token>
Token* try_token(SimpleToken *token) {
    if(token->get_type() == Token::type) {
        return static_cast<Token*>(token);
    } else {
        return NULL;
    }
}

class OpenBraceToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return OpenBraceToken::type;
    }

    static TokenType type;
};

class CloseBraceToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return CloseBraceToken::type;
    }

    static TokenType type;
};

class OpenBracketToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return OpenBracketToken::type;
    }

    static TokenType type;
};

class CloseBracketToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return CloseBracketToken::type;
    }

    static TokenType type;
};


class SemiColonToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return SemiColonToken::type;
    }

    static TokenType type;
};

class EqualToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return EqualToken::type;
    }

    static TokenType type;
};

class EOFToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return EOFToken::type;
    }

    static TokenType type;
};

class NewLineToken : public SimpleToken { 
  public:
    virtual TokenType& get_type() {
        return NewLineToken::type;
    }

    static TokenType type;
};




class OperatorToken : public SimpleToken {
  public:
    OperatorToken(char op) : _op(op) { }

    char get_op() const {
        return _op;
    }

    virtual TokenType& get_type() {
        return OperatorToken::type;
    }

    static TokenType type;
  private:
    char _op;
};

class IntegerToken : public SimpleToken {
  public:
    IntegerToken(int value) : _value(value) { }

    int get_value() const {
        return _value;
    }

    virtual TokenType& get_type() {
        return IntegerToken::type;
    }

    static TokenType type;
  private:
    int _value;
};

class IdentifierToken : public SimpleToken {
  public:
    IdentifierToken(const std::string& content) :
        _content(content)
    { }

    const std::string& get_content() const {
        return _content;
    }

    virtual TokenType& get_type() {
        return IdentifierToken::type;
    }

    static TokenType type;
  private:
    std::string _content;
};

} // namespace parser
} // namespace simple
