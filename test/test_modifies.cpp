
#include "gtest/gtest.h"
#include "simple/util/ast_utils.h"
#include "impl/solvers/modifies.h"
#include "impl/ast.h"
#include "impl/condition.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(ModifiesTest, FixtureTest1) {
    /*
     * proc test {
     *   x = 1;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");
    SimpleVariable var2("y"); // different name
    SimpleVariable var3("x"); // same name different pointer

    assign->set_variable(var);
    assign->set_expr(new SimpleConstAst(1));
    assign->set_line(1);
    assign->set_proc(proc);

    proc->set_first_statement(assign);

    SimpleRoot root(proc);

    ModifiesSolver solver(root, NULL);

    /*
     * Test true validation
     */
    EXPECT_TRUE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(assign, &var)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc, &var)));
    // Different variable objects with same name
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(assign, &var3)));
    EXPECT_TRUE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var3)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc, &var3)));
    /*
     * Test false validation
     */
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(assign, &var2)));
    EXPECT_FALSE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var2)));
    EXPECT_FALSE((solver.validate<ProcAst, SimpleVariable>(proc, &var2)));

    /*
     * Test validation of unrelated types
     */
    EXPECT_FALSE((solver.validate<ProcAst, StatementAst>(proc, assign)));
    EXPECT_FALSE((solver.validate<SimpleVariable, StatementAst>(&var, assign)));


    ConditionSet expected_vars;
    expected_vars.insert(new SimpleVariableCondition(SimpleVariable("x")));

    EXPECT_EQ(solver.solve_right<StatementAst>(assign), expected_vars);
    EXPECT_EQ(solver.solve_right<ProcAst>(proc), expected_vars);
    EXPECT_EQ(solver.solve_right<SimpleVariable>(&var), ConditionSet());

    ConditionSet expected_statements;
    expected_statements.insert(new SimpleStatementCondition(assign));
    expected_statements.insert(new SimpleProcCondition(proc));

    EXPECT_EQ(solver.solve_left<SimpleVariable>(&var), expected_statements);
    EXPECT_EQ(solver.solve_left<SimpleVariable>(&var2), ConditionSet());
    EXPECT_EQ(solver.solve_left<StatementAst>(assign), ConditionSet());
    EXPECT_EQ(solver.solve_left<ProcAst>(proc), ConditionSet());
}

