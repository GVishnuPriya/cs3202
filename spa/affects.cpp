
#include "spa/affects.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

Affects::Affects(SolverPtr affects_solver, StatementTable *statement_table) :
  _affects_solver(affects_solver), _statement_table(statement_table)
{ }

bool Affects::is_affects(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _affects_solver->validate(&c1, &c2);
}

StatementResults Affects::get_affected(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _affects_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults Affects::get_affecter(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _affects_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}