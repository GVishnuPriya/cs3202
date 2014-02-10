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
#include "simple/condition_set.h"
#include "simple/solver.h"
#include "simple/util/statement_visitor_generator.h"
#include "impl/expr.h"

namespace simple {
namespace impl {

using namespace simple;

class UsesSolver : public UsesQuerySolver {
  public:
    UsesSolver(const SimpleRoot& ast);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    VariableSet solve_used_vars(StatementAst *statement);
    StatementSet solve_using_statements(const SimpleVariable& variable);

    template <typename Condition>
    VariableSet index_uses(Condition *condition);

    ~UsesSolver() { }

  private:
    SimpleRoot _ast;

    std::map<SimpleVariable, StatementSet> _using_statement_index;
    std::map<SimpleVariable, ConditionSet> _using_condition_index;

    std::map<StatementAst*, VariableSet> _used_by_statement_index;
    std::map<ConditionPtr, ConditionSet> _used_by_condition_index;

    VariableSet index_statement_list(StatementAst *statement);
    void index_statement(StatementAst *statement, const SimpleVariable& variable);
    void index_condition(ConditionPtr condition, const SimpleVariable& variable);
    void index_container_statement(StatementAst *statement, const VariableSet& variables);
    void index_container_condition(ConditionPtr condition, const VariableSet& variables);
};

/*
 * Template declarations
 */

template <typename Condition1, typename Condition2>
bool UsesSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <typename Condition>
ConditionSet UsesSolver::solve_right(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
ConditionSet UsesSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
VariableSet UsesSolver::index_uses(Condition *condition) {
    return ConditionSet();
}

template <>
bool UsesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *ast, SimpleVariable *var);

template <>
ConditionSet UsesSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet UsesSolver::solve_right<ProcAst>(ProcAst *ast);

template <>
ConditionSet UsesSolver::solve_left<SimpleVariable>(SimpleVariable *variable);

template <>
VariableSet UsesSolver::index_uses<ProcAst>(ProcAst *proc);

template <>
VariableSet UsesSolver::index_uses<AssignmentAst>(AssignmentAst *assign);

template <>
VariableSet UsesSolver::index_uses<WhileAst>(WhileAst *ast);

template <>
VariableSet UsesSolver::index_uses<IfAst>(IfAst *ast);

template <>
VariableSet UsesSolver::index_uses<CallAst>(CallAst *ast);

template <>
VariableSet UsesSolver::index_uses<StatementAst>(StatementAst *statement);


} // namespace impl
} // namespace simple
