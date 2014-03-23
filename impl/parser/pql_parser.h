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

#include <string>
#include <stdexcept>
#include "simple/predicate.h"
#include "simple/query.h"
#include "simple/solver.h"
#include "impl/parse_error.h"
#include "impl/condition.h"
#include "impl/parser/tokenizer.h"
#include "impl/query.h"

namespace simple {
namespace parser {

using namespace simple;
using namespace simple::impl;

class SimplePqlParser {
  public:
    SimplePqlParser(
            std::shared_ptr<SimpleTokenizer> tokenizer, 
            const SimpleRoot& ast,
            const LineTable& line_table,
            const SolverTable& solver_table,
            const PredicateTable& pred_table);

    PqlQuerySet& get_query_set();

    PqlQuerySet parse_query();

    void parse_predicate();

    void parse_main_query();

    std::shared_ptr<PqlSelector> parse_selector();

    ClausePtr parse_clause();

    PqlTerm* parse_term();
    PqlTerm* parse_with_term();

    ConditionPtr parse_condition(const std::string& name);

    void parse_pattern();

    ClausePtr parse_with();
    
    std::shared_ptr<PqlSelector> parse_tuple_selector();

    template <typename Token>
    Token* current_token_as() {
        return token_cast<Token>(current_token());
    }

  protected:
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

    std::string current_token_as_keyword();

    SimpleToken* next_token();

    SimpleToken* current_token();

    PredicatePtr get_predicate(const std::string& name);

    StatementAst* get_statement(int line);

  private:
    std::shared_ptr<SimpleTokenizer> _tokenizer;

    SimpleRoot      _ast;
    LineTable       _line_table;
    SolverTable     _solver_table;
    PredicateTable  _pred_table;
    PqlQuerySet     _query_set;

    SimpleToken     *_current_token;
};



}
}
