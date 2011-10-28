
#pragma once

#include <map>
#include <set>
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
    ModifiesSolver(const SimpleRoot& ast, SolverTable *table);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    ConditionSet solve_variable(Condition *condition, SimpleVariable *variable);

    template <typename Condition>
    std::set<SimpleVariable> index_variables(Condition *condition);
  private:
    SimpleRoot _ast;
    std::map<SimpleVariable, ConditionSet> _var_index;

    void index_statement_list(StatementAst *statement, ConditionPtr condition, std::set<SimpleVariable>& result);
};

} // namespace impl
} // namespace simple
