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

#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/predicate.h"
#include "simple/solver.h"
#include <memory>
#include <string>
#include <vector>

namespace simple {

class PqlTermVisitor;

class PqlTerm {
  public:
    virtual void accept_pql_term_visitor(PqlTermVisitor *visitor) = 0;

    virtual ~PqlTerm() { }
};

class PqlConditionTerm : public PqlTerm {
  public:
    virtual ConditionPtr get_condition() = 0;

    virtual ~PqlConditionTerm() { }
};

class PqlVariableTerm : public PqlTerm {
  public:
    virtual std::string get_query_variable() = 0;

    virtual ~PqlVariableTerm() { }
};

class PqlWildcardTerm : public PqlTerm {
  public:
    virtual ~PqlWildcardTerm() { }
};

class PqlTermVisitor {
  public:
    virtual void visit_condition_term(PqlConditionTerm *term) = 0;
    virtual void visit_variable_term(PqlVariableTerm *term) = 0;
    virtual void visit_wildcard_term(PqlWildcardTerm *term) = 0;

    virtual ~PqlTermVisitor() { }
};

class PqlClause {
  public:
    virtual QuerySolver* get_solver() = 0;

    virtual PqlTerm* get_left_term() = 0;
    virtual PqlTerm* get_right_term() = 0;

    virtual ~PqlClause() { }
};

class PqlSelectorVisitor;

class PqlSelector {
  public:
    virtual void accept_pql_selector_visitor(PqlSelectorVisitor *visitor) = 0;

    virtual ~PqlSelector() { }
};

class PqlSingleVarSelector : public PqlSelector {
  public:
    virtual std::string get_qvar_name() = 0; 
    virtual void set_select_type(int select_type)= 0;
    virtual int get_select_type() = 0;
    virtual ~PqlSingleVarSelector() { }
};

class PqlBooleanSelector : public PqlSelector {
  public:
    virtual ~PqlBooleanSelector() { }
};

class PqlTupleSelector : public PqlSelector {
  public:
    virtual std::vector<std::string> get_tuples() = 0;

    virtual ~PqlTupleSelector() { }
};

class PqlSelectorVisitor {
  public:
    virtual void visit_single_var(PqlSingleVarSelector *selector) = 0;
    virtual void visit_boolean(PqlBooleanSelector *selector) = 0;
    virtual void visit_tuple(PqlTupleSelector *selector) = 0;

    virtual ~PqlSelectorVisitor() { }
};

class ClausePtr : public std::shared_ptr<PqlClause> {
  public:
    typedef std::shared_ptr<PqlClause> Parent;

    ClausePtr(PqlClause *clause) : Parent(clause) { }

    bool operator ==(const ClausePtr& other) const;

    bool operator <(const ClausePtr& other) const;
};

typedef std::set<ClausePtr> ClauseSet;

struct PqlQuerySet {
  public:
    std::map< std::string, 
        std::shared_ptr<SimplePredicate> >      predicates;

    std::shared_ptr<PqlSelector>                selector;
    std::set<ClausePtr>                         clauses;
};

} // namespace simple
