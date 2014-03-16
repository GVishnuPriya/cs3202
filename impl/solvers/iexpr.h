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
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class IExprSolver {
  public:
    IExprSolver(SimpleRoot ast);

    /*
     * Pattern a(v,_p_) = IExpr(a,p) & Modifies(a,v)
     */

    bool validate_statement_expr(StatementAst *statement, ExprAst *pattern);
    bool validate_assign_expr(AssignmentAst *assign_ast, ExprAst *pattern);

    StatementSet solve_left_statement(ExprAst *pattern);

    ExprSet solve_right_statement_expr(StatementAst *statement);
    ExprSet solve_right_assign_expr(AssignmentAst *assign_ast);

    void index_proc(ProcAst *proc);
    void index_statement_list(StatementAst *statement);
    void index_statement(StatementAst *statement) ;
    void index_while(WhileAst *while_ast);
    void index_if(IfAst *if_ast);
    void index_assign(AssignmentAst *assign_ast);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

  private:
    SimpleRoot _ast;

    // map string to set of assign statement for solve-left
    std::map<std::string, StatementSet > _pattern_index;
};

template <typename Condition>
ConditionSet IExprSolver::solve_right(Condition *condition) {
    return ConditionSet();
}

template <typename Condition>
ConditionSet IExprSolver::solve_left(Condition *condition) {
    return ConditionSet();
}

template <typename Condition1, typename Condition2>
bool IExprSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <>
ConditionSet IExprSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet IExprSolver::solve_left<ExprAst>(ExprAst *ast);

template <>
bool IExprSolver::validate<StatementAst, ExprAst>(
        StatementAst *statement, ExprAst *expr);

} // namespace impl
} // namespace simple
