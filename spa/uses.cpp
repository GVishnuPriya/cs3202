
#include "spa/uses.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

Uses::Uses(SolverPtr uses_solver, AST *ast) :
  _uses_solver(uses_solver), _ast(ast)
{ }

bool Uses::validate_uses(StatementLine s, Var v) {
    SimpleStatementCondition c1(_ast->get_statement(s));

    SimpleVariableCondition c2(v);

    return _uses_solver->validate(&c1, &c2);
}

bool Uses::validate_uses(Proc p, Var v) {
    SimpleProcCondition c1(_ast->get_proc(p));

    SimpleVariableCondition c2(v);

    return _uses_solver->validate(&c1, &c2);
}

VarResults Uses::get_used_vars(StatementLine s) {
    SimpleStatementCondition c1(_ast->get_statement(s));
    
    ConditionSet result = _uses_solver->solve_right(&c1);
    return condition_to_variable_results(result);
}

VarResults Uses::get_used_vars(Proc p) {
    SimpleProcCondition c1(_ast->get_proc(p));
    
    ConditionSet result = _uses_solver->solve_right(&c1);
    return condition_to_variable_results(result);
}

StatementResults Uses::get_using_statements(Var v) {
    SimpleVariableCondition c2(v);
    
    ConditionSet result = _uses_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}