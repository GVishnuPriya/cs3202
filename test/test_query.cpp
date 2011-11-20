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
#include <algorithm>
#include "gtest/gtest.h"
#include "simple/solver.h"
#include "simple/query.h"
#include "test/mock.h"
#include "impl/condition.h"
#include "impl/query.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;

TEST(QueryTest, ClausePtrOrdering) {
    std::shared_ptr<QuerySolver> solver(new MockSolver());

    ConditionPtr cond_x(new SimpleVariableCondition(SimpleVariable("x")));
    ConditionPtr cond_y(new SimpleVariableCondition(SimpleVariable("y")));

    ClausePtr clause1(new SimplePqlClause(solver,
                new SimplePqlConditionTerm(cond_x),
                new SimplePqlConditionTerm(cond_y)));
    ClausePtr clause2(new SimplePqlClause(solver,
                new SimplePqlConditionTerm(cond_x),
                new SimplePqlWildcardTerm()));
    ClausePtr clause3(new SimplePqlClause(solver,
                new SimplePqlConditionTerm(cond_x),
                new SimplePqlVariableTerm("s")));
    ClausePtr clause4(new SimplePqlClause(solver,
                new SimplePqlWildcardTerm(),
                new SimplePqlConditionTerm(cond_x)));
    ClausePtr clause5(new SimplePqlClause(solver,
                new SimplePqlWildcardTerm(),
                new SimplePqlWildcardTerm()));
    ClausePtr clause6(new SimplePqlClause(solver,
                new SimplePqlWildcardTerm(),
                new SimplePqlVariableTerm("s")));
    ClausePtr clause7(new SimplePqlClause(solver,
                new SimplePqlVariableTerm("s"),
                new SimplePqlConditionTerm(cond_x)));
    ClausePtr clause8(new SimplePqlClause(solver,
                new SimplePqlVariableTerm("s"),
                new SimplePqlWildcardTerm()));
    ClausePtr clause9(new SimplePqlClause(solver,
                new SimplePqlVariableTerm("s"),
                new SimplePqlVariableTerm("p")));

    EXPECT_TRUE(clause1 < clause2);
    EXPECT_TRUE(clause2 < clause3);
    EXPECT_TRUE(clause3 < clause4);
    EXPECT_TRUE(clause4 < clause5);
    EXPECT_TRUE(clause5 < clause6);
    EXPECT_TRUE(clause6 < clause7);
    EXPECT_TRUE(clause7 < clause8);
    EXPECT_TRUE(clause8 < clause9);

    EXPECT_TRUE(clause1 < clause3);
    EXPECT_TRUE(clause1 < clause4);
    EXPECT_TRUE(clause1 < clause5);
    EXPECT_TRUE(clause1 < clause6);
    EXPECT_TRUE(clause1 < clause7);
    EXPECT_TRUE(clause1 < clause8);
    EXPECT_TRUE(clause1 < clause9);

    EXPECT_TRUE(clause2 < clause4);
    EXPECT_TRUE(clause2 < clause5);
    EXPECT_TRUE(clause2 < clause6);
    EXPECT_TRUE(clause2 < clause7);
    EXPECT_TRUE(clause2 < clause8);
    EXPECT_TRUE(clause2 < clause9);

    EXPECT_TRUE(clause3 < clause5);
    EXPECT_TRUE(clause3 < clause6);
    EXPECT_TRUE(clause3 < clause7);
    EXPECT_TRUE(clause3 < clause8);
    EXPECT_TRUE(clause3 < clause9);

    EXPECT_TRUE(clause4 < clause6);
    EXPECT_TRUE(clause4 < clause7);
    EXPECT_TRUE(clause4 < clause8);
    EXPECT_TRUE(clause4 < clause9);

    EXPECT_TRUE(clause5 < clause7);
    EXPECT_TRUE(clause5 < clause8);
    EXPECT_TRUE(clause5 < clause9);

    EXPECT_TRUE(clause6 < clause8);
    EXPECT_TRUE(clause6 < clause9);

    EXPECT_TRUE(clause7 < clause9);

    ClauseSet set;
    std::vector<ClausePtr> list;

    set.insert(clause9);
    set.insert(clause8);
    set.insert(clause4);
    set.insert(clause6);
    set.insert(clause5);
    set.insert(clause7);
    set.insert(clause3);
    set.insert(clause2);
    set.insert(clause1);

    list.push_back(clause1);
    list.push_back(clause2);
    list.push_back(clause3);
    list.push_back(clause4);
    list.push_back(clause5);
    list.push_back(clause6);
    list.push_back(clause7);
    list.push_back(clause8);
    list.push_back(clause9);

    ClauseSet::iterator it1 = set.begin();
    std::vector<ClausePtr>::iterator it2 = list.begin();

    while(it1 != set.end() && it2 != list.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}


}
}
