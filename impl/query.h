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
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;

class SimplePqlConditionTerm : public PqlConditionTerm {
  public:
    SimplePqlConditionTerm(ConditionPtr condition) : 
        _condition(condition)
    { }

    ConditionPtr get_condition() {
        return _condition;
    }

    void accept_pql_term_visitor(PqlTermVisitor *visitor) {
        visitor->visit_condition_term(this);
    }

    ~SimplePqlConditionTerm() { }

  private:
    ConditionPtr _condition;
};

class SimplePqlVariableTerm : public PqlVariableTerm {
  public:
    SimplePqlVariableTerm(const std::string& qvar) :
        _qvar(qvar)
    { }

    std::string get_query_variable() {
        return _qvar;
    }

    void accept_pql_term_visitor(PqlTermVisitor *visitor) {
        visitor->visit_variable_term(this);
    }

    ~SimplePqlVariableTerm() { }

  private:
    std::string _qvar;
};

class SimplePqlWildcardTerm : public PqlWildcardTerm {
  public:
    SimplePqlWildcardTerm() { }

    void accept_pql_term_visitor(PqlTermVisitor *visitor) {
        visitor->visit_wildcard_term(this);
    }

    ~SimplePqlWildcardTerm() { }
};

class SimplePqlClause : public PqlClause {
  public:
    SimplePqlClause(std::shared_ptr<QuerySolver> solver,
            PqlTerm *left_term, PqlTerm *right_term) :
        _solver(solver), _left_term(left_term), 
        _right_term(right_term)
    { }

    QuerySolver* get_solver() {
        return _solver.get();
    }

    PqlTerm* get_left_term() {
        return _left_term.get();
    }

    PqlTerm* get_right_term() {
        return _right_term.get();
    }

  private:
    std::shared_ptr<QuerySolver>    _solver;
    std::shared_ptr<PqlTerm>        _left_term;
    std::shared_ptr<PqlTerm>        _right_term;
};

class SimplePqlSingleVarSelector : public PqlSingleVarSelector {
  public:
    SimplePqlSingleVarSelector(const std::string& qvar_name) :
        _qvar_name(qvar_name)
    { }

    std::string get_qvar_name() {
        return _qvar_name;
    }
    
    void set_select_type(int select_type) {
        _select_type = select_type;
    }
    
    int get_select_type() {
        return _select_type;
    }

    void accept_pql_selector_visitor(PqlSelectorVisitor *visitor) {
        visitor->visit_single_var(this);
    }

  private:
    std::string _qvar_name;
    int _select_type;
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
        _tuples(tuples)
    { }

    std::vector<std::string> get_tuples() {
        return _tuples;
    }

    void insert_qvar(const std::string& qvar_name) {
        _tuples.push_back(qvar_name);
    }
    
    void accept_pql_selector_visitor(PqlSelectorVisitor *visitor) {
        visitor->visit_tuple(this);
    }
  private:
    std::vector<std::string> _tuples;
};


}
}
