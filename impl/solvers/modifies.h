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

namespace simple {
namespace impl {

using namespace simple;

class ModifiesSolver : public ModifiesQuerySolver {
  public:
    ModifiesSolver(const SimpleRoot& ast);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    VariableSet solve_modified(Condition *condition);

    template <typename Condition>
    StatementSet solve_modifying(Condition *condition);

    VariableSet solve_modified_vars(StatementAst *statement);
    StatementSet solve_modifying_statements(const SimpleVariable& variable);

    template <typename Condition>
    VariableSet index_variables(Condition *condition);

    ~ModifiesSolver() { }

  private:
    SimpleRoot _ast;
    std::map<SimpleVariable, ConditionSet> _modifying_condition_index;
    std::map<SimpleVariable, StatementSet> _modifying_statement_index;
    std::map<StatementAst*, VariableSet> _modified_cache;

    VariableSet index_statement_list(StatementAst *statement);
    void index_statement(StatementAst *statement, const SimpleVariable& variable);
    void index_container_statement(StatementAst *statement, const VariableSet& variables);
    void index_container_condition(ConditionPtr condition, const VariableSet& variables);
};

/*
 * Template declarations
 */

template <typename Condition1, typename Condition2>
bool ModifiesSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <typename Condition>
ConditionSet ModifiesSolver::solve_right(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
ConditionSet ModifiesSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
VariableSet ModifiesSolver::index_variables(Condition *condition) {
    return ConditionSet();
}

template <typename Condition>
VariableSet ModifiesSolver::solve_modified(Condition *condition) {
    return VariableSet();
}

template <typename Condition>
StatementSet ModifiesSolver::solve_modifying(Condition *condition) {
    return StatementSet();
}

template <>
bool ModifiesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *ast, SimpleVariable *var);

template <>
bool ModifiesSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *ast, SimpleVariable *var);

template <>
bool ModifiesSolver::validate<AssignmentAst, SimpleVariable>(
        AssignmentAst *ast, SimpleVariable *var);

template <>
bool ModifiesSolver::validate<IfAst, SimpleVariable>(
        IfAst *ast, SimpleVariable *var); 

template <>
bool ModifiesSolver::validate<WhileAst, SimpleVariable>(
        WhileAst *ast, SimpleVariable *var);

template <>
bool ModifiesSolver::validate<CallAst, SimpleVariable>(
        CallAst *ast, SimpleVariable *var);

template <>
ConditionSet ModifiesSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet ModifiesSolver::solve_right<IfAst>(IfAst *ast);

template <>
ConditionSet ModifiesSolver::solve_right<WhileAst>(WhileAst *ast);

template <>
ConditionSet ModifiesSolver::solve_right<ProcAst>(ProcAst *ast);

template <>
ConditionSet ModifiesSolver::solve_right<AssignmentAst>(AssignmentAst *ast);

template <>
ConditionSet ModifiesSolver::solve_right<CallAst>(CallAst *ast);

template <>
ConditionSet ModifiesSolver::solve_left<SimpleVariable>(SimpleVariable *variable);

template <>
VariableSet ModifiesSolver::index_variables<ProcAst>(ProcAst *proc);

template <>
VariableSet ModifiesSolver::index_variables<AssignmentAst>(AssignmentAst *assign);

template <>
VariableSet ModifiesSolver::index_variables<WhileAst>(WhileAst *ast);

template <>
VariableSet ModifiesSolver::index_variables<IfAst>(IfAst *ast);

template <>
VariableSet ModifiesSolver::index_variables<CallAst>(CallAst *ast);

template <>
VariableSet ModifiesSolver::index_variables<StatementAst>(StatementAst *statement);


} // namespace impl
} // namespace simple
