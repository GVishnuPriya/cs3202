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

#include <exception>
#include <memory>
#include <string>
#include "simple/solver.h"
#include "simple/matcher.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::impl;

class SimpleQueryMatcher : public QueryMatcher {
  public:
    SimpleQueryMatcher(QuerySolver *solver);

    std::vector<ConditionPair> solve_both(QueryVariable *left, QueryVariable *right);

    void solve_right(SimpleCondition *left, QueryVariable *right);

    void solve_left(QueryVariable *left, SimpleCondition *right);

    bool validate(SimpleCondition *left, SimpleCondition *right);

    void solve_both_diff_qvar(const ConditionSet& left, const ConditionSet& right,
        /* output */    ConditionSet& new_left, ConditionSet& new_right,
                        std::vector<ConditionPair>& result_pairs);

    void solve_both_from_left(const ConditionSet& left, 
        /* output */    ConditionSet& new_left, ConditionSet& new_right,
                        std::vector<ConditionPair>& result_pairs);

    void solve_both_from_right(const ConditionSet& right,
        /* output */    ConditionSet& new_left, ConditionSet& new_right,
                        std::vector<ConditionPair>& result_pairs);

    void solve_both_same_qvar(const ConditionSet& values, ConditionSet& new_values,
                        std::vector<ConditionPair>& result_pairs);

    virtual ~SimpleQueryMatcher();

  private:
    std::unique_ptr<QuerySolver> _solver;
};


}
}
