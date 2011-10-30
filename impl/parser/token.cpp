
#include "impl/parser/token.h"

namespace simple {
namespace parser {

TokenType OpenBraceToken::type("OpenBraceToken");
TokenType CloseBraceToken::type("OpenBraceToken");
TokenType OpenBracketToken::type("OpenBracketToken");
TokenType CloseBracketToken::type("CloseBracketToken");
TokenType SemiColonToken::type("SemiColonToken");
TokenType EqualToken::type("EqualToken");
TokenType EOFToken::type("EOFToken");
TokenType NewLineToken::type("NewLineToken");
TokenType OperatorToken::type("OperatorToken");
TokenType IntegerToken::type("IntegerToken");
TokenType IdentifierToken::type("IdentifierToken");

} // namespace parser
} // namespace simple
