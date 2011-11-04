
#include <iostream>
#include "gtest/gtest.h"
#include "simple/ast.h"
#include "simple/util/ast_utils.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "impl/solvers/next.h"
#include "impl/solvers/inext.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(INextTest, BasicTest) {
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

    NextSolver next_solver(root);
    INextSolver solver(root, &next_solver);

    ConditionSet stat1_next;
    stat1_next.insert(new SimpleStatementCondition(stat2));

    EXPECT_EQ(solver.solve_right<StatementAst>(stat1), stat1_next);
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, stat2)));
}

TEST(INextTest, ConditionalTest) {
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
    before->set_line(1);
    set_proc(before, proc);

    SimpleConditionalAst *condition = new SimpleConditionalAst();
    condition->set_variable(var_i);
    condition->set_line(2);
    set_next(before, condition);

    SimpleAssignmentAst *then_branch = new SimpleAssignmentAst();
    then_branch->set_variable(var_x);
    then_branch->set_expr(new SimpleConstAst(1));
    then_branch->set_line(3);
    set_then_branch(then_branch, condition);

    SimpleAssignmentAst *else_branch = new SimpleAssignmentAst();
    else_branch->set_variable(var_y);
    else_branch->set_expr(new SimpleConstAst(2));
    else_branch->set_line(4);
    set_else_branch(else_branch, condition);

    SimpleAssignmentAst *after = new SimpleAssignmentAst();
    after->set_variable(var_b);
    after->set_expr(new SimpleConstAst(4));
    after->set_line(5);
    set_next(condition, after);

    SimpleRoot root(proc);
    NextSolver next_solver(root);
    INextSolver solver(root, &next_solver);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, condition)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, then_branch)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, else_branch)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, then_branch)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, else_branch)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(condition, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(then_branch, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(else_branch, after)));

    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(then_branch, else_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(else_branch, then_branch)));
    EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(condition, before)));
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
    before_next.insert(new SimpleStatementCondition(then_branch));
    before_next.insert(new SimpleStatementCondition(else_branch));
    before_next.insert(new SimpleStatementCondition(after));
    EXPECT_EQ(solver.solve_right<StatementAst>(before), before_next);

    ConditionSet condition_next;
    condition_next.insert(new SimpleStatementCondition(then_branch));
    condition_next.insert(new SimpleStatementCondition(else_branch));
    condition_next.insert(new SimpleStatementCondition(after));
    EXPECT_EQ(solver.solve_right<StatementAst>(condition), condition_next);

    ConditionSet branch_next;
    branch_next.insert(new SimpleStatementCondition(after));
    EXPECT_EQ(solver.solve_right<StatementAst>(then_branch), branch_next);
    EXPECT_EQ(solver.solve_right<StatementAst>(else_branch), branch_next);

    EXPECT_EQ(solver.solve_right<StatementAst>(after), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(before), ConditionSet());

    ConditionSet condition_prev;
    condition_prev.insert(new SimpleStatementCondition(before));
    EXPECT_EQ(solver.solve_left<StatementAst>(condition), condition_prev);

    ConditionSet branch_prev;
    branch_prev.insert(new SimpleStatementCondition(condition));
    branch_prev.insert(new SimpleStatementCondition(before));
    EXPECT_EQ(solver.solve_left<StatementAst>(then_branch), branch_prev);
    EXPECT_EQ(solver.solve_left<StatementAst>(else_branch), branch_prev);

    ConditionSet after_previous;
    after_previous.insert(new SimpleStatementCondition(before));
    after_previous.insert(new SimpleStatementCondition(condition));
    after_previous.insert(new SimpleStatementCondition(then_branch));
    after_previous.insert(new SimpleStatementCondition(else_branch));
    EXPECT_EQ(solver.solve_left<StatementAst>(after), after_previous);
}

TEST(INextTest, WhileTest) {
    /*
     * proc test {
     *   a = 1;
     *   while i {
     *     x = 2;
     *     y = 3;
     *     z = 5;
     *   }
     *   b = 4;
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
    before->set_expr(new SimpleConstAst(1));
    before->set_line(1);
    set_proc(before, proc);

    SimpleWhileAst *loop = new SimpleWhileAst();
    loop->set_variable(var_i);
    loop->set_line(2);
    set_next(before, loop);

    SimpleAssignmentAst *stat1 = new SimpleAssignmentAst();
    stat1->set_variable(var_x);
    stat1->set_expr(new SimpleConstAst(2));
    stat1->set_line(3);
    set_while_body(stat1, loop);

    SimpleAssignmentAst *stat2 = new SimpleAssignmentAst();
    stat2->set_variable(var_y);
    stat2->set_expr(new SimpleConstAst(3));
    stat2->set_line(4);
    set_next(stat1, stat2);

    SimpleAssignmentAst *stat3 = new SimpleAssignmentAst();
    stat3->set_variable(var_z);
    stat3->set_expr(new SimpleConstAst(5));
    stat3->set_line(5);
    set_next(stat2, stat3);


    SimpleAssignmentAst *after = new SimpleAssignmentAst();
    after->set_variable(var_b);
    after->set_expr(new SimpleConstAst(4));
    after->set_line(6);
    set_next(loop, after);

    SimpleRoot root(proc);
    NextSolver next_solver(root);
    INextSolver solver(root, &next_solver);

    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, loop)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, stat1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, stat2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(before, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop, loop)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop, stat1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop, stat2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop, stat3)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(loop, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, stat2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, stat3)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, loop)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, stat1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat1, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat2, loop)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat2, stat1)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat2, stat2)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat2, stat3)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat2, after)));
    EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(stat3, stat1)));

    ConditionSet loop_next;
    loop_next.insert(new SimpleStatementCondition(loop));
    loop_next.insert(new SimpleStatementCondition(stat1));
    loop_next.insert(new SimpleStatementCondition(stat2));
    loop_next.insert(new SimpleStatementCondition(stat3));
    loop_next.insert(new SimpleStatementCondition(after));
    EXPECT_EQ(solver.solve_right<StatementAst>(before), loop_next);
    EXPECT_EQ(solver.solve_right<StatementAst>(loop), loop_next);
    EXPECT_EQ(solver.solve_right<StatementAst>(stat1), loop_next);
    EXPECT_EQ(solver.solve_right<StatementAst>(stat2), loop_next);

    EXPECT_EQ(solver.solve_right<StatementAst>(after), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(before), ConditionSet());

    ConditionSet loop_prev;
    loop_prev.insert(new SimpleStatementCondition(before));
    loop_prev.insert(new SimpleStatementCondition(loop));
    loop_prev.insert(new SimpleStatementCondition(stat1));
    loop_prev.insert(new SimpleStatementCondition(stat2));
    loop_prev.insert(new SimpleStatementCondition(stat3));
    EXPECT_EQ(solver.solve_left<StatementAst>(loop), loop_prev); 
    EXPECT_EQ(solver.solve_left<StatementAst>(stat1), loop_prev); 
    EXPECT_EQ(solver.solve_left<StatementAst>(stat2), loop_prev); 
    EXPECT_EQ(solver.solve_left<StatementAst>(stat3), loop_prev); 
    EXPECT_EQ(solver.solve_left<StatementAst>(after), loop_prev); 
}




}
}
