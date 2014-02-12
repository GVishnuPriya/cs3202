
#include "spa/cfg.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

CFG::CFG(SolverPtr next_solver, StatementTable *statement_table) :
  _next_solver(next_solver), _statement_table(statement_table)
{ }

bool CFG::is_next(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _next_solver->validate(&c1, &c2);
}

StatementResults CFG::get_next(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _next_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults CFG::get_prev(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _next_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}