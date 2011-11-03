
#pragma once

#include <map>
#include "simple/solver.h"
#include "simple/condition_set.h"
#include "simple/ast.h"

namespace simple {
namespace impl {

using namespace simple;

class INextSolver {
  public:
    typedef std::set<StatementAst*>                 StatementSet;
    typedef std::map<StatementAst*, StatementSet >  INextTable;

    NextSolver(SimpleRoot ast, SolverTable *table) :
        _ast(ast), _next_solver(table->get_solver("Next"))
    { }


    const StatementSet& solve_inext(StatementAst *statement) {
        if(is_next_cached(statement)) {
            return _inext_cache[statement];
        } else {
            StatementSet& current_set = _inext_cache[statement]; // initialize
            StatementSet direct_next = solve_direct_next(statement);
            for(StatementSet::iterator it = direct_next.begin();
                it != direct_next.end(); ++it)
            {
                union_set(current_set, solve_inext(*it));
            }

            return current_set;
        }
    }

    const StatementSet& solve_iprev(StatementAst *statement) {
        if(is_prev_cached(statement)) {
            return _iprev_cache[statement];
        } else {

        }
    }

    bool is_next_cached(StatementAst *statement) {
        return _inext_cache.count(statement) > 0;
    }

    bool is_prev_cached(StatementAst *statement) {
        return _iprev_cache.count(statement) > 0;
    }


  private:
    SimpleRoot _ast;
    QuerySolver *_next_solver;
    INextTable _inext_cache;
    INextTable _iprev_cache;
};

template <>
StatementSet index_next<StatementAst>(StatementAst *statement) {
    SimpleStatementCondition condition(statement);
    ConditionSet direct_next = _next_solver->solve_right

}

template <>
validate INextSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2)
{
    return solve_inext(statement1).count(statment2) > 0;
}

}
}
