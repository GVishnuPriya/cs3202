
#pragma once

#include <map>
#include <set>
#include <memory>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/solver.h"
#include "simple/util/ast_utils.h"

namespace simple {
namespace impl {

using namespace simple;

class CachedNextSolver : public NextQuerySolver {
  public:

    CachedNextSolver(SimpleRoot ast, std::shared_ptr<NextQuerySolver> next_solver);

    template <typename Condition>
    ConditionSet solve_right(StatementAst *ast);

    template <typename Condition>
    ConditionSet solve_left(StatementAst *ast);

    void index_proc(ProcAst *proc);

    void index_statement_list(StatementAst *statement);

    void index_statement(StatementAst *statement);

    bool validate(StatementAst *statement1, StatementAst *statement2);
    
  private:
    std::map< StatementAst*, StatementSet > _next_statement_index;
    std::map< StatementAst*, StatementSet > _prev_statement_index;

    SimpleRoot _ast; 
    std::shared_ptr<NextQuerySolver> _next_solver;
};

template <typename Condition>
ConditionSet CachedNextSolver::solve_right(StatementAst *ast) {
    return ConditionSet();
}

template <typename Condition>
ConditionSet CachedNextSolver::solve_left(StatementAst *ast) {
    return ConditionSet();
}

template <>
ConditionSet CachedNextSolver::solve_right<StatementAst>(StatementAst *ast);

template <>
ConditionSet CachedNextSolver::solve_left<StatementAst>(StatementAst *ast);

}
}


