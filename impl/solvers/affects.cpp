
#include "simple/util/expr.h"
#include "simple/util/set_convert.h"
#include "impl/solvers/affects.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class SolveAffectedByVarsVisitorTraits {
  public:
    typedef StatementSet ResultType;
    typedef SimpleVariable ContextType;

    template <typename Ast>
    static StatementSet visit(
            AffectsSolver *solver, Ast *ast, SimpleVariable *var) 
    {
        return solver->template solve_affected_by_var<Ast>(*var, ast);
    }
};

class SolveAffectedStatementsVisitorTraits {
  public:
    typedef StatementSet ResultType;
    typedef int ContextType;

    template <typename Ast>
    static StatementSet visit(
            AffectsSolver *solver, Ast *ast, int *ctx) 
    {
        return solver->template solve_affected_statements<Ast>(ast);
    }
};

AffectsSolver::AffectsSolver(std::shared_ptr<NextQuerySolver> next_solver,
    std::shared_ptr<UsesQuerySolver> uses_solver,
    std::shared_ptr<ModifiesQuerySolver> modifies_solver) :
    _next_solver(next_solver),
    _uses_solver(uses_solver),
    _modifies_solver(modifies_solver)
{ }

template <>
StatementSet AffectsSolver::solve_affected_statements<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<AffectsSolver, SolveAffectedStatementsVisitorTraits>
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
StatementSet AffectsSolver::solve_affected_statements<AssignmentAst>(AssignmentAst *statement) {
    SimpleVariable modified_var = *statement->get_variable();
    return solve_next_affected_by_var(modified_var, statement);
}

StatementSet AffectsSolver::solve_next_affected_by_var(SimpleVariable var, StatementAst *statement) {
    StatementSet next = _next_solver->solve_next_statement(statement);
    
    StatementSet result;

    for(auto it=next.begin(); it != next.end(); ++it) {
        union_set(result, solve_affected_by_var<StatementAst>(var, *it));
    }

    return result;
}

template <>
StatementSet AffectsSolver::solve_affected_by_var<StatementAst>(
    SimpleVariable var, StatementAst *statement) 
{
    std::pair<SimpleVariable, StatementAst*> key(var, statement);
    if(_affected_by_var_cache.count(key) > 0) return _affected_by_var_cache[key];

    _affected_by_var_cache[key] = StatementSet();

    StatementVisitorGenerator<AffectsSolver, SolveAffectedByVarsVisitorTraits>
        visitor(this, &var);

    statement->accept_statement_visitor(&visitor);
    StatementSet result = visitor.return_result();

    _affected_by_var_cache[key] = result;
    return result;
}

template <>
StatementSet AffectsSolver::solve_affected_by_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement) 
{
    SimpleVariable modified_var = *statement->get_variable();
    VariableSet used_vars = get_expr_vars(statement->get_expr());

    StatementSet result;
    if(modified_var != var) {
        result = solve_next_affected_by_var(var, statement);
    }

    if(used_vars.count(var) > 0) {
        result.insert(statement);
    }

    return result;
}

template <>
StatementSet AffectsSolver::solve_affected_by_var<IfAst>(
    SimpleVariable var, IfAst *statement)
{
    return solve_next_affected_by_var(var, statement);
}

template <>
StatementSet AffectsSolver::solve_affected_by_var<WhileAst>(
    SimpleVariable var, WhileAst *statement)
{
    return solve_next_affected_by_var(var, statement);
}

template <>
StatementSet AffectsSolver::solve_affected_by_var<CallAst>(
    SimpleVariable var, CallAst *statement)
{
    return solve_next_affected_by_var(var, statement);
}

StatementSet AffectsSolver::solve_affecting_statements(StatementAst *affected) {
    VariableSet used_vars = _uses_solver->solve_used_vars(affected);
    return solve_prev_affecting_with_vars(used_vars, affected);
}

StatementSet AffectsSolver::solve_prev_affecting_with_vars(VariableSet vars, StatementAst *statement) {
    StatementSet prev = _next_solver->solve_prev_statement(statement);

    StatementSet result;
    for(auto it=prev.begin(); it != prev.end(); ++it) {
        union_set(result, solve_affecting_with_vars(vars, *it));
    }

    return result;
}

StatementSet AffectsSolver::solve_affecting_with_vars(VariableSet vars, StatementAst *statement) {
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
    return solve_affected_statements<StatementAst>(affecting).count(affected) > 0;
}

template <>
ConditionSet AffectsSolver::solve_right<StatementAst>(StatementAst *affected) {
    return statement_set_to_condition_set(solve_affected_statements<StatementAst>(affected));
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