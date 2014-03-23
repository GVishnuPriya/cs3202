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

#include "simple/ast.h"
#include "simple/solver.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class EqualSolver {
  public:
    EqualSolver(SimpleRoot ast);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_equal(Condition *condition);

  private:
    void index_proc(ProcAst *proc);
    void index_statement_list(StatementAst *statement);
    void index_statement(StatementAst *statement);
    void index_if(IfAst *ast);
    void index_while(WhileAst *ast);
    void index_variable(SimpleVariable *var);
    void index_assign(AssignmentAst *assign);
    void index_expr(ExprAst *expr);
    void index_binary_op_expr(BinaryOpAst *ast);
    void index_var_expr(VariableAst *ast);
    void index_const_expr(ConstAst *ast);

    SimpleRoot _ast;
    std::map< std::string, ConditionSet > _name_index;
    std::map< int, ConditionSet > _number_index;
};

template <typename Condition>
ConditionSet EqualSolver::solve_right(Condition *condition) {
    return solve_equal<Condition>(condition);
}

template <typename Condition>
ConditionSet EqualSolver::solve_left(Condition *condition) {
    return solve_equal<Condition>(condition);
}

template <typename Condition>
ConditionSet EqualSolver::solve_equal(Condition *condition) {
    return ConditionSet();
}

template <typename Condition1, typename Condition2>
bool EqualSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <>
ConditionSet EqualSolver::solve_equal<ProcAst>(ProcAst *proc);

template <>
ConditionSet EqualSolver::solve_equal<SimpleVariable>(SimpleVariable *var);

template <>
ConditionSet EqualSolver::solve_equal<StatementAst>(StatementAst *statement);

template <>
ConditionSet EqualSolver::solve_equal<SimpleConstant>(SimpleConstant *constant);

template <>
bool EqualSolver::validate<SimpleVariable, SimpleVariable>(
    SimpleVariable *var1, SimpleVariable *var2);

template <>
bool EqualSolver::validate<ProcAst, ProcAst>(
    ProcAst *proc1, ProcAst *proc2);

template <>
bool EqualSolver::validate<StatementAst, StatementAst>(
    StatementAst *statement1, StatementAst *statement2);

template <>
bool EqualSolver::validate<SimpleConstant, SimpleConstant>(
    SimpleConstant *constant1, SimpleConstant *constant2);

template <>
bool EqualSolver::validate<SimpleVariable, ProcAst>(
    SimpleVariable *var1, ProcAst *proc2);

template <>
bool EqualSolver::validate<ProcAst, SimpleVariable>(
    ProcAst *proc1, SimpleVariable *var2);

template <>
bool EqualSolver::validate<StatementAst, SimpleConstant>(
    StatementAst *statement1, SimpleConstant *constant2);

template <>
bool EqualSolver::validate<SimpleConstant, StatementAst>(
    SimpleConstant *constant1, StatementAst *statement2);

}
}