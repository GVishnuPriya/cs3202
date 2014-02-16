
#include "spa/affects_star.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

AffectsStar::AffectsStar(SolverPtr affects_star_solver, StatementTable *statement_table) :
  _affects_star_solver(affects_star_solver), _statement_table(statement_table)
{ }

bool AffectsStar::is_affects_star(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _affects_star_solver->validate(&c1, &c2);
}

StatementResults AffectsStar::get_affected_star(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _affects_star_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults AffectsStar::get_affecter_star(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _affects_star_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}