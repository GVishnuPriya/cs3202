
#include "spa/parent.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

Parent::Parent(SolverPtr parent_solver, StatementTable *statement_table) :
  _parent_solver(parent_solver), _statement_table(statement_table)
{ }

bool Parent::is_parent(StatementLine s1, StatementLine s2) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));

    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));

    return _parent_solver->validate(&c1, &c2);
}

StatementResults Parent::get_children(StatementLine s1) {
    SimpleStatementCondition c1(
        _statement_table->get_statement(s1));
    
    ConditionSet result = _parent_solver->solve_right(&c1);
    return condition_to_statement_results(result);
}

StatementResults Parent::get_parent(StatementLine s2) {
    SimpleStatementCondition c2(
        _statement_table->get_statement(s2));
    
    ConditionSet result = _parent_solver->solve_left(&c2);
    return condition_to_statement_results(result);
}

}