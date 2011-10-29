
#include <exception>
#include <string>

namespace simple {
namespace parser {

class InvalidTokenError : public std::exception { };

class TokenType {
  public:
    operator ==(const TokenType& other) const {
        return this == &other;
    }
  private:
    TokenType(const TokenType& other) = delete;
};

class SimpleToken { 
  public:
    virtual TokenType& get_type() = 0;
};

template <typename Token>
Token* token_cast(SimpleToken *token) {
    if(token->get_type() == Token::type) {
        return static_cast<Token*>(token);
    } else {
        throw InvalidTokenError();
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
