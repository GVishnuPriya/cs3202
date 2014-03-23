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

#include "impl/parser/expr_parser.h"

namespace simple {
namespace parser {

using namespace simple;
using namespace simple::impl;

ExprParser::ExprParser(std::shared_ptr<SimpleTokenizer> tokenizer) :
    _tokenizer(tokenizer)
{ 
    next_token();
}

ExprAst* ExprParser::parse_expr() {
    ExprAst *primary = parse_primary();
    return parse_binary_op_rhs(0, primary);
}

ExprAst* ExprParser::parse_binary_op_rhs(int precedence, ExprAst* lhs) {
    while(true) {
        int current_precedence = operator_precedence();
        if(current_precedence < precedence) {
            return lhs;
        }

        char current_op = current_token_as<OperatorToken>()->get_op();
        next_token();

        ExprAst *rhs = parse_primary();
        int next_precedence = operator_precedence();
        if(current_precedence < next_precedence) {
            rhs = parse_binary_op_rhs(current_precedence+1, rhs);
        }
        lhs = new SimpleBinaryOpAst(current_op, lhs, rhs);
    }
}

ExprAst* ExprParser::parse_primary() {
    SimpleToken *token = current_token();
    if(try_token<IdentifierToken>(token)) {
        return parse_variable();
    } else if(try_token<IntegerToken>(token)) {
        return parse_const();
    } else if(try_token<OpenBracketToken>(token)) {
        return parse_parent_expr();
    } else {
        throw ParseError("Invalid primary expression " + token->get_type().get_name());
    }
}

SimpleConstAst* ExprParser::parse_const() {
    int value = current_token_as<IntegerToken>()->get_value();
    next_token();
    return new SimpleConstAst(value);
}

SimpleVariableAst* ExprParser::parse_variable() {
    std::string name = current_token_as<IdentifierToken>()->get_content();
    next_token();
    return new SimpleVariableAst(name);
}

ExprAst* ExprParser::parse_parent_expr() {
    current_token_as<OpenBracketToken>();
    next_token(); // eat'('

    ExprAst *expr = parse_expr();

    current_token_as<CloseBracketToken>();
    next_token(); // eat ')'

    return expr;
}

int ExprParser::operator_precedence() {
    if(current_token_is<OperatorToken>()) {
        OperatorToken *token = current_token_as<OperatorToken>();
        switch(token->get_op()) {
            case '+':
            case '-':
                return 100;
            break;
            case '*':
                return 200;
            break;
            default:
                return -1; // not supported at the moment
            break;
        }
    } else {
        return -1;
    }
}

SimpleToken* ExprParser::next_token() {
    _current_token = _tokenizer->next_token();
    if(current_token_is<NewLineToken>()) {
        return next_token();
    } else {
        return _current_token;
    }
}

SimpleToken* ExprParser::current_token() {
    return _current_token;
}

}
}