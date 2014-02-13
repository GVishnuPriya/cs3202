
#include "spa/modifies.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

Modifies::Modifies(SolverPtr modifies_solver, AST *ast) :
  _modifies_solver(modifies_solver), _ast(ast)
{ }

bool Modifies::validate_modifies(StatementLine s, Var v) {
    SimpleStatementCondition c1(_ast->get_statement(s));

    SimpleVariableCondition c2(v);

    return _modifies_solver->validate(&c1, &c2);
}

bool Modifies::validate_modifies(Proc p, Var v) {
    SimpleProcCondition c1(_ast->get_proc(p));

    SimpleVariableCondition c2(v);

    return _modifies_solver->validate(&c1, &c2);
}

VarResults Modifies::get_modified_var(StatementLine s) {
    SimpleStatementCondition c1(_ast->get_statement(s));
    
    ConditionSet result = _modifies_solver->solve_right(&c1);
    return condition_to_variable_results(result);
}

VarResults Modifies::get_modified_var(Proc p) {
    SimpleProcCondition c1(_ast->get_proc(p));
    
    ConditionSet result = _modifies_solver->solve_right(&c1);
    return condition_to_variable_results(result);
}

StatementResults Modifies::get_modifiying_statements(Var v) {
    SimpleVariableCondition c2(v);
    
    ConditionSet result = _modifies_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}