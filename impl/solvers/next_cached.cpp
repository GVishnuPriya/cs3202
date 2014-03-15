
#include "impl/condition.h"
#include "impl/solvers/next_cached.h"
#include "simple/util/set_convert.h"
#include "simple/util/statement_visitor_generator.h"
#include "simple/types.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

CachedNextSolver::CachedNextSolver(SimpleRoot ast, std::shared_ptr<NextQuerySolver> next_solver) :
_ast(ast), _next_solver(next_solver)
{ 
	for(auto it = _ast.begin(); it != _ast.end(); ++it) {
		index_proc(*it);
	}
}

void CachedNextSolver::index_proc(ProcAst *proc) {
	index_statement_list(proc->get_statement());
}

void CachedNextSolver::index_statement_list(StatementAst *statement) {
	while(statement != NULL) {
		index_statement(statement);
		statement = statement->next();
	}
}

void CachedNextSolver::index_statement(StatementAst *statement) {
	StatementSet next_result = _next_solver->solve_next_statement(statement);
	StatementSet prev_result = _next_solver->solve_prev_statement(statement);
// index result both ways
	_next_statement_index[statement] = next_result;
	_prev_statement_index[statement] = prev_result;

	StatementType type = get_statement_type(statement);

	if (type == IfST) {
		IfAst *ifast = statement_cast<IfAst>(statement);
		index_statement_list(ifast->get_then_branch());
		index_statement_list(ifast->get_else_branch());
	}
	else if (type == WhileST){
		WhileAst *whileast = statement_cast<WhileAst>(statement);
		index_statement_list(whileast->get_body());
	}
}   

bool CachedNextSolver::validate(StatementAst *statement1, StatementAst *statement2){
	StatementSet next_result = _next_statement_index[statement1];

	if (next_result.count(statement2)>0)
	{
		return true;
	}else{
		return false;
	}
}

template <>
ConditionSet CachedNextSolver::solve_right<StatementAst>(StatementAst *ast) {
	StatementSet statements = _next_statement_index[ast];
	return statement_set_to_condition_set(statements);
}

template <>
ConditionSet CachedNextSolver::solve_left<StatementAst>(StatementAst *ast) {
	StatementSet statements = _prev_statement_index[ast];
	return statement_set_to_condition_set(_prev_statement_index[ast]);
}
}
}




