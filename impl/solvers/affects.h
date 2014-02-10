
#pragma once

#include <map>
#include <memory>
#include <utility>
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class AffectsSolver {
  public:
    AffectsSolver(std::shared_ptr<NextQuerySolver> next_solver);

    template <typename Condition>
    StatementSet solve_affected_statements(Condition *statement);

    template <typename Condition>
    StatementSet solve_affecting_statements(Condition *statement);

    bool validate_affect(StatementAst *statement1, StatementAst *statement2);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    StatementSet solve_next_affected_by_var(SimpleVariable var, StatementAst *statement);
    
    template <typename Condition>
    StatementSet solve_affected_by_var(SimpleVariable var, Condition *condition);

    StatementSet solve_prev_affecting_with_var(SimpleVariable var, StatementAst *statement);

    template <typename Condition>
    StatementSet solve_affecting_with_var(SimpleVariable var, Condition *statement);
    
  private:
    std::shared_ptr<NextQuerySolver> _next_solver;

    std::map< std::pair<SimpleVariable, StatementAst*>, StatementSet> 
    _affected_by_var_cache;
};

template <typename Condition>
StatementSet AffectsSolver::solve_affected_statements(Condition *statement) {
    return StatementSet();
}

template <typename Condition>
StatementSet AffectsSolver::solve_affecting_statements(Condition *statement) {
    return StatementSet();
}

template <typename Condition1, typename Condition2>
bool AffectsSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <typename Condition>
ConditionSet AffectsSolver::solve_right(Condition *condition) {
    return ConditionSet(); // empty set
}

template <typename Condition>
ConditionSet AffectsSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <>
StatementSet AffectsSolver::solve_affected_by_var<StatementAst>(
    SimpleVariable var, StatementAst *statement);

template <>
StatementSet AffectsSolver::solve_affected_by_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement);

template <>
StatementSet AffectsSolver::solve_affected_by_var<IfAst>(
    SimpleVariable var, IfAst *statement);

template <>
StatementSet AffectsSolver::solve_affected_by_var<WhileAst>(
    SimpleVariable var, WhileAst *statement);

template <>
StatementSet AffectsSolver::solve_affected_by_var<CallAst>(
    SimpleVariable var, CallAst *statement);

template <>
StatementSet AffectsSolver::solve_affecting_with_var<StatementAst>(
    SimpleVariable var, StatementAst *statement);

template <>
StatementSet AffectsSolver::solve_affecting_with_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement);

template <>
StatementSet AffectsSolver::solve_affecting_with_var<IfAst>(
    SimpleVariable var, IfAst *statement);

template <>
StatementSet AffectsSolver::solve_affecting_with_var<WhileAst>(
    SimpleVariable var, WhileAst *statement);

template <>
StatementSet AffectsSolver::solve_affecting_with_var<CallAst>(
    SimpleVariable var, CallAst *statement);

}
}