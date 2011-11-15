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
#include "simple/util/ast_utils.h"
#include "impl/ast.h"
#include "impl/solvers/ifollows.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(IFollowsTest, TrivialTest) {
    /*
     * proc test {
     *   a = 0;
     *   if i {
     *     x = 1;
     *     y = 2;
     *     z = 3;
     *   } else {
     *     p = 4;
     *     q = 5;
     *   }
     *   b = 6;
     *   c = 7
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");

    SimpleVariable var_a("a");
    SimpleVariable var_b("b");
    SimpleVariable var_c("c");
    SimpleVariable var_x("x");
    SimpleVariable var_y("y");
    SimpleVariable var_z("z");
    SimpleVariable var_p("p");
    SimpleVariable var_q("q");
    SimpleVariable var_i("i");

    SimpleAssignmentAst *before = new SimpleAssignmentAst();
    before->set_variable(var_a);
    before->set_expr(new SimpleConstAst(0));
    set_proc(before, proc);

    SimpleConditionalAst *condition = new SimpleConditionalAst();
    condition->set_variable(var_i);
    set_next(before, condition);

    SimpleAssignmentAst *then_branch = new SimpleAssignmentAst();
    then_branch->set_variable(var_x);
    then_branch->set_expr(new SimpleConstAst(1));
    set_then_branch(then_branch, condition);

    SimpleAssignmentAst *follow_then = new SimpleAssignmentAst();
    follow_then->set_variable(var_y);
    follow_then->set_expr(new SimpleConstAst(2));
    set_next(then_branch, follow_then);

    SimpleAssignmentAst *follow_then2 = new SimpleAssignmentAst();
    follow_then->set_variable(var_z);
    follow_then->set_expr(new SimpleConstAst(3));
    set_next(follow_then, follow_then2);

    SimpleAssignmentAst *else_branch = new SimpleAssignmentAst();
    else_branch->set_variable(var_p);
    else_branch->set_expr(new SimpleConstAst(4));
    set_else_branch(else_branch, condition);

    SimpleAssignmentAst *follow_else = new SimpleAssignmentAst();
    follow_else->set_variable(var_q);
    follow_else->set_expr(new SimpleConstAst(5));
    set_next(else_branch, follow_else);

    SimpleAssignmentAst *after = new SimpleAssignmentAst();
    after->set_variable(var_b);
    after->set_expr(new SimpleConstAst(6));
    set_next(condition, after);

    SimpleAssignmentAst *after2 = new SimpleAssignmentAst();
    after->set_variable(var_c);
    after->set_expr(new SimpleConstAst(7));
    set_next(after, after2);

    SimpleRoot root(proc);
    IFollowSolver solver(root);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, condition)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, after2)));

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, after2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(then_branch, follow_then)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(then_branch, follow_then2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(follow_then, follow_then2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(else_branch, follow_else)));

    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, then_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, else_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(else_branch, after)));
    EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc, proc)));


    ConditionSet ifollow_before;
    ifollow_before.insert(new SimpleStatementCondition(condition));
    ifollow_before.insert(new SimpleStatementCondition(after));
    ifollow_before.insert(new SimpleStatementCondition(after2));
    EXPECT_EQ(solver.solve_right<StatementAst>(before), ifollow_before);

    ConditionSet ifollow_condition;
    ifollow_condition.insert(new SimpleStatementCondition(after));
    ifollow_condition.insert(new SimpleStatementCondition(after2));
    EXPECT_EQ(solver.solve_right<StatementAst>(condition), ifollow_condition);

    ConditionSet ifollow_then;
    ifollow_then.insert(new SimpleStatementCondition(follow_then));
    ifollow_then.insert(new SimpleStatementCondition(follow_then2));
    EXPECT_EQ(solver.solve_right<StatementAst>(then_branch), ifollow_then);

    ConditionSet ifollow_else;
    ifollow_else.insert(new SimpleStatementCondition(follow_else));
    EXPECT_EQ(solver.solve_right<StatementAst>(else_branch), ifollow_else);

    ConditionSet ifollow_after;
    ifollow_after.insert(new SimpleStatementCondition(after2));
    EXPECT_EQ(solver.solve_right<StatementAst>(after), ifollow_after);

    ConditionSet ipreceed_after;
    ipreceed_after.insert(new SimpleStatementCondition(after));
    ipreceed_after.insert(new SimpleStatementCondition(before));
    ipreceed_after.insert(new SimpleStatementCondition(condition));
    EXPECT_EQ(solver.solve_left<StatementAst>(after2), ipreceed_after);

    ConditionSet ipreceed_else;
    ipreceed_else.insert(new SimpleStatementCondition(else_branch));
    EXPECT_EQ(solver.solve_left<StatementAst>(follow_else), ipreceed_else);

    ConditionSet ipreceed_then;
    ipreceed_then.insert(new SimpleStatementCondition(then_branch));
    ipreceed_then.insert(new SimpleStatementCondition(follow_then));
    EXPECT_EQ(solver.solve_left<StatementAst>(follow_then2), ipreceed_then);

    ConditionSet ipreceed_condition;
    ipreceed_condition.insert(new SimpleStatementCondition(before));
    EXPECT_EQ(solver.solve_left<StatementAst>(condition), ipreceed_condition);


    EXPECT_EQ(solver.solve_right<StatementAst>(after2), ConditionSet());
    EXPECT_EQ(solver.solve_right<StatementAst>(follow_then2), ConditionSet());
    EXPECT_EQ(solver.solve_right<StatementAst>(follow_else), ConditionSet());
    EXPECT_EQ(solver.solve_right<ProcAst>(proc), ConditionSet());
    EXPECT_EQ(solver.solve_right<SimpleVariable>(&var_x), ConditionSet());

    EXPECT_EQ(solver.solve_left<StatementAst>(before), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(then_branch), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(else_branch), ConditionSet());
}

}
}
