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

#include "impl/parser/token.h"

namespace simple {
namespace parser {

TokenType OpenBraceToken::type("OpenBraceToken");
TokenType CloseBraceToken::type("OpenBraceToken");
TokenType OpenBracketToken::type("OpenBracketToken");
TokenType CloseBracketToken::type("CloseBracketToken");
TokenType SemiColonToken::type("SemiColonToken");
TokenType CommaToken::type("CommaToken");
TokenType DotToken::type("DotToken");
TokenType EqualToken::type("EqualToken");
TokenType EOFToken::type("EOFToken");
TokenType NewLineToken::type("NewLineToken");
TokenType WildCardToken::type("WildCardToken");
TokenType OperatorToken::type("OperatorToken");
TokenType IntegerToken::type("IntegerToken");
TokenType IdentifierToken::type("IdentifierToken");
TokenType LiteralToken::type("LiteralToken");

} // namespace parser
} // namespace simple
