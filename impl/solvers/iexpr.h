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

class IExprSolver {
  public:

    IExprSolver(SimpleRoot ast) : _ast (ast) {}

	/*
	Pattern a(v,_p_) = IExpr(a,p) & Uses(a,v)

	*/

	bool IExprSolver::validate(AssignmentAst *assign_ast, ExprAst *pattern);

	ConditionSet solve_left(ExprAst *pattern) {
    ConditionSet result;
    std::string key = expr_to_string(pattern);
    std::set<AssignmentAst*> assign_stmts = _pattern_index[key];
    
    for (auto it = assign_stmts.begin(); it != assign_stmts.end(); ++it) {
      
      result.insert(new SimpleStatementCondition(*it));
      
    }
    
    return result;
  }
  ConditionSet solve_right(AssignmentAst *assign_ast) {
    ConditionSet result;
    result.insert(new SimplePatternCondition(assign_ast->get_expr()));
    return result;
  }
  
	bool IExprSolver::sub_expr(ExprAst *expr1, ExprAst *expr2);
	bool IExprSolver::sub_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2);
  
  void index_assign(AssignmentAst *assign_ast) {
    
    std::set<ExprAst*> sub_exprs = get_sub_exprs(assign_ast->get_expr());
    
    for (auto it = sub_exprs.begin(); it != sub_exprs.end(); ++it) {
      std::string key = expr_to_string(*it);
      _pattern_index[key].insert(assign_ast);
    }
    
  }
  

private:
	SimpleRoot _ast;
    
  // map string to set of assign statement for solve-left
  std::map<std::string, std::set<AssignmentAst*> > _pattern_index;
  

};
} // namespace impl
} // namespace simple
