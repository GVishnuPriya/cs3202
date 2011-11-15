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
#include <set>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;

class ICallSolver {
  public:
    ICallSolver(SimpleRoot ast);

    /*
     * SOLVE RIGHT PART
     */
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

    template <typename Condition>
    void index_calls(Condition *condition) {
        // no-op
    }

  private:
    typedef std::map<ProcAst*, std::set<ProcAst*> > ProcIndex;
    SimpleRoot _ast;
    ProcIndex _calls_table;
    ProcIndex _called_table;
};

template <>
ConditionSet ICallSolver::solve_right<ProcAst>(ProcAst *proc);

template <>
ConditionSet ICallSolver::solve_left<ProcAst>(ProcAst *proc);

template <>
bool ICallSolver::validate<ProcAst, ProcAst>(ProcAst *proc1, ProcAst *proc2);

template <>
void ICallSolver::index_calls<StatementAst>(StatementAst *statement);

template <>
void ICallSolver::index_calls<ProcAst>(ProcAst *proc);

template <>
void ICallSolver::index_calls<CallAst>(CallAst *call);

template <>
void ICallSolver::index_calls<WhileAst>(WhileAst *loop);

template <>
void ICallSolver::index_calls<ConditionalAst>(ConditionalAst *condition);





} // namespace impl
} // namespace simple
