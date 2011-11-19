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

#include "simple/util/query_utils.h"

namespace simple {
namespace util {

class SameTermComparator {
  public:
    template <typename Term1, typename Term2>
    bool is_same_term(Term1 *term1, Term2 *term2);
};

class SameTermVisitorTraits {
  public:
    typedef bool ResultType;

    template <typename Term1, typename Term2>
    static bool visit(SameTermComparator *comparator, Term1 *term1, Term2 *term2) {
        return comparator->is_same_term<Term1, Term2>(term1, term2);
    }
};

template <typename Term1, typename Term2>
bool SameTermComparator::is_same_term(Term1 *term1, Term2 *term2) {
    return false;
}

template <>
bool SameTermComparator::is_same_term<PqlConditionTerm, PqlConditionTerm>(
        PqlConditionTerm *term1, PqlConditionTerm *term2);

template <>
bool SameTermComparator::is_same_term<PqlVariableTerm, PqlVariableTerm>(
        PqlVariableTerm *term1, PqlVariableTerm *term2);

template <>
bool SameTermComparator::is_same_term<PqlWildcardTerm, PqlWildcardTerm>(
        PqlWildcardTerm *term1, PqlWildcardTerm *term2);



bool is_same_term(PqlTerm *term1, PqlTerm *term2) {
    SameTermComparator comparator;
    return double_dispatch_pql_terms<
        SameTermComparator, SameTermVisitorTraits>(
        &comparator, term1, term2);
}

bool is_same_clause(PqlClause *clause1, PqlClause *clause2) {
    return clause1->get_solver() == clause2->get_solver() &&
        is_same_term(clause1->get_left_term(), clause2->get_left_term()) &&
        is_same_term(clause1->get_right_term(), clause2->get_right_term());
}


template <>
bool SameTermComparator::is_same_term<PqlConditionTerm, PqlConditionTerm>(
        PqlConditionTerm *term1, PqlConditionTerm *term2) 
{
    return term1->get_condition() == term2->get_condition();
}

template <>
bool SameTermComparator::is_same_term<PqlVariableTerm, PqlVariableTerm>(
        PqlVariableTerm *term1, PqlVariableTerm *term2)
{
    return term1->get_query_variable() == term2->get_query_variable();
}

template <>
bool SameTermComparator::is_same_term<PqlWildcardTerm, PqlWildcardTerm>(
        PqlWildcardTerm *term1, PqlWildcardTerm *term2)
{
    return true;
}







}
}
