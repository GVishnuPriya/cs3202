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
#include "simple/solver.h"
#include "simple/util/query_utils.h"
#include "simple/util/ast_utils.h"
#include "simple/util/solver_generator.h"
#include "impl/ast.h"
#include "impl/linker.h"
#include "impl/parser/pql_parser.h"
#include "impl/solvers/follows.h"
#include "impl/solvers/modifies.h"
#include "impl/predicate.h"
#include "impl/processor.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(QueryProcessorTest, IntegratedTest) {
    /*
     * proc test {
     *   x = 1;
     *   y = 2;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *stat1 = new SimpleAssignmentAst();
    SimpleVariable var_x("x");
    SimpleVariable var_y("y");

    stat1->set_variable(var_x);
    stat1->set_expr(new SimpleConstAst(1));
    stat1->set_line(1);
    set_proc(stat1, proc);

    SimpleAssignmentAst *stat2 = new SimpleAssignmentAst();

    stat2->set_variable(var_y);
    stat2->set_expr(new SimpleConstAst(2));
    stat2->set_line(2);
    set_next(stat1, stat2);

    SimpleRoot ast(proc);

    std::shared_ptr<QuerySolver> follows_solver(
            new SimpleSolverGenerator<FollowSolver>(new FollowSolver(ast)));
    std::shared_ptr<QuerySolver> modifies_solver(
            new SimpleSolverGenerator<ModifiesSolver>(new ModifiesSolver(ast)));

    LineTable line_table;
    line_table[1] = stat1;
    line_table[2] = stat2;

    SolverTable solver_table;
    solver_table["follows"] = follows_solver;
    solver_table["modifies"] = modifies_solver;

    std::shared_ptr<SimplePredicate> wildcard_pred(new SimpleWildCardPredicate(ast));
    std::shared_ptr<SimplePredicate> statement_pred(new SimpleStatementPredicate(ast));
    std::shared_ptr<SimplePredicate> variable_pred(new SimpleVariablePredicate(ast));

    PredicateTable pred_table;
    pred_table["s"] = statement_pred;
    pred_table["v"] = variable_pred;

    std::shared_ptr<SimpleQueryLinker> linker1(new SimpleQueryLinker());

    QueryProcessor processor(linker1, pred_table, wildcard_pred);

    /*
     * Follows(1, s)
     */
    ClausePtr follows_clause(new SimplePqlClause(follows_solver,
                new SimplePqlConditionTerm(ConditionPtr(
                    new SimpleStatementCondition(stat1))),
                new SimplePqlVariableTerm("s")));

    processor.solve_clause(follows_clause.get());

    ConditionSet expected_s;
    expected_s.insert(new SimpleStatementCondition(stat2));

    EXPECT_EQ(linker1->get_conditions("s"), expected_s);

    /*
     * Modifies(s, v)
     */
    ClausePtr modifies_clause(new SimplePqlClause(modifies_solver,
                new SimplePqlVariableTerm("s"),
                new SimplePqlVariableTerm("v")));

    processor.solve_clause(modifies_clause.get());
    
    ConditionSet expected_v;
    expected_v.insert(new SimpleVariableCondition(var_y));

    EXPECT_EQ(linker1->get_conditions("s"), expected_s);
    EXPECT_EQ(linker1->get_conditions("v"), expected_v);

    EXPECT_EQ(linker1->get_linked_conditions("s", "v",
            new SimpleStatementCondition(stat2)), expected_v);
}



}
}
