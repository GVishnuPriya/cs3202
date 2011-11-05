
#include <list>
#include "impl/solvers/inext.h"
#include "impl/condition.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

template <>
ConditionSet INextSolver::solve_right<StatementAst>(StatementAst *statement) {
    StatementSet result_stats;
    solve_inext(statement, result_stats);

    if(result_stats.size() == 0) {
        // initialize empty set cache
        _inext_cache[statement] = result_stats;
        return ConditionSet();
    }

    ConditionSet result;

    for(StatementSet::iterator it = result_stats.begin(); 
            it != result_stats.end(); ++it)
    {
        result.insert(new SimpleStatementCondition(*it));
    }

    // cache the result for future use
    _inext_cache[statement] = std::move(result_stats);
    return result;
}

template <>
ConditionSet INextSolver::solve_left<StatementAst>(StatementAst *statement) {
    StatementSet result_stats;
    solve_iprev(statement, result_stats);

    if(result_stats.size() == 0) {
        // initialize empty set cache
        _iprev_cache[statement] = result_stats;
        return ConditionSet();
    }

    ConditionSet result;

    for(StatementSet::iterator it = result_stats.begin(); 
            it != result_stats.end(); ++it)
    {
        result.insert(new SimpleStatementCondition(*it));
    }

    // cache the result for future use
    _iprev_cache[statement] = std::move(result_stats);
    return result;
}


void INextSolver::solve_inext(StatementAst *statement, StatementSet& results) {
    if(_inext_cache.count(statement) > 0) {
        union_set(results, _inext_cache[statement]);
        return;
    } else {
        StatementSet direct_next = _next_solver->solve_next_statement(statement);
        if(direct_next.size() == 0) {
            return;
        }
        for(StatementSet::iterator it = direct_next.begin();
            it != direct_next.end(); ++it)
        {
            if(results.count(*it) == 0) {
                results.insert(*it);
                solve_inext(*it, results);
            }
        }
    }
}

void INextSolver::solve_iprev(StatementAst *statement, StatementSet& results) {
    if(_iprev_cache.count(statement) > 0) {
        union_set(results, _iprev_cache[statement]);
        return;
    } else {
        StatementSet direct_prev = _next_solver->solve_prev_statement(statement);
        if(direct_prev.size() == 0) {
            return;
        }
        for(StatementSet::iterator it = direct_prev.begin();
            it != direct_prev.end(); ++it)
        {
            if(results.count(*it) == 0) {
                results.insert(*it);
                solve_iprev(*it, results);
            }
        }
    }
}

template <>
bool INextSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2)
{
    ConditionPtr condition = new SimpleStatementCondition(statement2);
    return solve_right<StatementAst>(statement1).has_element(condition);
}

}
}
