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
#include "simple/matcher.h"

namespace simple {
namespace impl {

using namespace simple;

class SimplePqlSolveLeftQuery : public PqlSolveLeftQuery {
  public:
    SimplePqlSolveLeftQuery(
        MatcherPtr matcher, QvarPtr left_var, ConditionPtr right_criteria) :
        _matcher(matcher), _left_qvar(left_qvar), _right_criteria(right_criteria)
    { }

    QueryVariable* get_left_query_variable() {
        return _left_qvar.get();
    }

    SimpleCondition* get_right_criteria() {
        return _right_criteria.get();
    }

    QueryMatcher* get_matcher() {
        return _matcher.get();
    }

    void accept_pql_query_visitor(PqlQueryVisitor *visitor) {
        visitor->visit_solve_left(this);
    }

    ~SimplePqlSolveLeftQuery() { }

  private:
    MatcherPtr      _matcher;
    QVarPtr         _left_qvar;
    ConditionPtr    _right_criteria;
};

class SimplePqlSolveRightQuery : public PqlSolveRightQuery {
  public:
    SimplePqlSolveRightQuery(
        MatcherPtr matcher, ConditionPtr left_criteria, QVarPtr right_qvar) :
        _matcher(matcher), _left_criteria(left_criteria), _right_qvar(right_qvar)
    { }

    SimpleCondition* get_left_criteria() {
        return _left_criteria.get();
    }

    QueryVariable* get_right_query_variable() {
        return _right_qvar.get();
    }

    QueryMatcher* get_matcher() {
        return _matcher.get();
    }

    void accept_pql_query_visitor(PqlQueryVisitor *visitor) {
        visitor->visit_solve_right(this);
    }

    ~SimplePqlSolveRightQuery() { }

  private:
    MatcherPtr      _matcher;
    ConditionPtr    _left_criteria;
    QVarPtr         _right_qvar;
};

class SimplePqlSolveBothQuery : public PqlSolveBothQuery {
  public:
    SimplePqlSolveBothQuery(
        MatcherPtr matcher, QVarPtr left_qvar, QVarPtr right_qvar) :
        _matcher(matcher), _left_qvar(left_qvar), _right_qvar(right_qvar)
    { }

    QueryVariable* get_left_query_variable() {
        return _left_qvar.get();
    }

    QueryVariable* get_right_query_variable() {
        return _right_qvar.get();
    }

    QueryMatcher* get_matcher() {
        return _matcher.get();
    }

    void accept_pql_query_visitor(PqlQueryVisitor *visitor) {
        visitor->visit_solve_both(this);
    }

    ~SimplePqlSolveBothQuery() { }

  private:
    MatcherPtr  _matcher;
    QVarPtr     _left_qvar;
    QVarPtr     _right_qvar;
};

class SimplePqlValidateQuery : public PqlValidateQuery {
  public:
    SimplePqlValidateQuery(
        MatcherPtr matcher, ConditionPtr left_criteria, ConditionPtr right_criteria) :
        _matcher(matcher), _left_criteria(left_criteria), _right_criteria(right_criteria)
    { }

    SimpleCondition* get_left_criteria() {
        return _left_criteria.get();
    }

    SimpleCondition* get_right_criteria() {
        return _right_criteria.get();
    }

    QueryMatcher* get_matcher() {
        return _matcher.get();
    }

    void accept_pql_query_visitor(PqlQueryVisitor *visitor) {
        visitor->visit_validate(this);
    }

  private:
    MatcherPtr      _matcher;
    ConditionPtr    _left_criteria;
    ConditionPtr    _right_criteria;
};

class SimplePqlSingleVariableSelector : public PqlSingleVariableSelector {
  public:
    SimplePqlSingleVariableSelector(const std::string& qvar_name) :
        _qvar_name(qvar_name)
    { }

    std::string get_qvar_name() {
        return _qvar_name;
    }

    void accept_pql_selector_visitor(PqlSelectorVisitor *visitor) {
        visitor->visit_single_variable(this);
    }

  private:
    std::string _qvar_name;
};

class SimplePqlBooleanSelector : public PqlBooleanSelector {
  public:
    SimplePqlBooleanSelector() { }

    void accept_pql_selector_visitor(PqlSelectorVisitor *visitor) {
        visitor->visit_boolean(this);
    }
};

class SimplePqlTupleSelector : public PqlTupleSelector {
  public:
    SimplePqlTupleSelector() : _tuples() { }

    SimplePqlTupleSelector(std::vector<std::string> tuples) :
        _tupples(tuples)
    { }

    std::vector<std::string> get_tuples() {
        return _tuples;
    }

    void insert_qvar(const std::string& qvar_name) {
        _tuples.push_back(qvar_name);
    }

  private:
    std::vector<std::string> _tuples;
};


}
}
