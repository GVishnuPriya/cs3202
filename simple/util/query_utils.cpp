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

template <typename Term1, typename Term2>
bool is_same_term(Term1 *term1, Term2 *term2) {
    return false;
}

template <>
bool is_same_term<PqlConditionTerm, PqlConditionTerm>(
        PqlConditionTerm *term1, PqlConditionTerm *term2) 
{
    return term1->get_condition() == term2->get_condition();
}

template <>
bool is_same_term<PqlVariableTerm, PqlVariableTerm>(
        PqlVariableTerm *term1, PqlVariableTerm *term2)
{
    return term1->get_query_variable() == term2->get_query_variable();
}

template <>
bool is_same_term<PqlWildcardTerm, PqlWildcardTerm>(
        PqlWildcardTerm *term1, PqlWildcardTerm *term2)
{
    return true;
}


class SameTermVisitorTraits {
  public:
    typedef bool ResultType;
    typedef void* ContextType;

    template <typename Term1, typename Term2>
    static bool visit(int *comparator, Term1 *term1, Term2 *term2, void* context) {
        return is_same_term<Term1, Term2>(term1, term2);
    }
};

bool is_same_term(PqlTerm *term1, PqlTerm *term2) {
    return double_dispatch_pql_terms<
        int, SameTermVisitorTraits>(NULL, term1, term2, NULL);
}

bool is_same_clause(PqlClause *clause1, PqlClause *clause2) {
    return clause1->get_solver() == clause2->get_solver() &&
        is_same_term(clause1->get_left_term(), clause2->get_left_term()) &&
        is_same_term(clause1->get_right_term(), clause2->get_right_term());
}

template <typename Term1, typename Term2>
bool is_less_than_term(Term1 *term1, Term2 *term2) {
    return false;
}

template <>
bool is_less_than_term<PqlConditionTerm, PqlWildcardTerm>(
        PqlConditionTerm *term1, PqlWildcardTerm *term2)
{
    return true;
}

template <>
bool is_less_than_term<PqlConditionTerm, PqlVariableTerm>(
        PqlConditionTerm *term1, PqlVariableTerm *term2)
{
    return true;
}

template <>
bool is_less_than_term<PqlWildcardTerm, PqlVariableTerm>(
        PqlWildcardTerm *term1, PqlVariableTerm *term2)
{
    return true;
}

template <>
bool is_less_than_term<PqlConditionTerm, PqlConditionTerm>(
        PqlConditionTerm *term1, PqlConditionTerm *term2)
{
    return term1->get_condition() < term2->get_condition();
}

template <>
bool is_less_than_term<PqlVariableTerm, PqlVariableTerm>(
        PqlVariableTerm *term1, PqlVariableTerm *term2)
{
    return term1->get_query_variable() < term2->get_query_variable();
}

class LessThanTermVisitorTraits {
  public:
    typedef bool    ResultType;
    typedef void*   ContextType;

    template <typename Term1, typename Term2>
    static bool visit(int *visitor, Term1 *term1, Term2 *term2, void *context) {
        return is_less_than_term<Term1, Term2>(term1, term2);
    }
};

bool is_less_than_term(PqlTerm *term1, PqlTerm *term2) {
    return double_dispatch_pql_terms<int, LessThanTermVisitorTraits>(
            NULL, term1, term2, NULL);
}



} // namespace util
} // namespace simple
