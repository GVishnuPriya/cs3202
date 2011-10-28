
#pragma once

#include "simple/ast.h"
#include "simple/util/ast_utils.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;

class NextSolver {
  public:
    NextSolver(SimpleRoot ast, SolverTable *table) : _ast(ast) { }

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    ConditionSet solve_previous(Condition *condition);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    bool validate_next(Condition *condition, StatementAst *statement);

    template <typename Container>
    bool validate_container_next(Container *container, StatementAst *statement);

    template <typename Container>
    ConditionSet solve_container_next(Container *container);
       
    template <typename Ast>
    ConditionSet solve_last_previous(Ast *ast);

  private:
    SimpleRoot _ast;
};

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
ConditionSet NextSolver::solve_previous(Condition *condition) {
    return ConditionSet();
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
ConditionSet NextSolver::solve_container_next(Container *container) {
    return ConditionSet();
}

template <typename Ast>
ConditionSet NextSolver::solve_last_previous(Ast *ast) {
    return ConditionSet();
}

template <>
ConditionSet NextSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet NextSolver::solve_right<ConditionalAst>(ConditionalAst *ast);

template <>
ConditionSet NextSolver::solve_right<WhileAst>(WhileAst *ast);

template <>
ConditionSet NextSolver::solve_right<AssignmentAst>(AssignmentAst *ast);

template <>
ConditionSet NextSolver::solve_right<CallAst>(CallAst *ast);

template <>
ConditionSet NextSolver::solve_left<StatementAst>(StatementAst *ast);

template <>
ConditionSet NextSolver::solve_previous<StatementAst>(StatementAst *ast);

template <>
ConditionSet NextSolver::solve_previous<WhileAst>(WhileAst *ast);

template <>
ConditionSet NextSolver::solve_previous<AssignmentAst>(AssignmentAst *ast);

template <>
ConditionSet NextSolver::solve_previous<CallAst>(CallAst *ast);

template <>
ConditionSet NextSolver::solve_previous<ConditionalAst>(ConditionalAst *ast);

template <>
bool NextSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2);

template <>
bool NextSolver::validate_next<ConditionalAst>(ConditionalAst *condition, StatementAst *statement);

template <>
bool NextSolver::validate_next<WhileAst>(WhileAst *loop, StatementAst *statement);

template <>
bool NextSolver::validate_next<AssignmentAst>(AssignmentAst *assign, StatementAst *statement);

template <>
bool NextSolver::validate_next<CallAst>(CallAst *call, StatementAst *statement);

template <>
bool NextSolver::validate_container_next<ContainerAst>(ContainerAst *container, StatementAst *statement);

template <>
bool NextSolver::validate_container_next<ConditionalAst>(ConditionalAst *container, StatementAst *statement);

template <>
bool NextSolver::validate_container_next<WhileAst>(WhileAst *container, StatementAst *statement);


template <>
ConditionSet NextSolver::solve_container_next<ContainerAst>(ContainerAst *container);

template <>
ConditionSet NextSolver::solve_container_next<ConditionalAst>(ConditionalAst *condition);

template <>
ConditionSet NextSolver::solve_container_next<WhileAst>(WhileAst *loop);

template <>
ConditionSet NextSolver::solve_last_previous<StatementAst>(StatementAst *statement);

template <>
ConditionSet NextSolver::solve_last_previous<WhileAst>(WhileAst *loop);

template <>
ConditionSet NextSolver::solve_last_previous<AssignmentAst>(AssignmentAst *assign);

template <>
ConditionSet NextSolver::solve_last_previous<CallAst>(CallAst *call);

} // namespace impl
} // namespace simple
