
#pragma once

#include <list>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

using namespace simple;

class QuerySolver {
  public:
  virtual ConditionSet solve_left(SimpleCondition *right_condition) = 0;
  virtual ConditionSet solve_right(SimpleCondition *left_condition) = 0;
  virtual bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition) = 0;
};

class SolverTable {
  public:
    /*
     * Get a solver that has been created at the moment. If the solver 
     * of the given name has not been created, NULL is returned.
     */
    virtual QuerySolver* get_solver(std::string solver_name) = 0;
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
    virtual std::list<std::string> get_dependencies() = 0;

    /*
     * Get the name of the solver, which is the same as the query name,
     * i.e. Follows, Modifies.
     */
    virtual std::string get_name() = 0;
};

} // namespace matcher
