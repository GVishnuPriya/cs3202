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

#include <list>
#include <exception>
#include "impl/parser/token.h"
#include "impl/parser/tokenizer.h"
#include "simple/solver.h"
#include "simple/predicate.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::parser;

class MockTokenizer : public SimpleTokenizer {
  public:
    MockTokenizer() : 
        _tokens(), _first_token(true)
    { }
    
    void insert(SimpleToken *token) {
        _tokens.push_back(std::unique_ptr<SimpleToken>(token));

        if(_first_token) {
            _first_token = false;
            _token_it = _tokens.begin();
        }
    }

    SimpleToken* next_token() {
        if(_token_it == _tokens.end()) {
            throw std::exception();
        } else {
            return (_token_it++)->get();
        }
    }

    virtual ~MockTokenizer() { }

  private:
    std::list< std::unique_ptr<SimpleToken> > _tokens;

    std::list< std::unique_ptr<SimpleToken> >::iterator
    _token_it;

    bool _first_token;
};

class MockSolver : public QuerySolver {
  public:
    MockSolver() { }

    ConditionSet solve_left(SimpleCondition *right) {
        return ConditionSet();
    }

    ConditionSet solve_right(SimpleCondition *left) {
        return ConditionSet();
    }

    bool validate(SimpleCondition *left, SimpleCondition *right) {
        return false;
    }
};

class MockPredicate : public SimplePredicate {
  public:
    MockPredicate() : _conditions() { }

    const ConditionSet& global_set() {
        return _conditions;
    }

    void filter(ConditionSet& conditions) {
        // no-op
    }

    bool validate(ConditionPtr condition) {
        return false;
    }

    std::string get_predicate_name() {
        return "dummy";
    }
  private:
    ConditionSet _conditions;
};



}
}
