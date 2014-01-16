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

class ModifiesQuerySolver {
  public:
    virtual VariableSet solve_modified_vars(StatementAst *statement) = 0;
    virtual StatementSet solve_modifying_stats(SimpleVariable *var) = 0;

    virtual ~ModifiesQuerySolver() { }
};

class ModifiesSolver {
  public:
    ModifiesSolver(const SimpleRoot& ast);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    std::set<SimpleVariable> index_variables(Condition *condition);

    ~ModifiesSolver() { }
  private:
    SimpleRoot _ast;
    std::map<SimpleVariable, ConditionSet> _var_index;

    void index_statement_list(StatementAst *statement, ConditionPtr condition, std::set<SimpleVariable>& result);
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
std::set<SimpleVariable> ModifiesSolver::index_variables(Condition *condition) {
    return ConditionSet();
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
bool ModifiesSolver::validate<ConditionalAst, SimpleVariable>(
        ConditionalAst *ast, SimpleVariable *var); 

template <>
bool ModifiesSolver::validate<WhileAst, SimpleVariable>(
        WhileAst *ast, SimpleVariable *var);

template <>
bool ModifiesSolver::validate<CallAst, SimpleVariable>(
        CallAst *ast, SimpleVariable *var);

template <>
ConditionSet ModifiesSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet ModifiesSolver::solve_right<ConditionalAst>(ConditionalAst *ast);

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
std::set<SimpleVariable> ModifiesSolver::index_variables<ProcAst>(ProcAst *proc);

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<AssignmentAst>(AssignmentAst *assign);

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<WhileAst>(WhileAst *ast);

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<ConditionalAst>(ConditionalAst *ast);

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<CallAst>(CallAst *ast);

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<StatementAst>(StatementAst *statement);


} // namespace impl
} // namespace simple
