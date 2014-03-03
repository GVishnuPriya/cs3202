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

#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "impl/condition.h"
#include "impl/solvers/uses.h"

namespace simple {
namespace impl {

using namespace simple;

class PatternSolver {
  public:

    PatternSolver(std::shared_ptr<UsesSolver> uses_solver) : _uses_solver (uses_solver) {}

	/*
	3 kinds of pattern:
	Pattern a(v,p)
	Pattern w(v,_)
	Pattern i(v,_,_)

	Only the first one is non-trivial.
	*/

	bool PatternSolver::validate_assign(AssignmentAst *assign_ast, SimpleVariable *var, ExprAst *pattern);
	bool PatternSolver::validate_while(WhileAst *while_ast, SimpleVariable *var);
	bool PatternSolver::validate_if(IfAst *if_ast, SimpleVariable *var);

	ConditionSet PatternSolver::solve_left_assign(SimpleVariable *var, ExprAst *pattern);	
  ConditionSet PatternSolver::solve_left_while(SimpleVariable *var);
	ConditionSet PatternSolver::solve_left_if(SimpleVariable *var);

	bool PatternSolver::same_expr(ExprAst *expr1, ExprAst *expr2);
	bool PatternSolver::same_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2);
	bool PatternSolver::same_expr_var(VariableAst *expr1, ExprAst *expr2);
	bool PatternSolver::same_expr_const(ConstAst *expr1, ExprAst *expr2);

	bool PatternSolver::sub_expr(ExprAst *expr1, ExprAst *expr2);
	bool PatternSolver::sub_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2);

private:
	UsesSolver _uses_solver;

};
} // namespace impl
} // namespace simple
