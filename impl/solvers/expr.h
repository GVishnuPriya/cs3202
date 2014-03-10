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

namespace simple {
namespace impl {

using namespace simple;

class ExprSolver {
  public:

    ExprSolver(SimpleRoot ast);

	/*
	Pattern a(v,p) = Expr(a,p) & Modifies(a,v)

	*/

	bool ExprSolver::validate(AssignmentAst *assign_ast, ExprAst *pattern);

  ConditionSet solve_left(ExprAst *pattern);
  ConditionSet solve_right(AssignmentAst *assign_ast);
  
	bool same_expr(ExprAst *expr1, ExprAst *expr2);
	bool same_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2);
	bool same_expr_var(VariableAst *expr1, ExprAst *expr2);
	bool same_expr_const(ConstAst *expr1, ExprAst *expr2);
  
  
  // helper methods to index for solve-left
	void index_proc(ProcAst *proc);
  void index_statement_list(StatementAst *statement);
  void index_statement(StatementAst *statement) ;
  void index_while(WhileAst *while_ast);
  void index_if(IfAst *if_ast);
  void index_assign(AssignmentAst *assign_ast);

private:
	SimpleRoot _ast;
  
  // map string to set of assign statement for solve-left
  std::map<std::string, std::set<AssignmentAst*> > _pattern_index;
  

};
} // namespace impl
} // namespace simple
