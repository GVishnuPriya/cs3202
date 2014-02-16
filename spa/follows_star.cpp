
#include "spa/follows_star.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

FollowsStar::FollowsStar(SolverPtr follows_star_solver, StatementTable *statement_table) :
  _follows_star_solver(follows_star_solver), _statement_table(statement_table)
{ }

bool FollowsStar::is_follows_star(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _follows_star_solver->validate(&c1, &c2);
}

StatementResults FollowsStar::get_follows_star(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _follows_star_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults FollowsStar::get_preceding_star(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _follows_star_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}