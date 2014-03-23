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

#include "simple/ast.h"
#include "impl/ast.h"
#include "impl/parse_error.h"
#include "impl/parser/token.h"
#include "impl/parser/tokenizer.h"

namespace simple {
namespace parser {

using namespace simple::impl;
using namespace simple;

class ExprParser {
  public:
    ExprParser(std::shared_ptr<SimpleTokenizer> tokenizer);

    ExprAst* parse_expr();
    
  protected:
    ExprAst* parse_binary_op_rhs(int precedence, ExprAst* lhs);

    ExprAst* parse_primary();

    SimpleConstAst* parse_const();

    SimpleVariableAst* parse_variable();

    ExprAst* parse_parent_expr();

    int operator_precedence();

    template <typename Token>
    Token* current_token_as() {
        return token_cast<Token>(current_token());
    }

    template <typename Token>
    Token* next_token_as() {
        return token_cast<Token>(next_token());
    }

    template <typename Token>
    bool current_token_is() {
        if(try_token<Token>(current_token()) != NULL) {
            return true;
        } else {
            return false;
        }
    }

    SimpleToken* next_token();

    SimpleToken* current_token();

  private:
    SimpleToken* _current_token;

    std::shared_ptr<SimpleTokenizer> _tokenizer;
};

} // namespace parser
} // namespace simple