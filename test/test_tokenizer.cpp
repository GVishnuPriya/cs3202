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

#include "gtest/gtest.h"
#include "impl/parser/iterator_tokenizer.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::parser;

typedef std::string::iterator Iterator;

TEST(TokenizerTest, BasicTokenTest) {
    std::string source = "{ } ( ) \n \r\n ; = + - *";

    IteratorTokenizer<Iterator> tokenizer(source.begin(), source.end());

    tokenizer.next_token_as<OpenBraceToken>();
    tokenizer.next_token_as<CloseBraceToken>();
    tokenizer.next_token_as<OpenBracketToken>();
    tokenizer.next_token_as<CloseBracketToken>();
    tokenizer.next_token_as<NewLineToken>();
    tokenizer.next_token_as<NewLineToken>();
    tokenizer.next_token_as<SemiColonToken>();
    tokenizer.next_token_as<EqualToken>();

    EXPECT_EQ(tokenizer.next_token_as<OperatorToken>()->get_op(), '+');
    EXPECT_EQ(tokenizer.next_token_as<OperatorToken>()->get_op(), '-');
    EXPECT_EQ(tokenizer.next_token_as<OperatorToken>()->get_op(), '*');
    
    tokenizer.next_token_as<EOFToken>();
}

TEST(TokenizerTest, ComplexTokenTest) {
    std::string source = "testing a123 35 0 hello";
    IteratorTokenizer<Iterator> tokenizer(source.begin(), source.end());

    EXPECT_EQ(tokenizer.next_token_as<IdentifierToken>()->get_content(), "testing");
    EXPECT_EQ(tokenizer.next_token_as<IdentifierToken>()->get_content(), "a123");
    EXPECT_EQ(tokenizer.next_token_as<IntegerToken>()->get_value(), 35);
    EXPECT_EQ(tokenizer.next_token_as<IntegerToken>()->get_value(), 0);
    EXPECT_EQ(tokenizer.next_token_as<IdentifierToken>()->get_content(), "hello");

    tokenizer.next_token_as<EOFToken>();
}

}
}
