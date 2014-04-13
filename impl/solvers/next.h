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
#include "simple/util/ast_utils.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/solver.h"
#include "simple/next.h"

namespace simple {
namespace impl {

using namespace simple;

class NextSolver : public SimpleNextQuerySolver {
  public:
    NextSolver(SimpleRoot ast) : _ast(ast) { }

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    StatementSet solve_next_statement(StatementAst *statement);
    StatementSet solve_prev_statement(StatementAst *statement);

    template <typename Condition>
    StatementSet solve_next(Condition *condition);

    template <typename Condition>
    StatementSet solve_previous(Condition *condition);

    template <typename Condition>
    StatementSet solve_preceeding_previous(Condition *condition);

    template <typename Container>
    StatementSet solve_container_next(Container *container);
    
    template <typename Ast>
    StatementSet solve_last_previous(Ast *ast);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    bool validate_next(Condition *condition, StatementAst *statement);

    template <typename Container>
    bool validate_container_next(Container *container, StatementAst *statement);

  private:
    SimpleRoot _ast;
    std::map<StatementAst*, StatementSet> _next_cache;
    std::map<StatementAst*, StatementSet> _prev_cache;
};

template <typename Condition>
StatementSet solve_next(Condition *condition) {
    return StatementSet();
}

template <typename Condition>
StatementSet solve_left_statement(Condition *condition) {
    return StatementSet();
}

template <typename Condition>
ConditionSet NextSolver::solve_right(Condition *condition) {
    return ConditionSet();
}

template <typename Condition>
ConditionSet NextSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition1, typename Condition2>
bool NextSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <typename Condition>
StatementSet NextSolver::solve_previous(Condition *condition) {
    return StatementSet();
}

template <typename Condition>
StatementSet NextSolver::solve_preceeding_previous(Condition *condition) {
    return StatementSet();
}

template <typename Condition>
bool NextSolver::validate_next(Condition *condition, StatementAst *statement) {
    return false;
}

template <typename Container>
bool NextSolver::validate_container_next(Container *container, StatementAst *statement) {
    return false;
}

template <typename Container>
StatementSet NextSolver::solve_container_next(Container *container) {
    return StatementSet();
}

template <typename Ast>
StatementSet NextSolver::solve_last_previous(Ast *ast) {
    return StatementSet();
}

template <>
ConditionSet NextSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet NextSolver::solve_left<StatementAst>(StatementAst *ast);

template <>
StatementSet NextSolver::solve_next<StatementAst>(StatementAst *ast);

template <>
StatementSet NextSolver::solve_next<IfAst>(IfAst *ast);

template <>
StatementSet NextSolver::solve_next<WhileAst>(WhileAst *ast);

template <>
StatementSet NextSolver::solve_next<AssignmentAst>(AssignmentAst *ast);

template <>
StatementSet NextSolver::solve_next<CallAst>(CallAst *ast);

template <>
StatementSet NextSolver::solve_previous<StatementAst>(StatementAst *ast);

template <>
StatementSet NextSolver::solve_previous<WhileAst>(WhileAst *ast);

template <>
StatementSet NextSolver::solve_preceeding_previous<StatementAst>(StatementAst *ast);

template <>
StatementSet NextSolver::solve_preceeding_previous<WhileAst>(WhileAst *ast);

template <>
StatementSet NextSolver::solve_preceeding_previous<AssignmentAst>(AssignmentAst *ast);

template <>
StatementSet NextSolver::solve_preceeding_previous<CallAst>(CallAst *ast);

template <>
StatementSet NextSolver::solve_preceeding_previous<IfAst>(IfAst *ast);

template <>
bool NextSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2);

template <>
bool NextSolver::validate_next<IfAst>(IfAst *condition, StatementAst *statement);

template <>
bool NextSolver::validate_next<WhileAst>(WhileAst *loop, StatementAst *statement);

template <>
bool NextSolver::validate_next<AssignmentAst>(AssignmentAst *assign, StatementAst *statement);

template <>
bool NextSolver::validate_next<CallAst>(CallAst *call, StatementAst *statement);

template <>
bool NextSolver::validate_container_next<ContainerAst>(ContainerAst *container, StatementAst *statement);

template <>
bool NextSolver::validate_container_next<IfAst>(IfAst *container, StatementAst *statement);

template <>
bool NextSolver::validate_container_next<WhileAst>(WhileAst *container, StatementAst *statement);


template <>
StatementSet NextSolver::solve_container_next<ContainerAst>(ContainerAst *container);

template <>
StatementSet NextSolver::solve_container_next<IfAst>(IfAst *condition);

template <>
StatementSet NextSolver::solve_container_next<WhileAst>(WhileAst *loop);

template <>
StatementSet NextSolver::solve_last_previous<StatementAst>(StatementAst *statement);

template <>
StatementSet NextSolver::solve_last_previous<WhileAst>(WhileAst *loop);

template <>
StatementSet NextSolver::solve_last_previous<AssignmentAst>(AssignmentAst *assign);

template <>
StatementSet NextSolver::solve_last_previous<CallAst>(CallAst *call);

} // namespace impl
} // namespace simple
