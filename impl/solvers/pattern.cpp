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

#include "impl/solvers/pattern.h"
#include "simple/util/ast_utils.h"
#include "simple/util/condition_utils.h"
#include "simple/util/expr.h"
#include "impl/solvers/uses.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;


bool PatternSolver::validate_assign(AssignmentAst *assign_ast, SimpleVariable *var, ExprAst *pattern) {

	return (assign_ast->get_variable()==var && same_expr(assign_ast->get_expr(),pattern));

}


bool PatternSolver::validate_while(WhileAst *while_ast, SimpleVariable *var) {

	return while_ast->get_variable()==var;

}


bool PatternSolver::validate_if(IfAst *if_ast, SimpleVariable *var) {

	return if_ast->get_variable()==var;

}

ConditionSet PatternSolver::solve_left_assign(SimpleVariable *var, ExprAst *pattern) {

	ConditionSet result;

	ConditionSet candidate_statements = _uses_solver.solve_left(var); 

	for (auto it = candidate_statements.begin(); it!=candidate_statements.end();++it) {

		if (get_condition_type(it*)==StatementCT) {

			StatementAst *stmt_ast = static_cast<SimpleStatementCondition>(*it)->get_statement_ast();

			if (get_statement_type(stmt_ast)==AssignST) {
				if (validate_assign(statement_cast<AssignmentAst>(stmt_ast),var,pattern)) {
					result.insert(new SimpleStatementCondition(*it));
				}
			}

		}
	}

	return result;
}

ConditionSet PatternSolver::solve_left_while(SimpleVariable *var) {

	ConditionSet result;

	ConditionSet candidate_statements = _uses_solver.solve_left(var); 

	for (auto it = candidate_statements.begin(); it!=candidate_statements.end();++it) {

		if (get_condition_type(it*)==StatementCT) {

			StatementAst *stmt_ast = static_cast<SimpleStatementCondition>(*it)->get_statement_ast();

			if (get_statement_type(stmt_ast)==WhileST) {
				if (validate_while(statement_cast<WhileAst>(stmt_ast),var)) {
					result.insert(new SimpleStatementCondition(*it));
				}
			}

		}
	}

	return result;

}

ConditionSet PatternSolver::solve_left_if(SimpleVariable *var) {

	ConditionSet result;

	ConditionSet candidate_statements = _uses_solver.solve_left(var); 

	for (auto it = candidate_statements.begin(); it!=candidate_statements.end();++it) {

		if (get_condition_type(it*)==StatementCT) {

			StatementAst *stmt_ast = static_cast<SimpleStatementCondition>(*it)->get_statement_ast();

			if (get_statement_type(stmt_ast)==IfST) {
				if (validate_if(statement_cast<IfAst>(stmt_ast),var)) {
					result.insert(new SimpleStatementCondition(*it));
				}
			}

		}
	}

	return result;

}

bool PatternSolver::same_expr(ExprAst* expr1, ExprAst * expr2) {

	switch (get_expr_type(expr1)) {
	case BinaryOpET:
		return same_expr_bin_op(expr_cast<BinaryOpAst>(expr1),expr2);
	case VariableET:
		return same_expr_var(expr_cast<VariableAst>(expr1),expr2);
	case ConstantET:
		return same_expr_const(expr_cast<ConstAst>(expr1),expr2);
	}

}

bool PatternSolver::same_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case BinaryOpET:
		return (expr1->get_op() == expr_cast<BinaryOpAst>(expr2)->get_op()
			&& same_expr(expr1->get_lhs(),expr_cast<BinaryOpAst>(expr2)->get_lhs())
			&& same_expr(expr1->get_rhs(),expr_cast<BinaryOpAst>(expr2)->get_rhs()));
	default:
		return false;
	}

}

bool PatternSolver::same_expr_var(VariableAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case VariableET:
		return (expr1->get_variable() == expr_cast<VariableAst>(expr2)->get_variable());
	default:
		return false;
	}

}

bool PatternSolver::same_expr_const(ConstAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case ConstantET:
		return (expr1->get_value() == expr_cast<ConstAst>(expr2)->get_value());
	default:
		return false;
	}

}

bool PatternSolver::sub_expr(ExprAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr1)) {
	case BinaryOpET:
		return sub_expr_bin_op(expr_cast<BinaryOpAst>(expr1),expr2);
	case VariableET:
		return same_expr_var(expr_cast<VariableAst>(expr1),expr2);
	case ConstantET:
		return same_expr_const(expr_cast<ConstAst>(expr1),expr2);
	}

}

bool PatternSolver::sub_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case BinaryOpET:
		if (same_expr_bin_op(expr1,expr2)) return true;
	default:
		 return sub_expr(expr1->get_lhs(),expr2) || sub_expr(expr1->get_rhs(),expr2);
	}

}

}
}
