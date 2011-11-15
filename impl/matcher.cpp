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

/*
 * Note: Need to be careful in case both left and right are the same query variable
 */
std::vector<ConditionPair> SimpleQueryMatcher::solve_both(QueryVariable *left, QueryVariable *right) {
    std::vector<ConditionPair> pairs;

    /*
     * Both bounded
     */
    if(left->is_bounded() && right->is_bounded()) {
        ConditionSet new_left;
        ConditionSet new_right;

        for(ConditionSet::iterator left_it = left->get_conditions().begin();
                left_it != left->get_conditions().end(); ++left_it)
        {
          for(ConditionSet::iterator right_it = right->get_conditions().begin();
                  right_it != right->get_conditions().end(); ++right_it)
          {
            if(_solver->validate(left_it->get(), right_it->get())) {
              new_left.insert(*left_it);
              new_right.insert(*right_it);
              pairs.push_back(ConditionPair(*left_it, *right_it));
            }
          }
        }
                
        if(left == right) {
            new_left.union_with(new_right);
            left->set_conditions(std::move(new_left));
        } else {
            left->set_conditions(std::move(new_left));
            right->set_conditions(std::move(new_right));
        }

    /*
     * Left bounded, right unbounded
     */
    } else if(left->is_bounded() && !right->is_bounded()) {
        for(ConditionSet::iterator left_it = left->get_conditions().begin();
                left_it != left->get_conditions().end(); ++left_it)
        {
            ConditionSet right_results = _solver->solve_right(left_it->get());
            right->get_conditions().union_with(right_results);

            for(ConditionSet::iterator right_it = right_results.begin();
                    right_it != right_results.end(); ++right_it)
            {
                pairs.push_back(ConditionPair(*left_it, *right_it));
            }
        }

    /*
     * Left unbounded, right bounded
     */
    } else if(!left->is_bounded() && right->is_bounded()) {
        for(ConditionSet::iterator right_it = right->get_conditions().begin();
                right_it != right->get_conditions().end(); ++right_it)
        {
            ConditionSet left_results = _solver->solve_left(right_it->get());
            left->get_conditions().union_with(left_results);

            for(ConditionSet::iterator left_it = left_results.begin();
                    left_it != left_results.end(); ++left_it)
            {
                pairs.push_back(ConditionPair(*left_it, *right_it));
            }
        }

    /*
     * Both unbounded
     */
    } else {
        ConditionSet global_left = left->get_predicate()->global_set();
        ConditionSet new_left;
        ConditionSet new_right;

        for(ConditionSet::iterator left_it = global_left.begin();
                left_it != global_left.end(); ++left_it)
        {
            ConditionSet right_results = _solver->solve_right(left_it->get());
            if(!right_results.is_empty()) {
                new_right.union_with(right_results);
                new_left.insert(*left_it);

                for(ConditionSet::iterator right_it = right_results.begin();
                        right_it != right_results.end(); ++right_it)
                {
                    pairs.push_back(ConditionPair(*left_it, *right_it));
                }
            }
        }

        if(left == right) {
            new_left.union_with(new_right);
            left->set_conditions(std::move(new_left));
        } else {
            left->set_conditions(std::move(new_left));
            right->set_conditions(std::move(new_right));
        }
    }

    return pairs;
}

void SimpleQueryMatcher::solve_right(SimpleCondition *left, QueryVariable *right) {
    if(right->is_bounded()) {
        right->get_conditions().union_with(_solver->solve_right(left));
    } else {
        right->set_conditions(_solver->solve_right(left));
    }
}

void SimpleQueryMatcher::solve_left(QueryVariable *left, SimpleCondition *right) {
    if(left->is_bounded()) {
        left->get_conditions().union_with(_solver->solve_left(right));
    } else {
        left->set_conditions(_solver->solve_left(right));
    }
}

bool SimpleQueryMatcher::validate(SimpleCondition *left, SimpleCondition *right) {
    return _solver->validate(left, right);
}

SimpleQueryMatcher::~SimpleQueryMatcher() { }



}
}
