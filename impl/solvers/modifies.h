
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/solver.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;

class ModifiesSolver {
  public:
    ModifiesSolver(SimpleRoot ast, SolverTable *table) : _ast(ast) { }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    ConditionSet solve_variable(Condition *condition, SimpleVariable *variable);

  private:
    SimpleRoot _ast;

};

} // namespace impl
} // namespace simple
