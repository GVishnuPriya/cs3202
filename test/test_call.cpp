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
#include "impl/solvers/call.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "simple/util/ast_utils.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::util;
using namespace simple::impl;

TEST(CallTest, TrivialTest) {
    /*
     * proc test1 {
     *   call test2;
     *   if j {
     *     call test3
     *   } else {
     *     call test4
     *   }
     * }
     *
     * proc test2 {
     *   while i {
     *     call test3
     *   }
     * }
     *
     * proc test3 {
     *   call test2
     * }
     *
     * proc test4 {
     *   call test1
     * }
     */
    SimpleProcAst *proc1 = new SimpleProcAst("test1");
    SimpleProcAst *proc2 = new SimpleProcAst("test2");
    SimpleProcAst *proc3 = new SimpleProcAst("test3");
    SimpleProcAst *proc4 = new SimpleProcAst("test4");

    SimpleVariable var_i("i");

    SimpleCallAst *call1 = new SimpleCallAst(proc2);
    set_proc(call1, proc1);

    SimpleConditionalAst *condition = new SimpleConditionalAst();
    condition->set_variable(var_i);
    set_next(call1, condition);

    SimpleCallAst *call2 = new SimpleCallAst(proc3);
    set_then_branch(call2, condition);

    SimpleCallAst *call3 = new SimpleCallAst(proc4);
    set_else_branch(call3, condition);

    SimpleWhileAst *loop = new SimpleWhileAst();
    loop->set_variable(var_i);
    set_proc(loop, proc2);

    SimpleCallAst *call4 = new SimpleCallAst(proc3);
    set_while_body(call4, loop);

    SimpleCallAst *call5 = new SimpleCallAst(proc2);
    set_proc(call5, proc3);

    SimpleCallAst *call6 = new SimpleCallAst(proc1);
    set_proc(call6, proc4);

    std::vector<ProcAst*> procs;
    procs.push_back(proc1);
    procs.push_back(proc2);
    procs.push_back(proc3);
    procs.push_back(proc4);

    SimpleRoot root(procs.begin(), procs.end());
    CallSolver solver(root);

    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc1, proc2)));
    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc1, proc3)));
    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc1, proc4)));
    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc2, proc3)));
    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc3, proc2)));
    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc4, proc1)));
    
    EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc3, proc1)));
    EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc2, proc1)));
    EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc4, proc2)));
    EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc4, proc3)));
    EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc3, proc4)));

    ConditionSet proc1_calls;
    proc1_calls.insert(new SimpleProcCondition(proc2));
    proc1_calls.insert(new SimpleProcCondition(proc3));
    proc1_calls.insert(new SimpleProcCondition(proc4));
    EXPECT_EQ(solver.solve_right<ProcAst>(proc1), proc1_calls);

    ConditionSet proc2_calls;
    proc2_calls.insert(new SimpleProcCondition(proc3));
    EXPECT_EQ(solver.solve_right<ProcAst>(proc2), proc2_calls);
    
    ConditionSet proc3_calls;
    proc3_calls.insert(new SimpleProcCondition(proc2));
    EXPECT_EQ(solver.solve_right<ProcAst>(proc3), proc3_calls);
    
    ConditionSet proc4_calls;
    proc4_calls.insert(new SimpleProcCondition(proc1));
    EXPECT_EQ(solver.solve_right<ProcAst>(proc4), proc4_calls);
    
    ConditionSet proc1_called;
    proc1_called.insert(new SimpleProcCondition(proc4));
    EXPECT_EQ(solver.solve_left<ProcAst>(proc1), proc1_called);

    ConditionSet proc2_called;
    proc2_called.insert(new SimpleProcCondition(proc1));
    proc2_called.insert(new SimpleProcCondition(proc3));
    EXPECT_EQ(solver.solve_left<ProcAst>(proc2), proc2_called);

    ConditionSet proc3_called;
    proc3_called.insert(new SimpleProcCondition(proc1));
    proc3_called.insert(new SimpleProcCondition(proc2));
    EXPECT_EQ(solver.solve_left<ProcAst>(proc3), proc3_called);

    ConditionSet proc4_called;
    proc4_called.insert(new SimpleProcCondition(proc1));
    EXPECT_EQ(solver.solve_left<ProcAst>(proc4), proc4_called);
}

}
}
