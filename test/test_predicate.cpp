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
#include "impl/ast.h"
#include "simple/util/ast_utils.h"
#include "impl/predicate.h"
#include "impl/condition.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(PredicateTest, BasicTest) {
    /*
     * proc test1 {
     *   if i {
     *     x = 1;
     *   } else {
     *     y = 2;
     *   }
     *
     *   while j {
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
    SimpleVariable var_i("i");
    SimpleVariable var_j("j");

    SimpleIfAst *condition = new SimpleIfAst();
    condition->set_variable(var_i);
    set_proc(condition, proc1);

    SimpleWhileAst *loop = new SimpleWhileAst();
    loop->set_variable(var_j);
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

    std::unique_ptr<SimplePredicate> wildcard_pred(new SimpleWildCardPredicate(
            root, new WildCardPredicate()));

    ConditionSet wildcard_set;
    wildcard_set.insert(new SimpleProcCondition(proc1));
    wildcard_set.insert(new SimpleProcCondition(proc2));
    wildcard_set.insert(new SimpleStatementCondition(condition));
    wildcard_set.insert(new SimpleStatementCondition(loop));
    wildcard_set.insert(new SimpleStatementCondition(stat1_1));
    wildcard_set.insert(new SimpleStatementCondition(stat1_2));
    wildcard_set.insert(new SimpleStatementCondition(stat2_1));
    wildcard_set.insert(new SimpleStatementCondition(stat2_2));
    wildcard_set.insert(new SimpleStatementCondition(stat3));
    wildcard_set.insert(new SimpleVariableCondition(var_x));
    wildcard_set.insert(new SimpleVariableCondition(var_y));
    wildcard_set.insert(new SimpleVariableCondition(var_z));
    wildcard_set.insert(new SimpleVariableCondition(var_a));
    wildcard_set.insert(new SimpleVariableCondition(var_i));
    wildcard_set.insert(new SimpleVariableCondition(var_j));
    wildcard_set.insert(new SimplePatternCondition(new SimpleConstAst(1)));
    wildcard_set.insert(new SimplePatternCondition(new SimpleConstAst(2)));
    wildcard_set.insert(new SimplePatternCondition(new SimpleConstAst(3)));
    wildcard_set.insert(new SimplePatternCondition(new SimpleConstAst(4)));
    wildcard_set.insert(new SimpleConstantCondition(1));
    wildcard_set.insert(new SimpleConstantCondition(2));
    wildcard_set.insert(new SimpleConstantCondition(3));
    wildcard_set.insert(new SimpleConstantCondition(4));

    EXPECT_EQ(wildcard_pred->global_set(), wildcard_set);

    std::unique_ptr<SimplePredicate> statement_pred(new SimpleStatementPredicate(root, new StatementPredicate()));

    ConditionSet statement_set;
    statement_set.insert(new SimpleStatementCondition(condition));
    statement_set.insert(new SimpleStatementCondition(loop));
    statement_set.insert(new SimpleStatementCondition(stat1_1));
    statement_set.insert(new SimpleStatementCondition(stat1_2));
    statement_set.insert(new SimpleStatementCondition(stat2_1));
    statement_set.insert(new SimpleStatementCondition(stat2_2));
    statement_set.insert(new SimpleStatementCondition(stat3));

    EXPECT_EQ(statement_pred->global_set(), statement_set);

    std::unique_ptr<SimplePredicate> assign_pred(
            new PredicateGenerator<AssignPredicate>(root, new AssignPredicate()));

    ConditionSet assign_set;
    assign_set.insert(new SimpleStatementCondition(stat1_1));
    assign_set.insert(new SimpleStatementCondition(stat1_2));
    assign_set.insert(new SimpleStatementCondition(stat2_1));
    assign_set.insert(new SimpleStatementCondition(stat3));
    EXPECT_EQ(assign_pred->global_set(), assign_set);
}



}
}
