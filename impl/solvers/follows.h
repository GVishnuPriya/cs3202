
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"

namespace simple {
namespace impl {
namespace solver {

class FollowsSolver {
  public:
    FollowsSolver(SimpleRoot ast, SolverTable *table) : _ast(ast) { }

    /*
     * SOLVE RIGHT PART
     */
    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return ConditionSet();
    }

    template <>
    ConditionSet solve_right<StatementAst>(StatementAst *ast) {
        ConditionSet result;

        result.insert(new StatementCondition(ast->next()));
        return result;
    }

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return ConditionSet();
    }
    
    template <>
    ConditionSet solve_left<StatementAst>(StatementAst *ast) {
        ConditionSet result;

        result.insert(new StatementCondition(ast->prev()));
        return result;
    }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2) {
        return false;
    }

    template <>
    bool validate<StatementAst, StatementAst>(
            StatementAst *left, StatementAst *right)
    {
        return left->next() == right;
    }
};



} // namespace solver
} // namespace impl
} // namespace simple
