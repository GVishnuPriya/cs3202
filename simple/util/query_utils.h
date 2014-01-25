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

#include "simple/query.h"

namespace simple {
namespace util {

using namespace simple;

bool is_less_than_term(PqlTerm *term1, PqlTerm *term2);

bool is_same_term(PqlTerm *term1, PqlTerm *term2);

bool is_same_clause(PqlClause *clause1, PqlClause *clause2);


template <typename Visitor, typename VisitorTraits, typename FirstTerm>
class SecondPqlTermDoubleDispatcher : public PqlTermVisitor {
  public:
    SecondPqlTermDoubleDispatcher(Visitor *visitor, FirstTerm *term1,
            typename VisitorTraits::ContextType context) :
        _visitor(visitor), _term1(term1), _context(context)
    { }

    void visit_condition_term(PqlConditionTerm *term2) {
        dispatch_second_term<PqlConditionTerm>(term2);
    }

    void visit_variable_term(PqlVariableTerm *term2) {
        dispatch_second_term<PqlVariableTerm>(term2);
    }

    void visit_wildcard_term(PqlWildcardTerm *term2) {
        dispatch_second_term<PqlWildcardTerm>(term2);
    }

    typename VisitorTraits::ResultType
    return_result() {
        return std::move(_result);
    }

    template <typename SecondTerm>
    void dispatch_second_term(SecondTerm *term2) {
        _result = VisitorTraits::template visit<FirstTerm, SecondTerm>(
                _visitor, _term1, term2, _context);
    }

  private:
    Visitor     *_visitor;
    FirstTerm   *_term1;
    typename VisitorTraits::ResultType _result;
    typename VisitorTraits::ContextType _context;
};

template <typename Visitor, typename VisitorTraits>
class PqlTermDoubleDispatcher : public PqlTermVisitor {
  public:
    PqlTermDoubleDispatcher(Visitor *visitor, PqlTerm *term2,
            typename VisitorTraits::ContextType context) :
        _visitor(visitor), _term2(term2), _context(context)
    { }

    void visit_condition_term(PqlConditionTerm *term1) {
        dispatch_first_term<PqlConditionTerm>(term1);
    }

    void visit_variable_term(PqlVariableTerm *term1) {
        dispatch_first_term<PqlVariableTerm>(term1);
    }

    void visit_wildcard_term(PqlWildcardTerm *term1) {
        dispatch_first_term<PqlWildcardTerm>(term1);
    }

    typename VisitorTraits::ResultType
    return_result() {
        return std::move(_result);
    }

    template <typename Term>
    void dispatch_first_term(Term *term1) {
        SecondPqlTermDoubleDispatcher<
                Visitor, VisitorTraits, Term>
        visitor(_visitor, term1, _context);

        _term2->accept_pql_term_visitor(&visitor);
        _result = visitor.return_result();
    }

  private:
    Visitor *_visitor;
    PqlTerm *_term2;
    typename VisitorTraits::ResultType _result;
    typename VisitorTraits::ContextType _context;
};



template <typename Visitor, typename VisitorTraits>
typename VisitorTraits::ResultType
double_dispatch_pql_terms(Visitor *visitor, PqlTerm *term1, PqlTerm *term2,
        typename VisitorTraits::ContextType context)
{
    PqlTermDoubleDispatcher<Visitor, VisitorTraits> 
    dispatcher(visitor, term2, context);

    term1->accept_pql_term_visitor(&dispatcher);
    return dispatcher.return_result();
}


template <typename Selector1, typename Selector2>
inline bool same_selector_type() {
    return false;
}

template <>
inline bool same_selector_type<PqlSingleVarSelector, PqlSingleVarSelector>() {
    return true;
}

template <>
inline bool same_selector_type<PqlBooleanSelector, PqlBooleanSelector>() {
    return true;
}

template <>
inline bool same_selector_type<PqlTupleSelector, PqlTupleSelector>() {
    return true;
}

template <typename Selector>
class IsSelectorVisitor : public PqlSelectorVisitor {
  public:
    IsSelectorVisitor() : _result(false) { }

    void visit_single_var(PqlSingleVarSelector*) {
        _result = same_selector_type<Selector, 
                PqlSingleVarSelector>();
    }

    void visit_boolean(PqlBooleanSelector*) {
        _result = same_selector_type<Selector,
                PqlBooleanSelector>();
    }

    void visit_tuple(PqlTupleSelector*) {
        _result = same_selector_type<Selector,
                PqlTupleSelector>();
    }

    bool return_result() {
        return _result;
    }

  private:
    bool _result;
};

template <typename Selector>
bool is_selector(PqlSelector *selector) {
    IsSelectorVisitor<Selector> visitor;
    selector->accept_pql_selector_visitor(&visitor);
    return visitor.return_result();
}

template <typename Selector>
Selector* selector_cast(PqlSelector *selector) {
    if(is_selector<Selector>(selector)) {
        return static_cast<Selector*>(selector);
    } else {
        return NULL;
    }
}

} //namespace util
} // namespace simple
