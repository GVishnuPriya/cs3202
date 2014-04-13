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
  	ContainsSolver(SimpleRoot ast);

    bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition);

    ConditionSet solve_right(SimpleCondition *condition);

    ConditionSet solve_left(SimpleCondition *condition);

    ConditionSet index_var(SimpleCondition *condition);

  private:
  	SimpleRoot _ast;

    std::map< ConditionPtr, ConditionSet > _left_index;
    std::map< ConditionPtr, ConditionSet > _right_index;

    void index_proc(ProcAst *proc);
    void index_statement_list(StatementAst *statement);
    void index_statement(StatementAst *statement) ;
    void index_while(WhileAst *while_ast);
    void index_if(IfAst *if_ast);
    void index_assign(AssignmentAst *assign_ast);
    void index_operator(BinaryOpAst *op_ast);
    ConditionSet index_expr(ExprAst *expr_ast, ConditionPtr op_ptr);
};



} // namespace impl
} // namespace simple






