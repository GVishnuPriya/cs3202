/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *  Contributor(s):
 *    Daniel Le <GreenRecycleBin@gmail.com>
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

class UsesQuerySolver {
  public:
    virtual VariableSet solve_used_vars(StatementAst *statement) = 0;
    virtual StatementSet solve_using_stats(SimpleVariable *var) = 0;

    virtual ~UsesQuerySolver() { }
};

class UsesSolver {
  public:
    UsesSolver(const SimpleRoot& ast);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    ConditionSet solve_variable(Condition *condition, SimpleVariable *variable);

    template <typename Condition>
    std::set<SimpleVariable> index_variables(Condition *condition);

    ~UsesSolver() { }
  private:
    SimpleRoot _ast;
    std::map<SimpleVariable, ConditionSet> _var_index;

    void index_statement_list(StatementAst *statement, ConditionPtr condition, std::set<SimpleVariable>& result);
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
ConditionSet UsesSolver::solve_variable(Condition *condition, SimpleVariable *variable) {
    return ConditionSet(); // empty set
}

template <typename Condition>
ConditionSet UsesSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
std::set<SimpleVariable> UsesSolver::index_variables(Condition *condition) {
    return ConditionSet();
}

template <>
bool UsesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<AssignmentAst, SimpleVariable>(
        AssignmentAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<ExprAst, SimpleVariable>(
        ExprAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<VariableAst, SimpleVariable>(
        VariableAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<BinaryOpAst, SimpleVariable>(
        BinaryOpAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<ConditionalAst, SimpleVariable>(
        ConditionalAst *ast, SimpleVariable *var); 

template <>
bool UsesSolver::validate<WhileAst, SimpleVariable>(
        WhileAst *ast, SimpleVariable *var);

template <>
bool UsesSolver::validate<CallAst, SimpleVariable>(
        CallAst *ast, SimpleVariable *var);

template <>
ConditionSet UsesSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet UsesSolver::solve_right<ConditionalAst>(ConditionalAst *ast);

template <>
ConditionSet UsesSolver::solve_right<WhileAst>(WhileAst *ast);

template <>
ConditionSet UsesSolver::solve_right<ProcAst>(ProcAst *ast);

template <>
ConditionSet UsesSolver::solve_right<AssignmentAst>(AssignmentAst *ast);

template <>
ConditionSet UsesSolver::solve_right<ExprAst>(ExprAst *ast);

template <>
ConditionSet UsesSolver::solve_right<VariableAst>(VariableAst *ast);

template <>
ConditionSet UsesSolver::solve_right<ConstAst>(ConstAst *ast);

template <>
ConditionSet UsesSolver::solve_right<BinaryOpAst>(BinaryOpAst *ast);

template <>
ConditionSet UsesSolver::solve_right<CallAst>(CallAst *ast);

} // namespace impl
} // namespace simple
