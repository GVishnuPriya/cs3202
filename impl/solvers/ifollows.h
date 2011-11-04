
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class IFollowSolver {
  public:
    IFollowSolver(SimpleRoot ast) : _ast(ast) { }

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
ConditionSet IFollowSolver::solve_right<StatementAst>(StatementAst *statement) {
    ConditionSet result;

    if(statement->next()) {
        while(statement->next() != NULL) {
            result.insert(new SimpleStatementCondition(statement->next()));
            statement = statement->next();
        }
    }
    return result;
}
    
template <>
ConditionSet IFollowSolver::solve_left<StatementAst>(StatementAst *statement) {
    ConditionSet result;

    if(statement->prev()) {
        while(statement->prev() != NULL) {
            result.insert(new SimpleStatementCondition(statement->prev()));
            statement = statement->prev();
        }
    }
    return result;
}

template <>
bool IFollowSolver::validate<StatementAst, StatementAst>(
        StatementAst *left, StatementAst *right)
{
    while(left->next() != NULL) {
        if(left->next() == right) {
            return true;
        }
        left = left->next();
    }
    return false;
}

} // namespace impl
} // namespace simple
