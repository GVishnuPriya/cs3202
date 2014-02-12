
#include "simple/util/expr.h"
#include "simple/util/set_convert.h"
#include "impl/solvers/iaffects.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

IAffectsSolver::IAffectsSolver(std::shared_ptr<NextQuerySolver> next_solver,
    std::shared_ptr<ModifiesSolver> modifies_solver) :
    AffectsSolver(next_solver, modifies_solver)
{ }

StatementSet IAffectsSolver::solve_affected_by_var_assignment(
        SimpleVariable var, AssignmentAst *statement)
{
    SimpleVariable modified_var = *statement->get_variable();
    VariableSet used_vars = get_expr_vars(statement->get_expr());

    StatementSet result;
    if(modified_var != var) {
        result = solve_next_affected_by_var(var, statement);
    }

    if(used_vars.count(var) > 0) {
        union_set(result, solve_next_affected_by_var(modified_var, statement));
        result.insert(statement);
    }

    return result;
}

StatementSet IAffectsSolver::solve_affecting_with_var_assignment(
        SimpleVariable var, AssignmentAst *statement)
{
    SimpleVariable modified_var = *statement->get_variable();

    if(modified_var == var) {
        VariableSet used_vars = get_expr_vars(statement->get_expr());
        StatementSet result;

        for(auto it = used_vars.begin(); it != used_vars.end(); ++it) {
            union_set(result, solve_prev_affecting_with_var(*it, statement));
        }

        result.insert(statement);
        return result;
    } else {
        return solve_prev_affecting_with_var(var, statement);
    }
}


}
}