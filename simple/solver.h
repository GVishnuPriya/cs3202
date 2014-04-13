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
#include <stack>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

/*
 * QuerySolver is a generic interface for solving a two-clause relation.
 */
class QuerySolver {
  public:
    /*
     * Solving case S(a, b)
     * Validate two conditions on whether they satisfy the
     * underlying relation R(left_condition, right_condition)
     */
    virtual bool validate(SimpleCondition *left_condition, 
            SimpleCondition *right_condition) = 0;

    /*
     * Solving case S(A, b)
     * Provide the right condition and solve the left part conditions that
     * satisfy the relation R(left_result, right_condition)
     */
    virtual ConditionSet solve_left(SimpleCondition *right_condition) = 0;

    /*
     * Solving case S(a, B)
     * Provide the left condition and solve the right part conditions that
     * satisfy the relation R(left_condition, right_result)
     */
    virtual ConditionSet solve_right(SimpleCondition *left_condition) = 0;

   virtual ~QuerySolver() { }
};

/*
 * NextQuerySolver is a specialized solver interface for querying the solver
 * using statement AST instead of generic condition. It is the equivalen of 
 * CFG in our PKB.
 */
class NextQuerySolver {
  public:
    /*
     * Solve the next statements that is reachable from a statement node.
     */
    virtual StatementSet solve_next_statement(StatementAst *statement) = 0;

    /*
     * Solve the previous statements that can reach a statement node.
     */
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

class CallsQuerySolver {
  public:
    virtual ProcSet solve_called_procs(ProcAst *calling_proc) = 0;
    virtual ProcSet solve_calling_procs(ProcAst *called_proc) = 0;
    virtual CallSet solve_calling_statements(ProcAst *called_proc) = 0;

    virtual ~CallsQuerySolver() { }
};

typedef std::shared_ptr<QuerySolver> SolverPtr;
typedef std::map<std::string, SolverPtr > SolverTable;

} // namespace matcher
