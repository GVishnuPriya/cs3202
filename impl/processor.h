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

#pragma once

#include "simple/solver.h"
#include "simple/predicate.h"
#include "simple/query.h"
#include "simple/linker.h"
#include "simple/util/query_utils.h"

namespace simple {
namespace impl {

class QueryProcessor {
  public:
    QueryProcessor(
            const std::shared_ptr<QueryLinker>& linker,
            std::map<Qvar, PredicatePtr> predicates,
            PredicatePtr wildcard_pred);

    std::shared_ptr<QueryLinker> get_linker() {
        return _linker;
    }

    void solve_clause(PqlClause *clause);

    template <typename Term1, typename Term2>
    void solve_clause(QuerySolver *solver, Term1 *term1, Term2 *term2);

    ConditionSet get_qvar(const std::string& qvar);

    void set_qvar(const std::string& qvar, const ConditionSet& conditions);

    SimplePredicate* get_predicate(const std::string& qvar);

  private:
    std::shared_ptr<QueryLinker>        _linker;
    std::map<std::string, PredicatePtr> _predicates;
    PredicatePtr    _wildcard_pred;
};

/*
 * Solver(condition, condition)
 *
 * This is a simple validate query. Simply call Solver->validate()
 */
template <>
void QueryProcessor::solve_clause<PqlConditionTerm, PqlConditionTerm>(
        QuerySolver *solver, 
        PqlConditionTerm *term1, PqlConditionTerm *term2);

/*
 * Solver(qvar, qvar)
 *
 * This is a solve both query variable query. It is the most complicated
 * among all. The only correct way to solve this is go through all
 * possible combinations and validate the combinations.
 */
template <>
void QueryProcessor::solve_clause<PqlVariableTerm, PqlVariableTerm>(
        QuerySolver *solver,
        PqlVariableTerm *term1, PqlVariableTerm *term2);

/*
 * Solver(_, _)
 *
 * The way to solve this both wildcard term clause is not clear at 
 * the moment. For now the query is validated if there is at least 
 * one possible condition pair that match the solver.
 */
template <>
void QueryProcessor::solve_clause<PqlWildcardTerm, PqlWildcardTerm>(
        QuerySolver *solver,
        PqlWildcardTerm *term1, PqlWildcardTerm *term2);

/*
 * Solver(qvar, condition)
 *
 * This is a solve left query. Simply call Solver->solve_left().
 */
template <>
void QueryProcessor::solve_clause<PqlVariableTerm, PqlConditionTerm>(
        QuerySolver *solver,
        PqlVariableTerm *term1, PqlConditionTerm *term2);

/*
 * Solver(condition, qvar)
 *
 * This is a solve right query. Simply call Solver->solve_right().
 */
template <>
void QueryProcessor::solve_clause<PqlConditionTerm, PqlVariableTerm>(
        QuerySolver *solver,
        PqlConditionTerm *term1, PqlVariableTerm *term2);

/*
 * Solver(qvar, _)
 *
 * This is to find all possible left conditions that have results at
 * the right. Call Solver->solve_right() on all possible conditions in
 * left qvar and if the result condition set is non-empty, keep that
 * condition.
 */
template <>
void QueryProcessor::solve_clause<PqlVariableTerm, PqlWildcardTerm>(
        QuerySolver *solver,
        PqlVariableTerm *term1, PqlWildcardTerm *term2);

/*
 * Solver(_, qvar)
 *
 * This is to find all possible right conditions that have results at
 * the left. Call Solver->solve_left() on all possible conditions in
 * right qvar and if the result condition set is non-empty, keep that
 * condition.
 */
template <>
void QueryProcessor::solve_clause<PqlWildcardTerm, PqlVariableTerm>(
        QuerySolver *solver,
        PqlWildcardTerm *term1, PqlVariableTerm *term2);

/*
 * Solver(condition, _)
 *
 * This is also a validate query. Call Solver->solve_right() and see
 * if the returned condition set is non-empty.
 */
template <>
void QueryProcessor::solve_clause<PqlConditionTerm, PqlWildcardTerm>(
        QuerySolver *solver,
        PqlConditionTerm *term1, PqlWildcardTerm *term2);

/*
 * Solver(_, condition)
 *
 * This is also a validate query. Call Solver->solve_left() and see
 * if the returned condition set is non-empty.
 */
template <>
void QueryProcessor::solve_clause<PqlWildcardTerm, PqlConditionTerm>(
        QuerySolver *solver,
        PqlWildcardTerm *term1, PqlConditionTerm *term2);



} // namespace impl
} // namespace simple
