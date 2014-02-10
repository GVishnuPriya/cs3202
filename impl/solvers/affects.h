
#pragma once

#include <map>
#include <memory>
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class AffectsSolver {
  public:
    AffectsSolver(std::shared_ptr<NextQuerySolver> next_solver,
      std::shared_ptr<UsesQuerySolver> uses_solver,
      std::shared_ptr<ModifiesQuerySolver> modifies_solver);

    StatementSet solve_affected_statements(StatementAst *statement);
    StatementSet solve_affecting_statements(StatementAst *statement);
    bool validate_affect(StatementAst *statement1, StatementAst *statement2);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

  private:
    StatementSet solve_next_affected_by_vars(const VariableSet& vars, StatementAst *statement);
    StatementSet solve_affected_by_vars(const VariableSet& vars, StatementAst *statement);
    StatementSet solve_prev_affecting_with_vars(const VariableSet& vars, StatementAst *statement);
    StatementSet solve_affecting_with_vars(const VariableSet& vars, StatementAst *statement);
    
    std::shared_ptr<NextQuerySolver> _next_solver;
    std::shared_ptr<UsesQuerySolver> _uses_solver;
    std::shared_ptr<ModifiesQuerySolver> _modifies_solver;
};

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

}
}