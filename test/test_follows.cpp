
#include "gtest/gtest.h"
#include "simple/util/ast_utils.h"
#include "impl/ast.h"
#include "impl/solvers/follows.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(FollowsTest, TrivialTest) {
    /*
     * proc test {
     *   a = 0;
     *   if i {
     *     x = 1;
     *     y = 2;
     *   } else {
     *     z = 3;
     *   }
     *   b = 3;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");

    SimpleVariable var_a("a");
    SimpleVariable var_b("b");
    SimpleVariable var_x("x");
    SimpleVariable var_y("y");
    SimpleVariable var_z("z");
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

    SimpleAssignmentAst *else_branch = new SimpleAssignmentAst();
    else_branch->set_variable(var_z);
    else_branch->set_expr(new SimpleConstAst(3));
    set_else_branch(else_branch, condition);

    SimpleAssignmentAst *after = new SimpleAssignmentAst();
    after->set_variable(var_b);
    after->set_expr(new SimpleConstAst(4));
    set_next(condition, after);

    SimpleRoot root(proc);
    FollowSolver solver(root);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, condition)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(then_branch, follow_then)));

    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, then_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, else_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(else_branch, after)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(before, after)));
    EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc, proc)));

    EXPECT_EQ(solver.solve_right<StatementAst>(before), 
            ConditionSet(new SimpleStatementCondition(condition)));
    EXPECT_EQ(solver.solve_right<StatementAst>(condition), 
            ConditionSet(new SimpleStatementCondition(after)));
    EXPECT_EQ(solver.solve_right<StatementAst>(then_branch), 
            ConditionSet(new SimpleStatementCondition(follow_then)));

    EXPECT_EQ(solver.solve_right<StatementAst>(after), ConditionSet());
    EXPECT_EQ(solver.solve_right<StatementAst>(follow_then), ConditionSet());
    EXPECT_EQ(solver.solve_right<StatementAst>(else_branch), ConditionSet());
    EXPECT_EQ(solver.solve_right<ProcAst>(proc), ConditionSet());
    EXPECT_EQ(solver.solve_right<SimpleVariable>(&var_x), ConditionSet());

    EXPECT_EQ(solver.solve_left<StatementAst>(condition), 
            ConditionSet(new SimpleStatementCondition(before)));
    EXPECT_EQ(solver.solve_left<StatementAst>(follow_then), 
            ConditionSet(new SimpleStatementCondition(then_branch)));
    EXPECT_EQ(solver.solve_left<StatementAst>(after), 
            ConditionSet(new SimpleStatementCondition(condition)));

    EXPECT_EQ(solver.solve_left<StatementAst>(before), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(then_branch), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(else_branch), ConditionSet());
}

}
}
