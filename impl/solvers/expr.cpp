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

#include "impl/solvers/expr.h"
#include "simple/util/ast_utils.h"
#include "simple/util/condition_utils.h"
#include "simple/util/expr.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

ExprSolver::ExprSolver(SimpleRoot ast) : _ast(ast) {
	
	for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
		index_proc(*it);
	}
}


bool ExprSolver::validate(AssignmentAst *assign_ast, ExprAst *pattern) {

	return same_expr(assign_ast->get_expr(),pattern);

}

ConditionSet ExprSolver::solve_left(ExprAst *pattern) {
	ConditionSet result;
    std::string key = expr_to_string(pattern);
    std::set<AssignmentAst*> assign_stmts = _pattern_index[key];
    
    for (auto it = assign_stmts.begin(); it != assign_stmts.end(); ++it) {
      
      result.insert(new SimpleStatementCondition(*it));
      
    }
    
    return result;
}

ConditionSet ExprSolver::solve_right(AssignmentAst *assign_ast) {
    ConditionSet result;
    result.insert(new SimplePatternCondition(assign_ast->get_expr()));
    return result;
}

bool ExprSolver::same_expr(ExprAst* expr1, ExprAst * expr2) {

	switch (get_expr_type(expr1)) {
	case BinaryOpET:
		return same_expr_bin_op(expr_cast<BinaryOpAst>(expr1),expr2);
	case VariableET:
		return same_expr_var(expr_cast<VariableAst>(expr1),expr2);
	case ConstantET:
		return same_expr_const(expr_cast<ConstAst>(expr1),expr2);
	}

}

bool ExprSolver::same_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case BinaryOpET:
		return (expr1->get_op() == expr_cast<BinaryOpAst>(expr2)->get_op()
			&& same_expr(expr1->get_lhs(),expr_cast<BinaryOpAst>(expr2)->get_lhs())
			&& same_expr(expr1->get_rhs(),expr_cast<BinaryOpAst>(expr2)->get_rhs()));
	default:
		return false;
	}

}

bool ExprSolver::same_expr_var(VariableAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case VariableET:
		return (expr1->get_variable() == expr_cast<VariableAst>(expr2)->get_variable());
	default:
		return false;
	}

}

bool ExprSolver::same_expr_const(ConstAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case ConstantET:
		return (expr1->get_constant() == expr_cast<ConstAst>(expr2)->get_constant());
	default:
		return false;
	}

}


void ExprSolver::index_proc(ProcAst *proc) {
	index_statement_list(get_statement());
}
void ExprSolver::index_statement_list(StatementAst *statement) {
	while(statement!=NULL) {
		index_statement(statement);
		statement = statement->next();
	}
}
void ExprSolver::index_statement(StatementAst *statement) {
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
	}
}
void ExprSolver::index_while(WhileAst *while_ast) {
	index_statement_list(while_ast->get_body());
}
void ExprSolver::index_if(IfAst *if_ast) {
	index_statement_list(if_ast->get_then_branch());
	index_statement_list(if_ast->get_else_branch());
}
void ExprSolver::index_assign(AssignmentAst *assign_ast) {
	std::string key = expr_to_string(assign_ast->get_expr());
	_pattern_index[key].insert(assign_ast);
}




}
}
