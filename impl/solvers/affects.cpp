
#include "simple/util/expr_util.h"
#include "simple/util/set_convert.h"
#include "impl/solvers/affects.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

typedef std::pair<SimpleVariable, CallStack> AffectsCtx;

class SolveAffectedByVarVisitorTraits {
  public:
    typedef StackedStatementSet ResultType;
    typedef AffectsCtx ContextType;

    template <typename Ast>
    static StackedStatementSet visit(
            AffectsSolver *solver, Ast *ast, AffectsCtx *ctx) 
    {
        return solver->template solve_affected_by_var<Ast>(ctx->first, ast, ctx->second);
    }
};

class SolveAffectingWithVarVisitorTraits {
  public:
    typedef StackedStatementSet ResultType;
    typedef AffectsCtx ContextType;

    template <typename Ast>
    static StackedStatementSet visit(
            AffectsSolver *solver, Ast *ast, AffectsCtx *ctx) 
    {
        return solver->template solve_affecting_with_var<Ast>(ctx->first, ast, ctx->second);
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

AffectsSolver::AffectsSolver(
    std::shared_ptr<NextBipQuerySolver> next_solver,
    std::shared_ptr<ModifiesSolver> modifies_solver) :
    _next_solver(next_solver), _modifies_solver(modifies_solver)
{ }

StackedStatementSet AffectsSolver::solve_affected_by_var_assignment(
        SimpleVariable var, AssignmentAst *statement, CallStack callstack)
{
    SimpleVariable modified_var = *statement->get_variable();
    VariableSet used_vars = get_expr_vars(statement->get_expr());

    StackedStatementSet result;

    if(modified_var != var) {
        result = solve_next_affected_by_var(var, statement, callstack);
    }

    if(used_vars.count(var) > 0) {
        result.insert(StackedStatement(statement, CallStack()));
    }

    return result;
}

StackedStatementSet AffectsSolver::solve_affecting_with_var_assignment(
    SimpleVariable var, AssignmentAst *statement, CallStack callstack)
{
    SimpleVariable modified_var = *statement->get_variable();

    if(modified_var != var) return solve_prev_affecting_with_var(
        var, statement, callstack);

    StackedStatementSet result;
    result.insert(StackedStatement(statement, CallStack()));
    return result;
}

template <>
StatementSet AffectsSolver::solve_affected_statements<StatementAst>(StatementAst *statement) {
    if(_affected_statements_cache.count(statement) > 0) return _affected_statements_cache[statement];

    _visit_cache.clear();

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

    return to_statement_set(solve_next_affected_by_var(
        modified_var, statement, CallStack()));
}

/*
 * Solve the set of statement that is next after that statementAST
 */
StackedStatementSet AffectsSolver::solve_next_affected_by_var(
    SimpleVariable var, StatementAst *statement, CallStack callstack) 
{
    StackedStatementSet next = _next_solver->solve_next_bip_statement(statement, callstack);
    
    StackedStatementSet result;

    for(auto it=next.begin(); it != next.end(); ++it) {
        union_set(result, solve_affected_by_var<StatementAst>(var, it->first, it->second));
    }

    return result;
}

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<StatementAst>(
    SimpleVariable var, StatementAst *statement, CallStack callstack) 
{
    std::pair<SimpleVariable, StackedStatement> trace(
        var, StackedStatement(statement, callstack));

    if(_visit_cache.count(trace) > 0) return StackedStatementSet();

    _visit_cache.insert(trace);

    AffectsCtx ctx(var, callstack);
    StatementVisitorGenerator<AffectsSolver, SolveAffectedByVarVisitorTraits>
        visitor(this, &ctx);

    statement->accept_statement_visitor(&visitor);
    StackedStatementSet result = visitor.return_result();

    return result;
}

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement, CallStack callstack) 
{
    return solve_affected_by_var_assignment(var, statement, callstack);
}

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<IfAst>(
    SimpleVariable var, IfAst *statement, CallStack callstack)
{
    return solve_next_affected_by_var(var, statement, callstack);
}

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<WhileAst>(
    SimpleVariable var, WhileAst *statement, CallStack callstack)
{
    return solve_next_affected_by_var(var, statement, callstack);
}

template <>
StackedStatementSet AffectsSolver::solve_affected_by_var<CallAst>(
    SimpleVariable var, CallAst *statement, CallStack callstack)
{
    if(!_next_solver->is_bip() && _modifies_solver->get_vars_modified_by_proc(
        statement->get_proc_called()).count(var) > 0)
    {
        return StackedStatementSet();
    }
    
    return solve_next_affected_by_var(var, statement, callstack);
}

template <>
StatementSet AffectsSolver::solve_affecting_statements<StatementAst>(StatementAst *statement) {
    if(_affecting_statements_cache.count(statement) > 0) return _affecting_statements_cache[statement];

    _visit_cache.clear();

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
        union_set(result, to_statement_set(
            solve_prev_affecting_with_var(*it, statement, CallStack())));
    }

    return result;
}

StackedStatementSet AffectsSolver::solve_prev_affecting_with_var(
    SimpleVariable var, StatementAst *statement, CallStack callstack) 
{
    StackedStatementSet prev = _next_solver->solve_prev_bip_statement(statement, callstack);
    
    StackedStatementSet result;
    for(auto it=prev.begin(); it != prev.end(); ++it) {
        union_set(result, solve_affecting_with_var<StatementAst>(var, it->first, it->second));
    }

    return result;
}

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<StatementAst>(
    SimpleVariable var, StatementAst *statement, CallStack callstack) 
{
    std::pair<SimpleVariable, StackedStatement> trace(
        var, StackedStatement(statement, callstack));

    if(_visit_cache.count(trace) > 0) return StackedStatementSet();

    _visit_cache.insert(trace);

    AffectsCtx ctx(var, callstack);
    StatementVisitorGenerator<AffectsSolver, SolveAffectingWithVarVisitorTraits>
        visitor(this, &ctx);

    statement->accept_statement_visitor(&visitor);
    StackedStatementSet result = visitor.return_result();

    return result;
}

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<AssignmentAst>(
    SimpleVariable var, AssignmentAst *statement, CallStack callstack) 
{
    return solve_affecting_with_var_assignment(var, statement, callstack);
}

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<IfAst>(
    SimpleVariable var, IfAst *statement, CallStack callstack)
{
    return solve_prev_affecting_with_var(var, statement, callstack);
}

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<WhileAst>(
    SimpleVariable var, WhileAst *statement, CallStack callstack)
{
    return solve_prev_affecting_with_var(var, statement, callstack);
}

template <>
StackedStatementSet AffectsSolver::solve_affecting_with_var<CallAst>(
    SimpleVariable var, CallAst *statement, CallStack callstack)
{
    if(!_next_solver->is_bip() && _modifies_solver->get_vars_modified_by_proc(
        statement->get_proc_called()).count(var) > 0)
    {
        return StackedStatementSet();
    }

    return solve_prev_affecting_with_var(var, statement, callstack);
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