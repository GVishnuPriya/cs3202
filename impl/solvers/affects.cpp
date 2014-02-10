
#include "simple/util/set_convert.h"
#include "impl/solvers/affects.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

AffectsSolver::AffectsSolver(std::shared_ptr<NextQuerySolver> next_solver,
    std::shared_ptr<UsesQuerySolver> uses_solver,
    std::shared_ptr<ModifiesQuerySolver> modifies_solver) :
    _next_solver(next_solver),
    _uses_solver(uses_solver),
    _modifies_solver(modifies_solver)
{ }

StatementSet AffectsSolver::solve_affected_statements(StatementAst *affecting) {
    VariableSet modified_vars = _modifies_solver->solve_modified_vars(affecting);
    return solve_next_affected_by_vars(modified_vars, affecting);
}

StatementSet AffectsSolver::solve_next_affected_by_vars(const VariableSet& vars, StatementAst *statement) {
    StatementSet next = _next_solver->solve_next_statement(statement);
    
    StatementSet result;
    for(auto it=next.begin(); it != next.end(); ++it) {
        union_set(result, solve_affected_by_vars(vars, *it));
    }

    return result;
}

StatementSet AffectsSolver::solve_affected_by_vars(const VariableSet& vars, StatementAst *statement) {
    VariableSet used_vars = _uses_solver->solve_used_vars(statement);
    VariableSet modified_vars = _modifies_solver->solve_modified_vars(statement);

    bool is_affected;
    VariableSet remaining_vars;
    for(auto it = vars.begin(); it != vars.end(); ++it) {
        SimpleVariable var = *it;
        if(used_vars.count(var) > 0) {
            is_affected = true;
        }

        if(modified_vars.count(var) == 0) {
            remaining_vars.insert(var);
        }
    }

    if(remaining_vars.size() > 0) {
        StatementSet result = solve_next_affected_by_vars(remaining_vars, statement);
        if(is_affected) result.insert(statement);
        return result;

    } else {
        StatementSet result;
        if(is_affected) result.insert(statement);
        return result;
    }
}

StatementSet AffectsSolver::solve_affecting_statements(StatementAst *affected) {
    VariableSet used_vars = _uses_solver->solve_used_vars(affected);
    return solve_prev_affecting_with_vars(used_vars, affected);
}

StatementSet AffectsSolver::solve_prev_affecting_with_vars(const VariableSet& vars, StatementAst *statement) {
    StatementSet prev = _next_solver->solve_prev_statement(statement);

    StatementSet result;
    for(auto it=prev.begin(); it != prev.end(); ++it) {
        union_set(result, solve_affecting_with_vars(vars, *it));
    }

    return result;
}

StatementSet AffectsSolver::solve_affecting_with_vars(const VariableSet& vars, StatementAst *statement) {
    VariableSet modified_vars = _modifies_solver->solve_modified_vars(statement);

    bool is_affecting;
    VariableSet remaining_vars;
    for(auto it = vars.begin(); it != vars.end(); ++it) {
        if(modified_vars.count(*it) > 0) {
            is_affecting = true;
        } else {
            remaining_vars.insert(*it);
        }
    }

    if(remaining_vars.size() > 0) {
        StatementSet result = solve_prev_affecting_with_vars(remaining_vars, statement);
        if(is_affecting) result.insert(statement);
        return result;

    } else {
        StatementSet result;
        if(is_affecting) result.insert(statement);
        return result;
    }
}

bool AffectsSolver::validate_affect(StatementAst *affecting, StatementAst *affected) {
    return solve_affected_statements(affecting).count(affected) > 0;
}

template <>
ConditionSet AffectsSolver::solve_right<StatementAst>(StatementAst *affected) {
    return statement_set_to_condition_set(solve_affected_statements(affected));
}

template <>
ConditionSet AffectsSolver::solve_left<StatementAst>(StatementAst *affecting) {
    return statement_set_to_condition_set(solve_affecting_statements(affecting));
}

template <>
bool AffectsSolver::validate<StatementAst, StatementAst>(
    StatementAst *affecting, StatementAst *affected) 
{
    return validate_affect(affecting, affected);
}

}
}