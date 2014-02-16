
#include "spa/next_star.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

NextStar::NextStar(SolverPtr next_star_solver, StatementTable *statement_table) :
  _next_star_solver(next_star_solver), _statement_table(statement_table)
{ }

bool NextStar::is_next_star(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _next_star_solver->validate(&c1, &c2);
}

StatementResults NextStar::get_next_star(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _next_star_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults NextStar::get_prev_star(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _next_star_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}