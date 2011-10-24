
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"
#include "impl/condition.h"

namespace simple {
namespace impl {
namespace solver {

using namespace simple::ast;
using namespace simple::solver;
using namespace simple::condition;
using namespace simple::impl;

class FollowSolver {
  public:
    FollowSolver(SimpleRoot ast, SolverTable *table) : _ast(ast) { }

    /*
     * SOLVE RIGHT PART
     */
    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2) {
        return false;
    }

  private:
    SimpleRoot _ast;
};

template <>
ConditionSet FollowSolver::solve_right<StatementAst>(StatementAst *ast) {
    ConditionSet result;

    result.insert(new SimpleStatementCondition(ast->next()));
    return result;
}
    
template <>
ConditionSet FollowSolver::solve_left<StatementAst>(StatementAst *ast) {
    ConditionSet result;

    result.insert(new SimpleStatementCondition(ast->prev()));
    return result;
}

template <>
bool FollowSolver::validate<StatementAst, StatementAst>(
        StatementAst *left, StatementAst *right)
{
    return left->next() == right;
}

} // namespace solver
} // namespace impl
} // namespace simple
