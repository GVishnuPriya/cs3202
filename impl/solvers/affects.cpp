
#include "simple/util/expr.h"
#include "simple/util/set_convert.h"
#include "impl/solvers/affects.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class SolveAffectedByVarVisitorTraits {
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

class SolveAffectingWithVarVisitorTraits {
  public:
    typedef StatementSet ResultType;
    typedef SimpleVariable ContextType;

    template <typename Ast>
    static StatementSet visit(
            AffectsSolver *solver, Ast *ast, SimpleVariable *var) 
    {
        return solver->template solve_affecting_with_var<Ast>(*var, ast);
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

class SolveAffectingStatementsVisitorTraits {
  public:
    typedef StatementSet ResultType;
    typedef int ContextType;

    template <typename Ast>
    static StatementSet visit(
            AffectsSolver *solver, Ast *ast, int *ctx) 
    {
        return solver->template solve_affecting_statements<Ast>(ast);
    }
};

AffectsSolver::AffectsSolver(std::shared_ptr<NextQuerySolver> next_solver,
    std::shared_ptr<ModifiesSolver> modifies_solver) :
    _next_solver(next_solver), _modifies_solver(modifies_solver)
{ }

StatementSet AffectsSolver::solve_affected_by_var_assignment(
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

StatementSet AffectsSolver::solve_affecting_with_var_assignment(
    SimpleVariable var, AssignmentAst *statement)
{
    SimpleVariable modified_var = *statement->get_variable();

    if(modified_var == var) {
        StatementSet result;
        result.insert(statement);
        return result;
    } else {
        return solve_prev_affecting_with_var(var, statement);
    }
}


template <>
StatementSet AffectsSolver::solve_affected_statements<StatementAst>(StatementAst *statement) {
    if(_affected_statements_cache.count(statement) > 0) return _affected_statements_cache[statement];

    _affected_by_var_cache.clear();

    StatementVisitorGenerator<AffectsSolver, SolveAffectedStatementsVisitorTraits>
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    StatementSet result = visitor.return_result();

    _affected_statements_cache[statement] = result;
    return result;
}

/*
 * From an generic assignment statementAST, and to dispatch the assignmentAST to other specific method,
 * 1. whileAST -> Dispatch again,
 * 2. assignmentAST -> Get the uses varset in the assignment statement
 *							if used, then put it inside the varset
 *					  -> Get the modifies varset also
							if the affecting var is modified, we simply return the var set.
							if the affecting var is not modified, call solve_next_affected
 * 3. Call -> check if the variable contained in the modified table in that procedure
				-> Yes -> return empty set
				-> No -> move on to next statement
 */
template <>
StatementSet AffectsSolver::solve_affected_statements<AssignmentAst>(AssignmentAst *statement) {
    SimpleVariable modified_var = *statement->get_variable();
    return solve_next_affected_by_var(modified_var, statement);
}

/*
 * Solve the set of statement that is next after that statementAST
 */
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

    StatementVisitorGenerator<AffectsSolver, SolveAffectedByVarVisitorTraits>
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
    return solve_affected_by_var_assignment(var, statement);
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
    if(_modifies_solver->get_vars_modified_by_proc(
        statement->get_proc_called()).count(var) > 0)
    {
        return StatementSet();
    }
    
    return solve_next_affected_by_var(var, statement);
}


template <>
StatementSet AffectsSolver::solve_affecting_statements<StatementAst>(StatementAst *statement) {
    if(_affecting_statements_cache.count(statement) > 0) return _affecting_statements_cache[statement];

    _affecting_with_var_cache.clear();

    StatementVisitorGenerator<AffectsSolver, SolveAffectingStatementsVisitorTraits>
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    StatementSet result = visitor.return_result();

    _affecting_statements_cache[statement] = result;
    return result;
}

template <>
StatementSet AffectsSolver::solve_affecting_statements<AssignmentAst>(AssignmentAst *statement) {
    VariableSet used_vars = get_expr_vars(statement->get_expr());
    
    StatementSet result;
    for(auto it=used_vars.begin(); it!= used_vars.end(); ++it) {
        union_set(result, solve_prev_affecting_with_var(*it, statement));
    }

    return result;
}

StatementSet AffectsSolver::solve_prev_affecting_with_var(SimpleVariable var, StatementAst *statement) {
    StatementSet prev = _next_solver->solve_prev_statement(statement);
    
    StatementSet result;
    for(auto it=prev.begin(); it != prev.end(); ++it) {
        union_set(result, solve_affecting_with_var<StatementAst>(var, *it));
    }

    return result;
}

template <>
StatementSet AffectsSolver::solve_affecting_with_var<StatementAst>(
    SimpleVariable var, StatementAst *statement) 
{
    std::pair<SimpleVariable, StatementAst*> key(var, statement);
    if(_affecting_with_var_cache.count(key) > 0) return _affecting_with_var_cache[key];

    _affecting_with_var_cache[key] = StatementSet();

    StatementVisitorGenerator<AffectsSolver, SolveAffectingWithVarVisitorTraits>
        visitor(this, &var);

    statement->accept_statement_visitor(&visitor);
    StatementSet result = visitor.return_result();

    _affecting_with_var_cache[key] = result;
    return result;
}

template <>
StatementSet AffectsSolver::solve_affecting_with_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement) 
{
    return solve_affecting_with_var_assignment(var, statement);
}

template <>
StatementSet AffectsSolver::solve_affecting_with_var<IfAst>(
    SimpleVariable var, IfAst *statement)
{
    return solve_prev_affecting_with_var(var, statement);
}

template <>
StatementSet AffectsSolver::solve_affecting_with_var<WhileAst>(
    SimpleVariable var, WhileAst *statement)
{
    return solve_prev_affecting_with_var(var, statement);
}

template <>
StatementSet AffectsSolver::solve_affecting_with_var<CallAst>(
    SimpleVariable var, CallAst *statement)
{
    if(_modifies_solver->get_vars_modified_by_proc(
        statement->get_proc_called()).count(var) > 0)
    {
        return StatementSet();
    }

    return solve_prev_affecting_with_var(var, statement);
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
    return statement_set_to_condition_set(solve_affecting_statements<StatementAst>(affecting));
}

template <>
bool AffectsSolver::validate<StatementAst, StatementAst>(
    StatementAst *affecting, StatementAst *affected) 
{
    return validate_affect(affecting, affected);
}

}
}