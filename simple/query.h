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

#include "simple/condition.h"
#include "simple/qvar.h"
#include "simple/matcher.h"
#include <memory>
#include <string>
#include <vector>

namespace simple {

class PqlQueryVisitor;

class PqlQuery {
  public:
    virtual QueryMatcher* get_matcher() = 0;
    virtual accept_pql_query_visitor(PqlQueryVisitor *visitor) = 0;

    virtual ~PqlQuery() { }
};

class PqlSolveLeftQuery : public PqlQuery {
  public:
    virtual QueryVariable* get_left_query_variable() = 0;
    virtual SimpleCondition* get_right_criteria() = 0;

    virtual ~PqlSolverLeftQuery() { }
};

class PqlSolveRightQuery : public PqlQuery {
  public:
    virtual SimpleCondition* get_left_criteria() = 0;
    virtual QueryVariable* get_right_query_variable() = 0

    virtual ~PqlSolveRightQuery() { }
};

class PqlSolveBothQuery : public PqlQuery {
  public:
    virtual QueryVariable* get_left_query_variable() = 0;
    virtual QueryVariable* get_right_query_variable() = 0

    virtual ~PqlSolveBothQuery() { }
};

class PqlValidateQuery : public PqlQuery {
  public:
    virtual SimpleCondition* get_left_criteria() = 0;
    virtual SimpleCondition* get_right_criteria() = 0;

    virtual ~PqlValidateQuery() { }
};

class PqlQueryVisitor {
  public:
    virtual void visit_solve_left(PqlSolveLeftQuery *query) = 0;
    virtual void visit_solve_right(PqlSolveRightQuery *query) = 0;
    virtual void visit_solve_both(PqlSolveBothQuery *query) = 0;
    virtual void visit_validate(PqlValidateQuery *query) = 0;

    virtual ~PqlQueryVisitor() { }
};

class PqlSelectorVisitor;

class PqlSelector {
  public:
    virtual accept_pql_selection_visitor() = 0;

    virtual ~PqlSelection() { }
};

class PqlSingleVariableSelector : public PqlSelector {
  public:
    virtual std::string get_qvar_name() = 0;

    virtual ~PqlSingleVariableSelector() { }
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
    virtual void visit_single_variable(PqlSingleVariableSelector *selector) = 0;
    virtual void visit_boolean(PqlBooleanSelector *selector) = 0;
    virtual void visit_tuple(PqlTupleSelector *selector) = 0;

    virtual ~PqlSelectorVisitor() { }
};

struct PqlQuerySet {
  public:
    PqlQuerySet() : boolean_mode(false) { }

    std::vector< std::shared_ptr<PqlQuery> >    queries;
    
    std::map<std::string, QVarPtr>              qvars_table;
    
    std::shared_ptr<PqlSelector>                selector;

    bool boolean_mode;
};

} // namespace simple
