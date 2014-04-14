
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

ContainsSolver::ContainsSolver(SimpleRoot ast, bool indirect) : 
    _ast(ast), _indirect(indirect) 
{
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

void ContainsSolver::index_contains(ConditionPtr parent, ConditionPtr contained) {
    _left_index[parent].insert(contained);
    _right_index[contained].insert(parent);
}

ConditionSet ContainsSolver::index_contains_set(
    ConditionPtr parent, ConditionSet contained) 
{
    for(auto it=contained.begin(); it!=contained.end(); ++it) {
        index_contains(parent, *it);
    }

    if(_indirect) {
        contained.insert(parent);
        return contained;
    } else {
        ConditionSet result;
        result.insert(parent);
        return result;
    }
}

void ContainsSolver::index_proc(ProcAst *proc) {
    ConditionPtr parent(new SimpleProcCondition(proc));

    StatementAst *statement = proc->get_statement();
    ConditionSet contained = index_statement_list(statement);

    index_contains_set(parent, contained);
}

ConditionSet ContainsSolver::index_statement_list(StatementAst *statement) {
    ConditionSet result;

    while(statement != NULL) {
        result.union_with(index_statement(statement));

        statement = statement->next();
    }

    return result;
}

ConditionSet ContainsSolver::index_statement(StatementAst *statement) {
    switch(get_statement_type(statement)) {
        case AssignST:
            return index_assign(statement_cast<AssignmentAst>(statement));
        break;

        case WhileST:
            return index_while(statement_cast<WhileAst>(statement));
        break;

        case IfST:
            return index_if(statement_cast<IfAst>(statement));
        break;

        default:
            return ConditionSet();
        break;
    }
}

ConditionSet ContainsSolver::index_while(WhileAst *while_ast) {
    ConditionPtr parent(new SimpleStatementCondition(while_ast));

    StatementAst *statement = while_ast->get_body();
    ConditionSet contained = index_statement_list(statement);

    return index_contains_set(parent, contained);
}

ConditionSet ContainsSolver::index_if(IfAst *if_ast) {
    ConditionPtr parent(new SimpleStatementCondition(if_ast));
    
    StatementAst *then_branch = if_ast->get_then_branch();
    StatementAst *else_branch = if_ast->get_else_branch();

    ConditionSet left_contained = index_statement_list(then_branch);
    ConditionSet right_contained = index_statement_list(else_branch);

    left_contained.union_with(right_contained);
    return index_contains_set(parent, left_contained);
}

ConditionSet ContainsSolver::index_assign(AssignmentAst *assign_ast) {
    ConditionPtr parent(new SimpleStatementCondition(assign_ast));

    ConditionPtr var(new SimpleVariableCondition (*assign_ast->get_variable()));

    ConditionSet contained = index_expr(assign_ast->get_expr());
    contained.insert(var);

    return index_contains_set(parent, contained);
}

ConditionSet ContainsSolver::index_expr(ExprAst *expr) {
    ExprType expr_type = get_expr_type(expr);

    switch(expr_type) {
        case ConstantET:
            return index_const_expr(expr_cast<ConstAst>(expr));
        break;
        case VariableET:
            return index_var_expr(expr_cast<VariableAst>(expr));
        break;
        case BinaryOpET:
            return index_binary_op_expr(expr_cast<BinaryOpAst>(expr));
        break;
        default:
            return ConditionSet();
        break;
    }
}

ConditionSet ContainsSolver::index_binary_op_expr(BinaryOpAst *expr) {
    ConditionPtr parent(new SimpleOperatorCondition(expr->get_op()));

    ConditionSet left_contained = index_expr(expr->get_lhs());
    ConditionSet right_contained = index_expr(expr->get_rhs());

    left_contained.union_with(right_contained);

    return index_contains_set(parent, left_contained);
}

ConditionSet ContainsSolver::index_const_expr(ConstAst *expr) {
    ConditionPtr condition(new SimpleConstantCondition(*expr->get_constant()));

    ConditionSet result;
    result.insert(condition);

    return result;
}

ConditionSet ContainsSolver::index_var_expr(VariableAst *expr) {
    ConditionPtr condition(new SimpleVariableCondition(*expr->get_variable()));

    ConditionSet result;
    result.insert(condition);

    return result;
}

}
}

