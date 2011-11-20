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
#include "impl/solvers/uses.h"
#include "impl/ast.h"
#include "impl/condition.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(UsesTest, FixtureTest1) {
    /*
     * proc test {
     *   x = y;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");
    SimpleVariable var2("y"); // different name
    SimpleVariable var3("y"); // same name different pointer

    assign->set_variable(var);
    assign->set_expr(new SimpleVariableAst(var2));
    assign->set_line(1);
    assign->set_proc(proc);

    proc->set_first_statement(assign);

    SimpleRoot root(proc);

    UsesSolver solver(root);

    /*
     * Test true validation
     */
    EXPECT_TRUE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var2)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(assign, &var2)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc, &var2)));
    // Different variable objects with same name
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(assign, &var3)));
    EXPECT_TRUE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var3)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc, &var3)));
    /*
     * Test false validation
     */
    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(assign, &var)));
    EXPECT_FALSE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var)));
    EXPECT_FALSE((solver.validate<ProcAst, SimpleVariable>(proc, &var)));

    /*
     * Test validation of unrelated types
     */
    EXPECT_FALSE((solver.validate<ProcAst, StatementAst>(proc, assign)));
    EXPECT_FALSE((solver.validate<SimpleVariable, StatementAst>(&var, assign)));


    ConditionSet expected_vars;
    expected_vars.insert(new SimpleVariableCondition(SimpleVariable("y")));

    EXPECT_EQ(solver.solve_right<StatementAst>(assign), expected_vars);
    EXPECT_EQ(solver.solve_right<ProcAst>(proc), expected_vars);
    EXPECT_EQ(solver.solve_right<SimpleVariable>(&var), ConditionSet());
}

}
}
