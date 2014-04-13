
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

bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition){
//not done
	return false;
}

ConditionSet ContainsSolver::solve_left(SimpleCondition *right_condition) {
	return _right_index[right_condition];
}

ConditionSet ContainsSolver::solve_right(SimpleCondition *left_condition) {
	return _left_index[left_condition];
}

void ContainsSolver::index_proc(ProcAst *proc) {
    index_statement_list(proc->get_statement());
    ConditionPtr left(new SimpleProcCondition(proc));

	StatementAst *statement = proc->get_statement();
    ConditionSet right_set;
    while(statement != NULL) {
    	ConditionPtr right(new SimpleStatementCondition(statement));
    	right_set.insert(right);
    	_right_index[right].insert(left);
    	statement = statement->next();
    }

    _left_index[left] = right_set;
}

void ContainsSolver::index_statement_list(StatementAst *statement) {
    while(statement!=NULL) {
        index_statement(statement);
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
    ConditionSet right_set;
    StatementAst *statement = while_ast->get_body();

    while(statement != NULL) {
    	index_statement(statement);
    	ConditionPtr right(new SimpleStatementCondition(statement));
    	right_set.insert(right);
    	_right_index[right].insert(left);

    	statement = statement->next();
    }
    ConditionPtr var(new SimpleVariableCondition(*while_ast->get_variable()));
    right_set.insert(var);
    _right_index[var].insert(left);
    _left_index[left] = right_set;
}

void ContainsSolver::index_if(IfAst *if_ast) {
    ConditionPtr left(new SimpleStatementCondition(if_ast));
    ConditionSet right_set;
    StatementAst *statement = if_ast->get_then_branch();

    // index if branch
    while(statement != NULL) {
    	index_statement(statement);
    	ConditionPtr right(new SimpleStatementCondition(statement));
    	right_set.insert(right);
    	_right_index[right].insert(left);

    	statement = statement->next();
    }

    StatementAst *statement1 = if_ast->get_else_branch();
    // index else branch
    while(statement1 != NULL) {
    	index_statement(statement1);
    	ConditionPtr right(new SimpleStatementCondition(statement1));
    	right_set.insert(right);
    	_right_index[right].insert(left);

    	statement1 = statement1->next();
    }

    ConditionPtr var(new SimpleVariableCondition(*if_ast->get_variable()));
    right_set.insert(var);
    _right_index[var].insert(left);
    _left_index[left] = right_set;
}


void ContainsSolver::index_assign(AssignmentAst *assign_ast) {
    ConditionPtr left(new SimpleStatementCondition(assign_ast));
    ConditionPtr right1(new SimpleVariableCondition (*assign_ast->get_variable()));
    ConditionSet right_set;
    right_set.insert(right1);
    _right_index[right1].insert(left);
    ExprAst *right2 = assign_ast->get_expr();
    ExprType type = get_expr_type(right2);
  
    if(type == ConstantET){
    		ConditionPtr right_constant(new SimpleConstantCondition (*expr_cast<ConstAst>(right2)->get_constant()));
    		right_set.insert(right_constant);
    		_right_index[right_constant].insert(left);
    }else if(type == BinaryOpET){
    		ConditionPtr right_op(new SimpleOperatorCondition (expr_cast<BinaryOpAst>(right2)->get_op()));
    		right_set.insert(right_op);
    		_right_index[right_op].insert(left);
    		index_operator(expr_cast<BinaryOpAst>(right2));
    }else{
    		ConditionPtr right_var(new SimpleVariableCondition (*expr_cast<VariableAst>(right2)->get_variable()));
    		right_set.insert(right_var);
    		_right_index[right_var].insert(left);
    }
    _left_index[left]=right_set;
}

void ContainsSolver::index_operator(BinaryOpAst *op_ast){
	ConditionPtr left(new SimpleOperatorCondition(expr_cast<BinaryOpAst>(op_ast)->get_op()));
    ExprAst *lhs = op_ast->get_lhs();
    ExprAst *rhs = op_ast->get_rhs();
    ConditionSet right_set;
    right_set.union_with(index_expr(lhs, left));
    right_set.union_with( index_expr(rhs, left));

    _left_index[left]=right_set;

}

ConditionSet ContainsSolver::index_expr(ExprAst *expr_ast, ConditionPtr op_ptr){
	ConditionSet right_set;

  ExprType type = get_expr_type(expr_ast);
  
  if(type == ConstantET){
    		ConditionPtr _constant(new SimpleConstantCondition (*expr_cast<ConstAst>(expr_ast)->get_constant()));
    		right_set.insert(_constant);
    		_right_index[_constant].insert(op_ptr);
  }else if(type==BinaryOpET){
    		ConditionPtr _op(new SimpleOperatorCondition (expr_cast<BinaryOpAst>(expr_ast)->get_op()));
    		right_set.insert(_op);
    		_right_index[_op].insert(op_ptr);
    		index_operator(expr_cast<BinaryOpAst>(expr_ast));
  }else{
    		ConditionPtr _var(new SimpleVariableCondition (*expr_cast<VariableAst>(expr_ast)->get_variable()));
    		right_set.insert(_var);
    		_right_index[_var].insert(op_ptr);
  }
    return right_set;
}
}
}

