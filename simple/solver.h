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

#include <map>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

class QueryValidator {
  public:
    virtual bool validate(SimpleCondition *left_condition, 
            SimpleCondition *right_condition) = 0;

    virtual ~QueryValidator() { }
};

class QuerySolver : public QueryValidator {
  public:
    virtual ConditionSet solve_left(SimpleCondition *right_condition) = 0;
    virtual ConditionSet solve_right(SimpleCondition *left_condition) = 0;

   virtual ~QuerySolver() { }
};

class NextQuerySolver {
  public:
    virtual StatementSet solve_next_statement(StatementAst *statement) = 0;
    virtual StatementSet solve_prev_statement(StatementAst *statement) = 0;

    virtual ~NextQuerySolver() { }
};

class ModifiesQuerySolver {
  public:
    virtual VariableSet solve_modified_vars(StatementAst *statement) = 0;
    virtual StatementSet solve_modifying_statements(const SimpleVariable& variable) = 0;

    virtual ~ModifiesQuerySolver() { }
};

class UsesQuerySolver {
  public:
    virtual VariableSet solve_used_vars(StatementAst *statement) = 0;
    virtual StatementSet solve_using_statements(const SimpleVariable& variable) = 0;

    virtual ~UsesQuerySolver() { }
};

typedef std::map<std::string, std::shared_ptr<QuerySolver> > SolverTable;

} // namespace matcher
