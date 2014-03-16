
#include "gtest/gtest.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "impl/solvers/expr.h"
#include "simple/util/ast_utils.h"
#include "simple/util/condition_utils.h"
#include "simple/util/expr.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(ExprTest, BasicTest1) {
    /*
     * proc test {
     *   x = y;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var_x("x");
    SimpleVariable var_y("y");

    ExprAst *expr = new SimpleVariableAst(var_y);
    EXPECT_EQ(expr_to_string(expr), "y");

    assign->set_variable(var_x);
    assign->set_expr(expr);
    assign->set_line(1);
    assign->set_proc(proc);

    proc->set_first_statement(assign);

    SimpleRoot root(proc);
    ExprSolver solver(root);

    EXPECT_TRUE(solver.validate_assign_expr(assign, expr));
    EXPECT_TRUE(solver.validate_statement_expr(assign, expr));
    EXPECT_TRUE((solver.validate<StatementAst, ExprAst>(assign, expr)));

    ExprSet expected_expr;
    expected_expr.insert(expr);

    EXPECT_EQ(solver.solve_right_assign_expr(assign), expected_expr);
    EXPECT_EQ(solver.solve_right_statement_expr(assign), expected_expr);

    StatementSet expected_statement;
    expected_statement.insert(assign);

    EXPECT_EQ(solver.solve_left_statement(expr), expected_statement);
}

TEST(ExprTest, BasicTest2) {
    /*
     * proc test {
     *   x = 1;
     *   y = b;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleVariable var_x("x");
    SimpleVariable var_y("y");
    SimpleVariable var_b("b");

    ExprAst *expr1 = new SimpleConstAst(1);
    EXPECT_EQ(expr_to_string(expr1), "1");

    ExprAst *expr2 = new SimpleVariableAst(var_b);
    EXPECT_EQ(expr_to_string(expr2), "b");

    SimpleAssignmentAst *assign1 = new SimpleAssignmentAst();
    assign1->set_variable(var_x);
    assign1->set_expr(expr1);
    assign1->set_line(1);
    assign1->set_proc(proc);

    SimpleAssignmentAst *assign2 = new SimpleAssignmentAst();
    assign2->set_variable(var_y);
    assign2->set_expr(expr2);
    set_next(assign1, assign2);

    proc->set_first_statement(assign1);

    SimpleRoot root(proc);
    ExprSolver solver(root);

    EXPECT_TRUE(solver.validate_assign_expr(assign1, expr1));
    EXPECT_TRUE(solver.validate_statement_expr(assign1, expr1));
    EXPECT_TRUE((solver.validate<StatementAst, ExprAst>(assign1, expr1)));

    ExprSet expected_expr1;
    expected_expr1.insert(expr1);

    EXPECT_EQ(solver.solve_right_assign_expr(assign1), expected_expr1);
    EXPECT_EQ(solver.solve_right_statement_expr(assign1), expected_expr1);

    ConditionPtr expr1_condition(new SimplePatternCondition(clone_expr(expr1)));
    EXPECT_EQ(condition_to_string(expr1_condition), "1");

    ConditionSet expected_left1;
    expected_left1.insert(expr1_condition);

    EXPECT_EQ((solver.solve_right<StatementAst>(assign1)), expected_left1);

    StatementSet expected_statement1;
    expected_statement1.insert(assign1);

    EXPECT_EQ(solver.solve_left_statement(expr1), expected_statement1);

    EXPECT_TRUE(solver.validate_assign_expr(assign2, expr2));
    EXPECT_TRUE(solver.validate_statement_expr(assign2, expr2));

    ExprSet expected_expr2;
    expected_expr2.insert(expr2);

    EXPECT_EQ(solver.solve_right_assign_expr(assign2), expected_expr2);
    EXPECT_EQ(solver.solve_right_statement_expr(assign2), expected_expr2);

    ConditionPtr expr2_condition(new SimplePatternCondition(clone_expr(expr2)));
    EXPECT_EQ(condition_to_string(expr2_condition), "b");

    ConditionSet expected_left2;
    expected_left2.insert(expr2_condition);

    EXPECT_EQ((solver.solve_right<StatementAst>(assign2)), expected_left2);
    EXPECT_NE((solver.solve_right<StatementAst>(assign2)), expected_left1);

    StatementSet expected_statement2;
    expected_statement2.insert(assign2);

    EXPECT_EQ(solver.solve_left_statement(expr2), expected_statement2);
}

}
}