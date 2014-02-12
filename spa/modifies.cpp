
#include "spa/modifies.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

Modifies::Modifies(SolverPtr modifies_solver, StatementTable *statement_table) :
  _modifies_solver(modifies_solver), _statement_table(statement_table)
{ }

bool Modifies::validate_modifies(StatementLine s, Var v) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s));

    SimpleVariableCondition c2(v);

    return _modifies_solver->validate(&c1, &c2);
}

VarResults Modifies::get_modified_var(StatementLine s) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s));
    
    ConditionSet result = _modifies_solver->solve_right(&c1);
    return condition_to_variable_results(result);
}

StatementResults Modifies::get_modifiying_statements(Var v) {
    SimpleVariableCondition c2(v);
    
    ConditionSet result = _modifies_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}