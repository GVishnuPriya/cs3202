
#pragma once

#include <map>
#include <memory>
#include <utility>
#include "simple/solver.h"
#include "simple/next.h"
#include "impl/solvers/modifies.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class AffectsSolver {
  public:
    AffectsSolver(std::shared_ptr<NextBipQuerySolver> next_solver,
        std::shared_ptr<ModifiesSolver> modifies_solver);

    virtual StackedStatementSet solve_affected_by_var_assignment(
        SimpleVariable var, AssignmentAst *statement, CallStack callstack);

    virtual StackedStatementSet solve_affecting_with_var_assignment(
        SimpleVariable var, AssignmentAst *statement, CallStack callstack);

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

    StackedStatementSet solve_next_affected_by_var(
        SimpleVariable var, StatementAst *statement, CallStack callstack);
    
    template <typename Condition>
    StackedStatementSet solve_affected_by_var(
        SimpleVariable var, Condition *condition, CallStack callstack);

    StackedStatementSet solve_prev_affecting_with_var(
        SimpleVariable var, StatementAst *statement, CallStack callstack);

    template <typename Condition>
    StackedStatementSet solve_affecting_with_var(
        SimpleVariable var, Condition *statement, CallStack callstack);
    
  protected:
    std::shared_ptr<NextBipQuerySolver> _next_solver;
    std::shared_ptr<ModifiesSolver> _modifies_solver;

    std::map< StatementAst*, StatementSet > _affected_statements_cache;
    std::map< StatementAst*, StatementSet > _affecting_statements_cache;

    std::set< std::pair<SimpleVariable, StackedStatement> > 
    _visit_cache;
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
StackedStatementSet AffectsSolver::solve_affected_by_var<StatementAst>(
    SimpleVariable var, StatementAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<IfAst>(
    SimpleVariable var, IfAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<WhileAst>(
    SimpleVariable var, WhileAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<CallAst>(
    SimpleVariable var, CallAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<StatementAst>(
    SimpleVariable var, StatementAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<IfAst>(
    SimpleVariable var, IfAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<WhileAst>(
    SimpleVariable var, WhileAst *statement, CallStack callstack);

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<CallAst>(
    SimpleVariable var, CallAst *statement, CallStack callstack);


template <>
ConditionSet AffectsSolver::solve_right<StatementAst>(StatementAst *affected);

template <>
ConditionSet AffectsSolver::solve_left<StatementAst>(StatementAst *affecting);

template <>
bool AffectsSolver::validate<StatementAst, StatementAst>(
    StatementAst *affecting, StatementAst *affected);

}
}