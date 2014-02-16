
#include "spa/follows.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

Follows::Follows(SolverPtr follows_solver, StatementTable *statement_table) :
  _follows_solver(follows_solver), _statement_table(statement_table)
{ }

bool Follows::is_follows(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _follows_solver->validate(&c1, &c2);
}

StatementResults Follows::get_follows(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _follows_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults Follows::get_preceding(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _follows_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}