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

#include "impl/solvers/directuses.h"
#include "simple/util/ast_utils.h"
#include "simple/util/set_convert.h"
#include "simple/util/condition_utils.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

DirectUsesSolver(SimpleRoot ast) : _ast(ast) { 
	for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
		index_proc(*it);
	}
}

bool DirectUsesSolver::validate(StatementAst *statement, SimpleVariable *var) {
	switch(get_statement_type(statement)) {
		case WhileST:
			return (statement_cast<WhileAst>(statement)->get_variable() == var);
		case IfST:
			return (statement_cast<IfAst>(statement)->get_variable() == var);
		default:
			return false;
	}
}

ConditionSet DirectUsesSolver::solve_left(SimpleVariable *var) {
	return _right_condition_index[*var];
}

ConditionSet DirectUsesSolver::solve_right(StatementAst *statement) {
	ConditionSet result;
	switch(get_statement_type(statement)) {
		case WhileST:
			result.insert(new SimpleVariableCondition(statement_cast<WhileAst>(statement)->get_variable()));
		case IfST:
			result.insert(new SimpleVariableCondition(statement_cast<IfAst>(statement)->get_variable()));
	}
	return result;
}

void DirectUsesSolver::index_proc(ProcAst *proc) {
	index_statement_list(get_statement());
}

void DirectUsesSolver::index_statement_list(StatementAst *statement) {
	while(statement!=NULL) {
		index_statement(statement);
		statement = statement->next();
	}
}

void DirectUsesSolver::index_statement(StatementAst *statement) {
	switch(get_statement_type(statement)) {
		case WhileST:
			index_while(statement_cast<WhileAst>(statement));
			break;
		case IfST:
			index_if(statement_cast<IfAst>(statement));
			break;
	}
}

void DirectUsesSolver::index_while(WhileAst *while_ast) {
	ConditionPtr statement_condition(new SimpleStatementCondition(while_ast));
	_right_condition_index[while_ast->get_variable()].insert(statement_condition);
	index_statement_list(while_ast->get_body());
}

void DirectUsesSolver::index_if(IfAst *if_ast) {
	ConditionPtr statement_condition(new SimpleStatementCondition(if_ast));
	_right_condition_index[if_ast->get_variable()].insert(statement_condition);
	index_statement_list(if_ast->get_then_branch());
	index_statement_list(if_ast->get_else_branch());
}


}
}
