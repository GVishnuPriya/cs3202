#pragma once

#include <map>
#include <set>
#include <memory>
#include "simple/util/expr_util.h"
#include "simple/ast.h"
#include "simple/solver.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class ContainsSolver : public QuerySolver {
  public:
  	ContainsSolver(SimpleRoot ast, bool indirect);

    bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition);

    ConditionSet solve_right(SimpleCondition *condition);

    ConditionSet solve_left(SimpleCondition *condition);

  private:
  	SimpleRoot _ast;
    bool _indirect;

    std::map< ConditionPtr, ConditionSet > _left_index;
    std::map< ConditionPtr, ConditionSet > _right_index;

    void index_contains(ConditionPtr left, ConditionPtr right);
    ConditionSet index_contains_set(ConditionPtr left, ConditionSet right);

    void index_proc(ProcAst *proc);
    ConditionSet index_statement_list(
      StatementAst *statement);

    ConditionSet index_statement(StatementAst *statement) ;
    ConditionSet index_while(WhileAst *while_ast);
    ConditionSet index_if(IfAst *if_ast);
    ConditionSet index_assign(AssignmentAst *assign_ast);

    ConditionSet index_expr(ExprAst *expr);

    ConditionSet index_binary_op_expr(BinaryOpAst *expr);
    ConditionSet index_const_expr(ConstAst *expr);
    ConditionSet index_var_expr(VariableAst *expr);
};

} // namespace impl
} // namespace simple






