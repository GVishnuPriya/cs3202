
#include "impl/condition.h"
#include "impl/solvers/modifies.h"
#include "impl/solvers/contains.h"
#include "impl/solvers/uses.h"
#include "simple/util/ast_utils.h"
#include "simple/util/set_convert.h"
#include "simple/util/condition_utils.h"
#include "simple/util/expr_util.h"
#include "simple/util/set_utils.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

ContainsSolver::ContainsSolver(SimpleRoot ast) : _ast(ast) {
    for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
        index_proc(*it);
    }
}

bool ContainsSolver::validate(SimpleCondition *left_condition, SimpleCondition *right_condition){
    ConditionPtr left(clone_condition(right_condition));
    ConditionPtr right(clone_condition(left_condition));

	return _left_index[left].has_element(right);
}

ConditionSet ContainsSolver::solve_left(SimpleCondition *right_condition) {
    ConditionPtr right(clone_condition(right_condition));

	return _right_index[right];
}

ConditionSet ContainsSolver::solve_right(SimpleCondition *left_condition) {
    ConditionPtr left(clone_condition(left_condition));

    return _left_index[left];
}

void ContainsSolver::index_condition(ConditionPtr left, ConditionPtr right) {
    _left_index[left].insert(right);
    _right_index[right].insert(left);
}

void ContainsSolver::index_proc(ProcAst *proc) {
    ConditionPtr left(new SimpleProcCondition(proc));
    StatementAst *statement = proc->get_statement();

    index_statement_list(statement, left);
}

void ContainsSolver::index_statement_list(
    StatementAst *statement, ConditionPtr left)
{
    ConditionSet& right_set = _left_index[left];

    while(statement != NULL) {
        index_statement(statement);

        ConditionPtr right(new SimpleStatementCondition(statement));

        right_set.insert(right);
        _right_index[right].insert(left);

        statement = statement->next();
    }
}

void ContainsSolver::index_statement(StatementAst *statement) {
    switch(get_statement_type(statement)) {
        case AssignST:
            index_assign(statement_cast<AssignmentAst>(statement));
        break;

        case WhileST:
            index_while(statement_cast<WhileAst>(statement));
        break;

        case IfST:
            index_if(statement_cast<IfAst>(statement));
        break;

        default:
            // noop
        break;
    }
}

void ContainsSolver::index_while(WhileAst *while_ast) {
    ConditionPtr left(new SimpleStatementCondition(while_ast));
    StatementAst *statement = while_ast->get_body();

    index_statement_list(statement, left);
}

void ContainsSolver::index_if(IfAst *if_ast) {
    ConditionPtr left(new SimpleStatementCondition(if_ast));
    
    StatementAst *then_branch = if_ast->get_then_branch();
    StatementAst *else_branch = if_ast->get_else_branch();

    index_statement_list(then_branch, left);
    index_statement_list(else_branch, left);
}


void ContainsSolver::index_assign(AssignmentAst *assign_ast) {
    ConditionPtr left(new SimpleStatementCondition(assign_ast));
    ConditionPtr right(new SimpleVariableCondition (*assign_ast->get_variable()));

    index_condition(left, right);
    index_expr(assign_ast->get_expr(), left);
}

void ContainsSolver::index_expr(ExprAst *expr, ConditionPtr parent) {
    ExprType expr_type = get_expr_type(expr);

    switch(expr_type) {
        case ConstantET:
            index_const_expr(expr_cast<ConstAst>(expr), parent);
        break;
        case VariableET:
            index_var_expr(expr_cast<VariableAst>(expr), parent);
        break;
        case BinaryOpET:
            index_binary_op_expr(expr_cast<BinaryOpAst>(expr), parent);
        break;
        default:
            // noop
        break;
    }
}

void ContainsSolver::index_binary_op_expr(
    BinaryOpAst *expr, ConditionPtr parent) 
{
    ConditionPtr condition(new SimpleOperatorCondition(expr->get_op()));
    index_condition(parent, condition);

    index_expr(expr->get_lhs(), condition);
    index_expr(expr->get_rhs(), condition);
}

void ContainsSolver::index_const_expr(
    ConstAst *expr, ConditionPtr parent) 
{
    ConditionPtr right(new SimpleConstantCondition(*expr->get_constant()));
    index_condition(parent, right);
}

void ContainsSolver::index_var_expr(
    VariableAst *expr, ConditionPtr parent) 
{
    ConditionPtr right(new SimpleVariableCondition(*expr->get_variable()));
    index_condition(parent, right);
}

}
}

