
#include <list>
#include "impl/solvers/inext.h"
#include "impl/condition.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

template <>
ConditionSet INextSolver::solve_right<StatementAst>(StatementAst *statement) {
    StatementSet statements;
    std::list<StatementSet*> list;
    list.push_back(&statements);
    solve_inext(statement, list);

    ConditionSet result;

    for(StatementSet::iterator it = statements.begin(); 
            it != statements.end(); ++it)
    {
        result.insert(new SimpleStatementCondition(*it));
    }

    return result;
}

template <>
ConditionSet INextSolver::solve_left<StatementAst>(StatementAst *statement) {
    StatementSet statements;
    std::list<StatementSet*> list;
    list.push_back(&statements);
    solve_iprev(statement, list);

    ConditionSet result;

    for(StatementSet::iterator it = statements.begin(); 
            it != statements.end(); ++it)
    {
        result.insert(new SimpleStatementCondition(*it));
    }

    return result;
}

void INextSolver::solve_inext(StatementAst *statement, std::list<StatementSet*> results) {
    if(_inext_cache.count(statement) > 0 && _inext_cache[statement].size() > 0) {
        for(std::list<StatementSet*>::iterator it = results.begin(); 
            it != results.end(); ++it)
        {
            union_set(*(*it), _inext_cache[statement]);
        }
    } else {
        StatementSet direct_next = _next_solver->solve_next(statement);
        if(direct_next.size() == 0) {
             return;
        } else if(direct_next.size() == 1) { 
            results.push_back(&_inext_cache[statement]);
            index_inext(statement, results);
        } else {
            StatementSet temp;
            results.push_back(&temp);
            index_inext(statement, results);
            _inext_cache[statement] = std::move(temp);
        }
    }
}

void INextSolver::solve_iprev(StatementAst *statement, std::list<StatementSet*> results) {
    if(_iprev_cache.count(statement) > 0 && _iprev_cache[statement].size() > 0) {
        for(std::list<StatementSet*>::iterator it = results.begin(); 
            it != results.end(); ++it)
        {
            union_set(*(*it), _iprev_cache[statement]);
        }
    } else {
        StatementSet direct_prev = _next_solver->solve_previous(statement);
        if(direct_prev.size() == 0) {
             return;
        } else if(direct_prev.size() == 1) { 
            results.push_back(&_iprev_cache[statement]);
            index_iprev(statement, results);
        } else {
            StatementSet temp;
            results.push_back(&temp);
            index_iprev(statement, results);
            _iprev_cache[statement] = std::move(temp);
        }
    }
}

void INextSolver::index_inext(StatementAst *statement, const std::list<StatementSet*>& results) {
    StatementSet direct_next = _next_solver->solve_next(statement);
    for(StatementSet::iterator it = direct_next.begin();
        it != direct_next.end(); ++it)
    {
        for(std::list<StatementSet*>::const_iterator it2 = results.begin();
            it2 != results.end(); ++it2)
        {
            (*it2)->insert(*it);
        }
        solve_inext(*it, results);
    }
}


void INextSolver::index_iprev(StatementAst *statement, const std::list<StatementSet*>& results) {
    StatementSet direct_prev = _next_solver->solve_previous(statement);
    for(StatementSet::iterator it = direct_prev.begin();
        it != direct_prev.end(); ++it)
    {
        for(std::list<StatementSet*>::const_iterator it2 = results.begin();
            it2 != results.end(); ++it2)
        {
            (*it2)->insert(*it);
        }
        solve_iprev(*it, results);
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
