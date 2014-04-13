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

#pragma once

#include <set>
#include <map>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/util/ast_utils.h"
#include "simple/solver.h"
#include "simple/next.h"

namespace simple {
namespace impl {

using namespace simple;

class NextBipSolver : 
    public NextQuerySolver,
    public NextBipQuerySolver
{
  public:
    NextBipSolver(SimpleRoot ast, 
        std::shared_ptr<NextQuerySolver> next_solver, 
        std::shared_ptr<CallsQuerySolver> calls_solver);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    StatementSet solve_next_statement(StatementAst *statement);

    StatementSet solve_prev_statement(StatementAst *statement);

    StackedStatementSet solve_next_bip_statement(
        StatementAst *statement, CallStack callstack);

    StackedStatementSet solve_prev_bip_statement(
        StatementAst *statement, CallStack callstack);

    StackedStatementSet solve_next_bip(
        StatementAst *statement, CallStack callstack);
    
  private:
    SimpleRoot _ast; 
    std::shared_ptr<NextQuerySolver> _next_solver;
    std::shared_ptr<CallsQuerySolver> _calls_solver;
};

template <typename Condition>
ConditionSet NextBipSolver::solve_right(Condition *condition) {
    return ConditionSet();
}

template <typename Condition>
ConditionSet NextBipSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition1, typename Condition2>
bool NextBipSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <>
ConditionSet NextBipSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet NextBipSolver::solve_left<StatementAst>(StatementAst *ast);

template <>
bool NextBipSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2);

}
}