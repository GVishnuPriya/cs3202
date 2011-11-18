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

#include <vector>
#include <memory>
#include "simple/qvar.h"
#include "simple/predicate.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

class QueryMatcher {
  public:
    virtual std::vector<ConditionPair> solve_both(QueryVariable *left, QueryVariable *right) = 0;
    virtual void solve_right(SimpleCondition *condition, QueryVariable *var) = 0;
    virtual void solve_left(QueryVariable *var, SimpleCondition *condition) = 0;
    virtual bool validate(SimpleCondition *left, SimpleCondition *right) = 0;

    virtual ~QueryMatcher() { }
};

typedef std::shared_ptr<QueryMatcher>   MatcherPtr;

class MatcherTable {
  public:
    virtual QueryMatcher* get_matcher(const std::string& name) const = 0;
    virtual void insert(const std::string& name, QueryMatcher *matcher) = 0;

    virtual ~MatcherTable() { }
};


}
