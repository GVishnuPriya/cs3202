
#include "simple/util/expr_util.h"
#include "simple/util/set_convert.h"
#include "impl/solvers/iaffects.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

IAffectsSolver::IAffectsSolver(std::shared_ptr<NextBipQuerySolver> next_solver,
    std::shared_ptr<ModifiesSolver> modifies_solver) :
    AffectsSolver(next_solver, modifies_solver)
{ }

StackedStatementSet IAffectsSolver::solve_affected_by_var_assignment(
        SimpleVariable var, AssignmentAst *statement, CallStack callstack)
{
    SimpleVariable modified_var = *statement->get_variable();
    VariableSet used_vars = get_expr_vars(statement->get_expr());

    StackedStatementSet result;

    if(modified_var != var) {
        result = solve_next_affected_by_var(var, statement, callstack);
    }

    if(used_vars.count(var) > 0) {
        union_set(result, solve_next_affected_by_var(modified_var, statement, callstack));
        result.insert(StackedStatement(statement, callstack));
    }

    return result;
}

StackedStatementSet IAffectsSolver::solve_affecting_with_var_assignment(
        SimpleVariable var, AssignmentAst *statement, CallStack callstack)
{
    SimpleVariable modified_var = *statement->get_variable();

    if(modified_var != var) return solve_prev_affecting_with_var(var, statement, callstack);

    VariableSet used_vars = get_expr_vars(statement->get_expr());
    StackedStatementSet result;

    for(auto it = used_vars.begin(); it != used_vars.end(); ++it) {
        union_set(result, solve_prev_affecting_with_var(*it, statement, callstack));
    }

    result.insert(StackedStatement(statement, callstack));
    return result;
}


}
}