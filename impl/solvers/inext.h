
#pragma once

#include <map>
#include "simple/solver.h"
#include "impl/solvers/next.h"
#include "simple/condition_set.h"
#include "simple/ast.h"

namespace simple {
namespace impl {

using namespace simple;

class INextSolver {
  public:
    typedef std::map<StatementAst*, StatementSet>  INextTable;

    INextSolver(SimpleRoot ast, NextQuerySolver *solver) :
        _ast(ast), _next_solver(solver)
    { }

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

    void solve_inext(StatementAst *statement, StatementSet& results);
    void solve_iprev(StatementAst *statement, StatementSet& results);

  private:
    SimpleRoot _ast;
    NextQuerySolver *_next_solver;
    INextTable _inext_cache;
    INextTable _iprev_cache;
    INextTable _inext_partial_cache;
    INextTable _iprev_partial_cache;
};

template <>
bool INextSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2);

template <>
ConditionSet INextSolver::solve_right<StatementAst>(StatementAst *statement);

template <>
ConditionSet INextSolver::solve_left<StatementAst>(StatementAst *statement);

}
}
