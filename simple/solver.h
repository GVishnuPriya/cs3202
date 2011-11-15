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

#include <list>
#include <exception>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

using namespace simple;

class SolverDependencyException : public std::exception { };

class QuerySolver {
  public:
  virtual ConditionSet solve_left(SimpleCondition *right_condition) = 0;
  virtual ConditionSet solve_right(SimpleCondition *left_condition) = 0;
  virtual bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition) = 0;

  virtual ~QuerySolver() { }
};

class SolverTable {
  public:
    /*
     * Get a solver that has been created at the moment. If the solver 
     * of the given name has not been created, NULL is returned.
     */
    virtual QuerySolver* get_solver(const std::string& solver_name) = 0;

    virtual bool has_solver(const std::string& solver_name) = 0;

    virtual ~SolverTable() { }
};

class SolverFactory {
  public:
    /*
     * Create a solver instance based on an AST given and a table of 
     * other solvers created prior to this.
     */
    virtual QuerySolver* createSolver(
            SimpleRoot ast, SolverTable *table) = 0;

    /*
     * Dependencies allows a solver to depend on another solver so that 
     * they can share memoized tables. the solver's dependencies are 
     * guaranteed to be in the solver table during construction, or 
     * runtime exception will be raised.
     */
    virtual const std::list<std::string>& get_dependencies() = 0;

    /*
     * Get the name of the solver, which is the same as the query name,
     * i.e. Follows, Modifies.
     */
    virtual std::string get_name() = 0;

    virtual ~SolverFactory() { }
};

} // namespace matcher
