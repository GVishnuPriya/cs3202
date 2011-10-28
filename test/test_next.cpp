
#include "gtest/gtest.h"
#include "simple/ast.h"
#include "simple/util/ast_utils.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "impl/solvers/next.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(NextTest, BasicTest) {
    /*
     * proc test {
     *   x = 1;
     *   y = x;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");

    SimpleVariable var_x("x");
    SimpleVariable var_y("y");

    SimpleAssignmentAst *stat1 = new SimpleAssignmentAst();
    stat1->set_variable(var_x);
    stat1->set_expr(new SimpleConstAst(1));
    set_proc(stat1, proc);

    SimpleAssignmentAst *stat2 = new SimpleAssignmentAst();
    stat2->set_variable(var_y);
    stat2->set_expr(new SimpleVariableAst(var_x));
    set_next(stat1, stat2);

    SimpleRoot root(proc);

    NextSolver solver(root, NULL);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, stat2)));
}

TEST(NextTest, ConditionalTest) {
    /*
     * proc test {
     *   a = 0;
     *   if i {
     *     x = 1;
     *   } else {
     *     y = 2;
     *   }
     *   b = 3;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");

    SimpleVariable var_a("a");
    SimpleVariable var_b("b");
    SimpleVariable var_x("x");
    SimpleVariable var_y("y");
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

    SimpleAssignmentAst *else_branch = new SimpleAssignmentAst();
    else_branch->set_variable(var_y);
    else_branch->set_expr(new SimpleConstAst(2));
    set_else_branch(else_branch, condition);

    SimpleAssignmentAst *after = new SimpleAssignmentAst();
    after->set_variable(var_b);
    after->set_expr(new SimpleConstAst(4));
    set_next(condition, after);

    SimpleRoot root(proc);
    NextSolver solver(root, NULL);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, condition)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, then_branch)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, else_branch)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(then_branch, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(else_branch, after)));

    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, after)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(before, after)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(then_branch, else_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(else_branch, then_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(before, then_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(before, else_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, before)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, after)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(then_branch, before)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(else_branch, before)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(after, before)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(after, condition)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(after, then_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(after, else_branch)));

    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(before, before)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, condition)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(then_branch, then_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(else_branch, else_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(after, after)));

    ConditionSet before_next;
    before_next.insert(new SimpleStatementCondition(condition));
    EXPECT_EQ(solver.solve_right<StatementAst>(before), before_next);

    ConditionSet condition_next;
    condition_next.insert(new SimpleStatementCondition(then_branch));
    condition_next.insert(new SimpleStatementCondition(else_branch));
    EXPECT_EQ(solver.solve_right<StatementAst>(condition), condition_next);

    ConditionSet branch_next;
    branch_next.insert(new SimpleStatementCondition(after));
    EXPECT_EQ(solver.solve_right<StatementAst>(then_branch), branch_next);
    EXPECT_EQ(solver.solve_right<StatementAst>(else_branch), branch_next);

    EXPECT_EQ(solver.solve_right<StatementAst>(after), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(before), ConditionSet());

    ConditionSet condition_prev(new SimpleStatementCondition(before));
    EXPECT_EQ(solver.solve_left<StatementAst>(condition), condition_prev);

    ConditionSet branch_prev;
    branch_prev.insert(new SimpleStatementCondition(condition));
    EXPECT_EQ(solver.solve_left<StatementAst>(then_branch), branch_prev);
    EXPECT_EQ(solver.solve_left<StatementAst>(else_branch), branch_prev);

    ConditionSet after_previous;
    after_previous.insert(new SimpleStatementCondition(then_branch));
    after_previous.insert(new SimpleStatementCondition(else_branch));
    EXPECT_EQ(solver.solve_left<StatementAst>(after), after_previous);
}

TEST(NextTest, WhileTest) {
    /*
     * proc test {
     *   a = 1;
     *   while i {
     *     x = 2;
     *     y = 3;
     *   }
     *   b = 4;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");

    SimpleVariable var_a("a");
    SimpleVariable var_b("b");
    SimpleVariable var_x("x");
    SimpleVariable var_y("y");
    SimpleVariable var_i("i");

    SimpleAssignmentAst *before = new SimpleAssignmentAst();
    before->set_variable(var_a);
    before->set_expr(new SimpleConstAst(1));
    set_proc(before, proc);

    SimpleWhileAst *loop = new SimpleWhileAst();
    loop->set_variable(var_i);
    set_next(before, loop);

    SimpleAssignmentAst *stat1 = new SimpleAssignmentAst();
    stat1->set_variable(var_x);
    stat1->set_expr(new SimpleConstAst(2));
    set_while_body(stat1, loop);

    SimpleAssignmentAst *stat2 = new SimpleAssignmentAst();
    stat2->set_variable(var_y);
    stat2->set_expr(new SimpleConstAst(3));
    set_next(stat1, stat2);

    SimpleAssignmentAst *after = new SimpleAssignmentAst();
    after->set_variable(var_b);
    after->set_expr(new SimpleConstAst(4));
    set_next(loop, after);

    SimpleRoot root(proc);
    NextSolver solver(root, NULL);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, loop)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop, stat1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, stat2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat2, loop)));

    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(stat2, after)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(before, stat1)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(loop, loop)));

    ConditionSet before_next(new SimpleStatementCondition(loop));
    EXPECT_EQ(solver.solve_right<StatementAst>(before), before_next);

    ConditionSet loop_next;
    loop_next.insert(new SimpleStatementCondition(stat1));
    loop_next.insert(new SimpleStatementCondition(after));
    EXPECT_EQ(solver.solve_right<StatementAst>(loop), loop_next);

    ConditionSet stat1_next(new SimpleStatementCondition(stat2));
    EXPECT_EQ(solver.solve_right<StatementAst>(stat1), stat1_next);

    ConditionSet stat2_next(new SimpleStatementCondition(loop));
    EXPECT_EQ(solver.solve_right<StatementAst>(stat2), stat2_next);

    EXPECT_EQ(solver.solve_right<StatementAst>(after), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(before), ConditionSet());

    ConditionSet loop_prev;
    loop_prev.insert(new SimpleStatementCondition(before));
    loop_prev.insert(new SimpleStatementCondition(stat2));
    EXPECT_EQ(solver.solve_left<StatementAst>(loop), loop_prev); 

    ConditionSet stat1_prev(new SimpleStatementCondition(loop));
    EXPECT_EQ(solver.solve_left<StatementAst>(stat1), stat1_prev); 

    ConditionSet stat2_prev(new SimpleStatementCondition(stat1));
    EXPECT_EQ(solver.solve_left<StatementAst>(stat2), stat2_prev); 

    ConditionSet after_prev(new SimpleStatementCondition(loop));
    EXPECT_EQ(solver.solve_left<StatementAst>(after), after_prev); 
}

} // namespace test
} // namespace simple