TEST(ModifiesTest, QuirksTest) {
    /*
     * proc test1 {
     *   if a {
     *     x = 1;
     *   } else {
     *     y = 2;
     *   }
     *
     *   while b {
     *     z  = 3;
     *     call test2;
     *   }
     * }
     *
     * proc test2 {
     *   a = 4;
     * }
     */

    SimpleProcAst *proc1 = new SimpleProcAst("test1");
    SimpleProcAst *proc2 = new SimpleProcAst("test2");

    SimpleVariable var_x("x");
    SimpleVariable var_y("y");
    SimpleVariable var_z("z");
    SimpleVariable var_a("a");

    SimpleConditionalAst *condition = new SimpleConditionalAst();
    set_proc(condition, proc1);

    SimpleWhileAst *loop = new SimpleWhileAst();
    set_next(condition, loop);

    SimpleAssignmentAst *stat1_1 = new SimpleAssignmentAst();
    stat1_1->set_variable(var_x);
    stat1_1->set_expr(new SimpleConstAst(1));
    set_then_branch(stat1_1, condition);

    SimpleAssignmentAst *stat1_2 = new SimpleAssignmentAst();
    stat1_2->set_variable(var_y);
    stat1_2->set_expr(new SimpleConstAst(2));
    set_else_branch(stat1_2, condition);

    SimpleAssignmentAst *stat2_1 = new SimpleAssignmentAst();
    stat2_1->set_variable(var_z);
    stat2_1->set_expr(new SimpleConstAst(3));
    set_while_body(stat2_1, loop);

    SimpleCallAst *stat2_2 = new SimpleCallAst();
    stat2_2->set_proc_called(proc2);
    set_next(stat2_1, stat2_2);

    SimpleAssignmentAst *stat3 = new SimpleAssignmentAst();
    stat3->set_variable(var_a);
    stat3->set_expr(new SimpleConstAst(4));
    set_proc(stat3, proc2);

    std::vector<ProcAst*> procs;
    procs.push_back(proc1);
    procs.push_back(proc2);

    SimpleRoot root(procs);
    ModifiesSolver solver(root, NULL);

    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(condition, &var_x)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(condition, &var_y)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(condition, &var_z)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(condition, &var_a)));

    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(loop, &var_z)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(loop, &var_a)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(loop, &var_x)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(loop, &var_y)));

    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc1, &var_x)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc1, &var_y)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc1, &var_z)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc1, &var_a)));
    
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc2, &var_a)));
    EXPECT_FALSE((solver.validate<ProcAst, SimpleVariable>(proc2, &var_x)));
    EXPECT_FALSE((solver.validate<ProcAst, SimpleVariable>(proc2, &var_y)));
    EXPECT_FALSE((solver.validate<ProcAst, SimpleVariable>(proc2, &var_z)));

    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(stat1_1, &var_x)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(stat1_2, &var_y)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(stat2_1, &var_z)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(stat2_2, &var_a)));

    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat1_1, &var_y)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat1_1, &var_z)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat1_1, &var_a)));

    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat1_2, &var_x)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat1_2, &var_z)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat1_2, &var_a)));

    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat2_1, &var_x)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat2_1, &var_y)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat2_1, &var_a)));

    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat2_2, &var_y)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat2_2, &var_z)));
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(stat2_2, &var_x)));

    ConditionSet condition_vars;
    condition_vars.insert(new SimpleVariableCondition(var_x));
    condition_vars.insert(new SimpleVariableCondition(var_y));
    EXPECT_EQ(solver.solve_right<StatementAst>(condition), condition_vars);

    ConditionSet loop_vars;
    loop_vars.insert(new SimpleVariableCondition(var_z));
    loop_vars.insert(new SimpleVariableCondition(var_a));
    EXPECT_EQ(solver.solve_right<StatementAst>(loop), loop_vars);

    ConditionSet proc_vars;
    proc_vars.insert(new SimpleVariableCondition(var_x));
    proc_vars.insert(new SimpleVariableCondition(var_y));
    proc_vars.insert(new SimpleVariableCondition(var_z));
    proc_vars.insert(new SimpleVariableCondition(var_a));
    EXPECT_EQ(solver.solve_right<ProcAst>(proc1), proc_vars);

    ConditionSet proc2_vars;
    proc2_vars.insert(new SimpleVariableCondition(var_a));
    EXPECT_EQ(solver.solve_right<ProcAst>(proc2), proc2_vars);
    EXPECT_EQ(solver.solve_right<StatementAst>(stat2_2), proc2_vars);

    ConditionSet x_statements;
    x_statements.insert(new SimpleProcCondition(proc1));
    x_statements.insert(new SimpleStatementCondition(condition));
    x_statements.insert(new SimpleStatementCondition(stat1_1));
    EXPECT_EQ(solver.solve_left<SimpleVariable>(&var_x), x_statements);

    ConditionSet y_statements;
    y_statements.insert(new SimpleProcCondition(proc1));
    y_statements.insert(new SimpleStatementCondition(condition));
    y_statements.insert(new SimpleStatementCondition(stat1_2));
    EXPECT_EQ(solver.solve_left<SimpleVariable>(&var_y), y_statements);

    ConditionSet z_statements;
    z_statements.insert(new SimpleProcCondition(proc1));
    z_statements.insert(new SimpleStatementCondition(loop));
    z_statements.insert(new SimpleStatementCondition(stat2_1));
    EXPECT_EQ(solver.solve_left<SimpleVariable>(&var_z), z_statements);

    ConditionSet a_statements;
    a_statements.insert(new SimpleProcCondition(proc1));
    a_statements.insert(new SimpleProcCondition(proc2));
    a_statements.insert(new SimpleStatementCondition(loop));
    a_statements.insert(new SimpleStatementCondition(stat2_2));
    a_statements.insert(new SimpleStatementCondition(stat3));
    EXPECT_EQ(solver.solve_left<SimpleVariable>(&var_a), a_statements);
}

} // namespace test
} // namespace simple
