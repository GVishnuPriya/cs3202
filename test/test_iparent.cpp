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
#include "impl/solvers/iparent.h"
#include "simple/util/ast_utils.h"
#include "impl/ast.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(IParentTest, TrivialTest) {
    /*
     * proc test {
     *   a = 1          // first
     *   while i {      // loop1
     *     if j {       // condition1
     *       b = 2      // assign1
     *       while k {  // loop2
     *         c = 3    // assign2
     *       }
     *     } else {
     *       if l {     // condition2
     *         d = 4    // assign3
     *       } else {
     *         e = 5    // assign4
     *       }
     *     }
     *   }
     *   f = 6          // last
     * }
     */

    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *first = new SimpleAssignmentAst();
    set_proc(first, proc);

    SimpleWhileAst *loop1 = new SimpleWhileAst();
    set_next(first, loop1);

    SimpleIfAst *condition1 = new SimpleIfAst();
    set_while_body(condition1, loop1);

    SimpleAssignmentAst *assign1 = new SimpleAssignmentAst();
    set_then_branch(assign1, condition1);

    SimpleWhileAst *loop2 = new SimpleWhileAst();
    set_next(assign1, loop2);

    SimpleAssignmentAst *assign2 = new SimpleAssignmentAst();
    set_while_body(assign2, loop2);

    SimpleIfAst *condition2 = new SimpleIfAst();
    set_else_branch(condition2, condition1);

    SimpleAssignmentAst *assign3 = new SimpleAssignmentAst();
    set_then_branch(assign3, condition2);

    SimpleAssignmentAst *assign4 = new SimpleAssignmentAst();
    set_else_branch(assign4, condition2);

    SimpleAssignmentAst *last = new SimpleAssignmentAst();
    set_next(loop1, last);

    SimpleRoot root(proc);
    IParentSolver solver(root);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition1, assign1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition1, loop2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition1, assign2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition1, condition2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition1, assign3)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition1, assign4)));

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop2, assign2)));

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition2, assign3)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition2, assign4)));

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop1, condition1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop1, assign1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop1, loop2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop1, assign2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop1, condition2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop1, assign3)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop1, assign4)));

    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(loop1, first)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(loop2, assign1)));


    EXPECT_EQ(solver.solve_left<StatementAst>(condition1), 
                ConditionSet(new SimpleStatementCondition(loop1)));

    ConditionSet parents;
    parents.insert(new SimpleStatementCondition(condition1));
    parents.insert(new SimpleStatementCondition(loop1));
    EXPECT_EQ(solver.solve_left<StatementAst>(assign1), parents);
    EXPECT_EQ(solver.solve_left<StatementAst>(loop2), parents);
    EXPECT_EQ(solver.solve_left<StatementAst>(condition2), parents);

    parents.insert(new SimpleStatementCondition(condition2));
    EXPECT_EQ(solver.solve_left<StatementAst>(assign3), parents);
    EXPECT_EQ(solver.solve_left<StatementAst>(assign4), parents);

    EXPECT_EQ(solver.solve_left<StatementAst>(first), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(last), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(loop1), ConditionSet());

    ConditionSet loop1_child;
    loop1_child.insert(new SimpleStatementCondition(condition1));
    loop1_child.insert(new SimpleStatementCondition(assign1));
    loop1_child.insert(new SimpleStatementCondition(loop2));
    loop1_child.insert(new SimpleStatementCondition(assign2));
    loop1_child.insert(new SimpleStatementCondition(condition2));
    loop1_child.insert(new SimpleStatementCondition(assign3));
    loop1_child.insert(new SimpleStatementCondition(assign4));
    EXPECT_EQ(solver.solve_right<StatementAst>(loop1), loop1_child);


    ConditionSet condition1_child;
    condition1_child.insert(new SimpleStatementCondition(assign1));
    condition1_child.insert(new SimpleStatementCondition(loop2));
    condition1_child.insert(new SimpleStatementCondition(condition2));
    condition1_child.insert(new SimpleStatementCondition(assign2));
    condition1_child.insert(new SimpleStatementCondition(assign3));
    condition1_child.insert(new SimpleStatementCondition(assign4));
    EXPECT_EQ(solver.solve_right<StatementAst>(condition1), condition1_child);

    ConditionSet condition2_child;
    condition2_child.insert(new SimpleStatementCondition(assign3));
    condition2_child.insert(new SimpleStatementCondition(assign4));
    EXPECT_EQ(solver.solve_right<StatementAst>(condition2), condition2_child);

    EXPECT_EQ(solver.solve_right<StatementAst>(first), ConditionSet());
    EXPECT_EQ(solver.solve_right<StatementAst>(last), ConditionSet());
    EXPECT_EQ(solver.solve_right<StatementAst>(assign1), ConditionSet());
    EXPECT_EQ(solver.solve_right<StatementAst>(assign2), ConditionSet());

}


}
}
