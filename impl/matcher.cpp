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

#include <vector>
#include "impl/matcher.h"

namespace simple {
namespace impl {


SimpleQueryMatcher::SimpleQueryMatcher(QuerySolver *solver) : _solver(solver) { } 


void SimpleQueryMatcher::solve_both_diff_qvar(
                    const ConditionSet& left, const ConditionSet& right,
    /* output */    ConditionSet& new_left, ConditionSet& new_right,
                    std::vector<ConditionPair>& result_pairs) 
{
    for(ConditionSet::iterator left_it = left.begin(); 
            left_it != left.end(); ++left_it)
    {
        for(ConditionSet::iterator right_it = right.begin();
              right_it != right.end(); ++right_it)
        {
            if(_solver->validate(left_it->get(), right_it->get())) {
                new_left.insert(*left_it);
                new_right.insert(*right_it);
                result_pairs.push_back(ConditionPair(*left_it, *right_it));
            }
        }
    }
}

void SimpleQueryMatcher::solve_both_same_qvar(const ConditionSet& values,
     /* output */   ConditionSet& new_values,
                    std::vector<ConditionPair>& result_pairs)
{
    for(ConditionSet::iterator it = values.begin();
            it != values.end(); ++it)
    {
        if(_solver->validate(it->get(), it->get())) {
            new_values.insert(*it);
            result_pairs.push_back(ConditionPair(*it, *it));
        }
    }
}

std::vector<ConditionPair> SimpleQueryMatcher::solve_both(QueryVariable *left, QueryVariable *right) {
    std::vector<ConditionPair> pairs;

    if(left == right) {
        ConditionSet new_values;
        solve_both_same_qvar(left->get_conditions(), new_values, pairs);

        left->set_conditions(std::move(new_values));
    } else {
        ConditionSet new_left;
        ConditionSet new_right;

        solve_both_diff_qvar(left->get_conditions(), right->get_conditions(),
                new_left, new_right, pairs);

        left->set_conditions(std::move(new_left));
        right->set_conditions(std::move(new_right));
    }

    return pairs;
}

void SimpleQueryMatcher::solve_right(SimpleCondition *left, QueryVariable *right) {
    ConditionSet new_right = right->get_conditions();
    new_right.intersect_with(_solver->solve_right(left));
    right->set_conditions(new_right);
}

void SimpleQueryMatcher::solve_left(QueryVariable *left, SimpleCondition *right) {
    ConditionSet new_left = left->get_conditions();
    new_left.intersect_with(_solver->solve_left(right));
    left->set_conditions(new_left);
}

bool SimpleQueryMatcher::validate(SimpleCondition *left, SimpleCondition *right) {
    return _solver->validate(left, right);
}

SimpleQueryMatcher::~SimpleQueryMatcher() { }



}
}
