
#include "spa/parent_star.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

ParentStar::ParentStar(SolverPtr parent_star_solver, StatementTable *statement_table) :
  _parent_star_solver(parent_star_solver), _statement_table(statement_table)
{ }

bool ParentStar::is_parent_star(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _parent_star_solver->validate(&c1, &c2);
}

StatementResults ParentStar::get_children_star(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _parent_star_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults ParentStar::get_parent_star(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _parent_star_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}