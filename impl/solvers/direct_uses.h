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

class DirectUsesSolver {
  public:

	DirectUsesSolver(SimpleRoot ast);

	/*
	DirectUses checks for control variables of container statements.
	Pattern w(v,_) = DirectUses(w,v) 
	Pattern i(v,_,_) = DirectUses(i,v)
	*/

	bool validate(StatementAst *statement, SimpleVariable *var);
	
	ConditionSet solve_left(SimpleVariable *var);
	
	ConditionSet solve_right(StatementAst *statement);
	
	// index for solve-left
	void index_proc(ProcAst *proc);
	void index_statement_list(StatementAst *statement);
	void index_statement(StatementAst *statement);
	void index_while(WhileAst *while_ast);
	void index_if(IfAst *if_ast);
  
private:
	SimpleRoot _ast;
	std::map<SimpleVariable, ConditionSet> _right_condition_index;

};
} // namespace impl
} // namespace simple
