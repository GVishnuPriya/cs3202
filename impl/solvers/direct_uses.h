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
#include "simple/solver.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class DirectUsesSolver : public QuerySolver  {
  public:

	DirectUsesSolver(SimpleRoot ast);

  ConditionSet solve_left(SimpleCondition *right_condition);
  ConditionSet solve_right(SimpleCondition *left_condition);
  bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition);

	/*
	DirectUses checks for control variables of container statements.
	Pattern w(v,_) = DirectUses(w,v) 
	Pattern i(v,_,_) = DirectUses(i,v)
	*/

	bool validate_statement_var(StatementAst *statement, SimpleVariable *var);
	
	StatementSet solve_left_statement(SimpleVariable *var);
	
	VariableSet solve_right_var(StatementAst *statement);
	
	// index for solve-left
	void index_proc(ProcAst *proc);
	void index_statement_list(StatementAst *statement);
	void index_statement(StatementAst *statement);
	void index_while(WhileAst *while_ast);
	void index_if(IfAst *if_ast);
	void index_assign(AssignmentAst *assign);
  
private:
	SimpleRoot _ast;
	std::map<SimpleVariable, StatementSet> _right_statement_index;

};
} // namespace impl
} // namespace simple
