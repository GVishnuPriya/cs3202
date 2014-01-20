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

#include <exception>
#include "gtest/gtest.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "impl/solvers/modifies.h"
#include "impl/solvers/next.h"
#include "simple/util/solver_generator.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(SolverTest, BasicTest) {
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

    SimpleIfAst *condition = new SimpleIfAst();
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

    SimpleRoot root(procs.begin(), procs.end());

    SimpleStatementCondition loop_condition(loop);
    SimpleStatementCondition stat1_condition(stat1_1);
    SimpleVariableCondition z_condition(var_z);
    
    SimpleSolverGenerator<ModifiesSolver> modifies_solver(new ModifiesSolver(root));
    SimpleSolverGenerator<NextSolver> next_solver(new NextSolver(root));
    
    QuerySolver *modifies_interface = &modifies_solver;
    EXPECT_EQ((modifies_interface->validate(&loop_condition, &z_condition)), 
        (modifies_solver.get_solver()->validate<StatementAst, SimpleVariable>(loop, &var_z)));;

    EXPECT_EQ((modifies_interface->solve_right(&loop_condition)), 
        (modifies_solver.get_solver()->solve_right<StatementAst>(loop)));;

    EXPECT_EQ((modifies_interface->solve_left(&z_condition)), 
        (modifies_solver.get_solver()->solve_left<SimpleVariable>(&var_z)));;


    QuerySolver *next_interface = &next_solver;
    EXPECT_EQ((next_interface->validate(&stat1_condition, &loop_condition)), 
        (next_solver.get_solver()->validate<StatementAst, StatementAst>(stat1_1, loop)));;

    EXPECT_EQ((next_interface->solve_right(&stat1_condition)), 
        (next_solver.get_solver()->solve_right<StatementAst>(stat1_1)));;

    EXPECT_EQ((next_interface->solve_left(&loop_condition)), 
        (next_solver.get_solver()->solve_left<StatementAst>(loop)));;

}

}
}
