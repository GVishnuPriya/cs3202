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
#include <memory>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/solver.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;

class AssignStatementSolver {
  public:
    virtual VariableSet solve_assignment(AssignmentAst *assign) = 0;

    virtual ~AssignStatementSolver() { }
};

class AssignmentSolver {
  public:
    AssignmentSolver(const SimpleRoot& ast, 
        std::shared_ptr<AssignStatementSolver> assign_solver);

    VariableSet get_right_vars_from_statement(StatementAst *statement);
    VariableSet get_right_vars_from_proc(ProcAst *proc);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    VariableSet index_variables(Condition *condition);

    ~AssignmentSolver() { }

  private:
    SimpleRoot _ast;
    std::shared_ptr<AssignStatementSolver> _assign_solver;

    std::map<SimpleVariable, ConditionSet> _right_condition_index;

    std::map<StatementAst*, VariableSet> _left_statement_index;
    std::map<ProcAst*, VariableSet> _left_proc_index;

    VariableSet index_statement_list(StatementAst *statement);
    void index_statement_variables(StatementAst *statement, const VariableSet& variables);
};

/*
 * Template declarations
 */

template <typename Condition1, typename Condition2>
bool AssignmentSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <typename Condition>
ConditionSet AssignmentSolver::solve_right(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
ConditionSet AssignmentSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
VariableSet AssignmentSolver::index_variables(Condition *condition) {
    return ConditionSet();
}

template <>
bool AssignmentSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *ast, SimpleVariable *var);

template <>
bool AssignmentSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *ast, SimpleVariable *var);

template <>
ConditionSet AssignmentSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet AssignmentSolver::solve_right<ProcAst>(ProcAst *ast);

template <>
ConditionSet AssignmentSolver::solve_left<SimpleVariable>(SimpleVariable *variable);

template <>
VariableSet AssignmentSolver::index_variables<ProcAst>(ProcAst *proc);

template <>
VariableSet AssignmentSolver::index_variables<AssignmentAst>(AssignmentAst *assign);

template <>
VariableSet AssignmentSolver::index_variables<WhileAst>(WhileAst *ast);

template <>
VariableSet AssignmentSolver::index_variables<IfAst>(IfAst *ast);

template <>
VariableSet AssignmentSolver::index_variables<CallAst>(CallAst *ast);

template <>
VariableSet AssignmentSolver::index_variables<StatementAst>(StatementAst *statement);


} // namespace impl
} // namespace simple
