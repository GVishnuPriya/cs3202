/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <list>
#include "impl/solvers/inext.h"
#include "impl/condition.h"
#include "simple/util/set_convert.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple::util;

template <>
ConditionSet INextSolver::solve_right<StatementAst>(StatementAst *statement) {
    return statement_set_to_condition_set(solve_next_statement(statement));
}

template <>
ConditionSet INextSolver::solve_left<StatementAst>(StatementAst *statement) {
    return statement_set_to_condition_set(solve_prev_statement(statement));
}

StatementSet INextSolver::solve_next_statement(StatementAst *statement) {
    if(_inext_cache.count(statement) > 0) return _inext_cache[statement];

    _visit_cache.clear();
    StatementSet results = to_statement_set(
        solve_inext(statement, CallStack()));

    _inext_cache[statement] = results;

    return results;
}

StatementSet INextSolver::solve_prev_statement(StatementAst *statement) {
    if(_iprev_cache.count(statement) > 0) return _iprev_cache[statement];

    _visit_cache.clear();
    StatementSet results = to_statement_set(
        solve_iprev(statement, CallStack()));

    _iprev_cache[statement] = results;
    return results;
}

StackedStatementSet INextSolver::solve_inext(
    StatementAst *statement, CallStack callstack) 
{
    StackedStatementSet results;

    StackedStatement current_trace(statement, callstack);
    if(_visit_cache.count(current_trace)) return StackedStatementSet();
    _visit_cache.insert(current_trace);

    StackedStatementSet direct_next = _next_solver->solve_next_bip_statement(
        statement, callstack);
    
    union_set(results, direct_next);

    for(auto it = direct_next.begin(); it != direct_next.end(); ++it) {
        union_set(results, solve_inext(it->first, it->second));
    }

    return results;
}

StackedStatementSet INextSolver::solve_iprev(
    StatementAst *statement, CallStack callstack) 
{
    StackedStatementSet results;

    StackedStatement current_trace(statement, callstack);
    if(_visit_cache.count(current_trace)) return StackedStatementSet();
    _visit_cache.insert(current_trace);

    StackedStatementSet direct_prev = _next_solver->solve_prev_bip_statement(
        statement, callstack);

    union_set(results, direct_prev);

    for(auto it = direct_prev.begin(); it != direct_prev.end(); ++it) {
        union_set(results, solve_iprev(it->first, it->second));
    }

    return results;
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
