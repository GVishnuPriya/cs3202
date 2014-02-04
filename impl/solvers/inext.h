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

#include <map>
#include "simple/solver.h"
#include "simple/condition_set.h"
#include "simple/ast.h"

namespace simple {
namespace impl {

using namespace simple;

class INextSolver {
  public:
    typedef std::map<StatementAst*, StatementSet>  INextTable;

    INextSolver(SimpleRoot ast, std::shared_ptr<NextQuerySolver> solver) :
        _ast(ast), _next_solver(solver)
    { }

    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2) {
        return false;
    }

    void solve_inext(StatementAst *statement, StatementSet& results);
    void solve_iprev(StatementAst *statement, StatementSet& results);

  private:
    SimpleRoot _ast;
    std::shared_ptr<NextQuerySolver> _next_solver;
    INextTable _inext_cache;
    INextTable _iprev_cache;
};

template <>
bool INextSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2);

template <>
ConditionSet INextSolver::solve_right<StatementAst>(StatementAst *statement);

template <>
ConditionSet INextSolver::solve_left<StatementAst>(StatementAst *statement);

}
}
