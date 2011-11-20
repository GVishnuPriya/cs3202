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

#include "impl/processor.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class SolveClauseVisitorTraits {
  public:
    typedef bool            ResultType;
    typedef QuerySolver*    ContextType;

    template <typename Term1, typename Term2>
    static bool visit(QueryProcessor *processor, 
            Term1 *term1, Term2 *term2, QuerySolver *solver)
    {
        processor->solve_clause<Term1, Term2>(solver, term1, term2);
        return true;
    }
};

void QueryProcessor::solve_clause(PqlClause *clause) {
    double_dispatch_pql_terms<QueryProcessor, SolveClauseVisitorTraits>(
            this, clause->get_left_term(), clause->get_right_term(),
            clause->get_solver());
}

/*
 * Solver(condition, condition)
 */
template <>
void QueryProcessor::solve_clause<PqlConditionTerm, PqlConditionTerm>(
        QuerySolver *solver, 
        PqlConditionTerm *term1, PqlConditionTerm *term2)
{
    if(!solver->validate(term1->get_condition().get(), 
                term2->get_condition().get()))
    {
        _linker->invalidate_state();
    }
}

/*
 * Solver(qvar, qvar)
 */
template <>
void QueryProcessor::solve_clause<PqlVariableTerm, PqlVariableTerm>(
        QuerySolver *solver,
        PqlVariableTerm *term1, PqlVariableTerm *term2)
{
    std::string qvar1 = term1->get_query_variable();
    std::string qvar2 = term2->get_query_variable();

    if(qvar1 == qvar2) {
        ConditionSet conditions = get_qvar(qvar1);
        ConditionSet new_conditions;

        for(ConditionSet::iterator cit = conditions.begin();
                cit != conditions.end(); ++cit)
        {
            if(solver->validate(*cit, *cit)) {
                new_conditions.insert(*cit);
            }
        }
        _linker->update_results(qvar1, new_conditions);
    } else {
        ConditionSet conditions1 = get_qvar(qvar1);
        ConditionSet conditions2 = get_qvar(qvar2);

        std::vector<ConditionPair> links;

        for(ConditionSet::iterator cit1 = conditions1.begin();
                cit1 != conditions1.end(); ++cit1)
        {
            for(ConditionSet::iterator cit2 = conditions2.begin();
                    cit2 != conditions2.end(); ++cit2)
            {
                if(solver->validate(*cit1, *cit2)) {
                    links.push_back(ConditionPair(*cit1, *cit2));
                }
            }
        }
        _linker->update_links(qvar1, qvar2, links);
    }

}

/*
 * Solver(_, _)
 */
template <>
void QueryProcessor::solve_clause<PqlWildcardTerm, PqlWildcardTerm>(
        QuerySolver *solver,
        PqlWildcardTerm *term1, PqlWildcardTerm *term2)
{
    ConditionSet left = _wildcard_pred->global_set();
    for(ConditionSet::iterator it = left.begin();
            it != left.end(); ++it)
    {
        if(!solver->solve_left(*it).is_empty()) {
            return;
        }
    }

    _linker->invalidate_state();
}

/*
 * Solver(qvar, condition)
 */
template <>
void QueryProcessor::solve_clause<PqlVariableTerm, PqlConditionTerm>(
        QuerySolver *solver,
        PqlVariableTerm *term1, PqlConditionTerm *term2)
{
    ConditionSet left = solver->solve_left(term2->get_condition().get());
    set_qvar(term1->get_query_variable(), left);
}

/*
 * Solver(condition, qvar)
 */
template <>
void QueryProcessor::solve_clause<PqlConditionTerm, PqlVariableTerm>(
        QuerySolver *solver,
        PqlConditionTerm *term1, PqlVariableTerm *term2)
{

    ConditionSet right = solver->solve_right(term1->get_condition().get());
    set_qvar(term2->get_query_variable(), right);
}

/*
 * Solver(qvar, _)
 */
template <>
void QueryProcessor::solve_clause<PqlVariableTerm, PqlWildcardTerm>(
        QuerySolver *solver,
        PqlVariableTerm *term1, PqlWildcardTerm *term2)
{
    std::string qvar = term1->get_query_variable();
    ConditionSet left_conditions = get_qvar(qvar);

    ConditionSet new_left;
    for(ConditionSet::iterator cit = left_conditions.begin();
            cit != left_conditions.end(); ++cit)
    {
        if(!solver->solve_right(*cit).is_empty()) {
            new_left.insert(*cit);
        }
    }

    set_qvar(qvar, new_left);
}

/*
 * Solver(_, qvar)
 */
template <>
void QueryProcessor::solve_clause<PqlWildcardTerm, PqlVariableTerm>(
        QuerySolver *solver,
        PqlWildcardTerm *term1, PqlVariableTerm *term2)
{
    std::string qvar = term2->get_query_variable();
    ConditionSet right_conditions = get_qvar(qvar);

    ConditionSet new_right;
    for(ConditionSet::iterator cit = right_conditions.begin();
            cit != right_conditions.end(); ++cit)
    {
        if(!solver->solve_left(*cit).is_empty()) {
            new_right.insert(*cit);
        }
    }

    set_qvar(qvar, new_right);
}

/*
 * Solver(condition, _)
 */
template <>
void QueryProcessor::solve_clause<PqlConditionTerm, PqlWildcardTerm>(
        QuerySolver *solver,
        PqlConditionTerm *term1, PqlWildcardTerm *term2)
{
    if(solver->solve_right(term1->get_condition()).is_empty()) {
        _linker->invalidate_state();
    }
}

/*
 * Solver(_, condition)
 */
template <>
void QueryProcessor::solve_clause<PqlWildcardTerm, PqlConditionTerm>(
        QuerySolver *solver,
        PqlWildcardTerm *term1, PqlConditionTerm *term2)
{
    if(solver->solve_left(term2->get_condition()).is_empty()) {
        _linker->invalidate_state();
    }
}

ConditionSet QueryProcessor::get_qvar(const std::string& qvar) {
    return _linker->get_conditions(qvar, get_predicate(qvar));
}

void QueryProcessor::set_qvar(const std::string& qvar, 
        const ConditionSet& conditions)
{
    // Initialize the qvar first
    if(!_linker->is_initialized(qvar)) {
        _linker->update_results(qvar, get_predicate(qvar)->global_set());
    }

    _linker->update_results(qvar, conditions);
}

SimplePredicate* QueryProcessor::get_predicate(const std::string& qvar) 
{
    if(_predicates.count(qvar) > 0) {
        return _predicates[qvar].get();
    } else {
        return _wildcard_pred.get();
    }
}



} // namespace impl
} // namespace simple
