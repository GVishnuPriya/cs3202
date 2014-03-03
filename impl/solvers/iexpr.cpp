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

#include "impl/solvers/iexpr.h"
#include "impl/solvers/expr.h"
#include "simple/util/ast_utils.h"
#include "simple/util/condition_utils.h"
#include "simple/util/expr.h"
#include "impl/solvers/uses.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;


bool IExprSolver::validate(AssignmentAst *assign_ast, ExprAst *pattern) {

	return sub_expr(assign_ast->get_expr(),pattern);

}

ConditionSet IExprSolver::solve_left(ExprAst *pattern) {

	ConditionSet result;
	/*
	ConditionSet candidate_statements; 

	for (auto it = candidate_statements.begin(); it!=candidate_statements.end();++it) {

		if (get_condition_type(it*)==StatementCT) {

			StatementAst *stmt_ast = static_cast<SimpleStatementCondition>(*it)->get_statement_ast();

			if (get_statement_type(stmt_ast)==AssignST) {
				if (validate(static_cast<AssignmentAst*>(stmt_ast),pattern)) {
					result.insert(new SimpleStatementCondition(*it));
				}
			}

		}
	}
	*/
	return result;
}


bool IExprSolver::sub_expr(ExprAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr1)) {
	case BinaryOpET:
		return sub_expr_bin_op(expr_cast<BinaryOpAst>(expr1),expr2);
	case VariableET:
		return same_expr_var(expr_cast<VariableAst>(expr1),expr2);
	case ConstantET:
		return same_expr_const(expr_cast<ConstAst>(expr1),expr2);
	}

}

bool IExprSolver::sub_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2) {

	switch (get_expr_type(expr2)) {
	case BinaryOpET:
		if (same_expr_bin_op(expr1,expr2)) return true;
	default:
		 return sub_expr(expr1->get_lhs(),expr2) || sub_expr(expr1->get_rhs(),expr2);
	}

}

}
}
