
#include "simple/util/set_convert.h"
#include "impl/solvers/next_bip.h"

namespace simple {
namespace impl {

bool is_first_statement(StatementAst *statement) {
    StatementAst *prev = statement->prev();
    StatementAst *parent = statement->get_parent();

    if(!prev && !parent) return true;
    return false;
}

bool is_last_statement(StatementAst *statement);

bool is_last_container_statement(StatementAst *statement) {
    StatementAst *next = statement->next();
    StatementAst *parent = statement->get_parent();

    if(!next && !parent) return true;
    if(!next && parent) return is_last_statement(parent);

    return false;
}

bool is_last_statement(StatementAst *statement) {
    // If statement can never be the last statement
    IfAst *if_ast = statement_cast<IfAst>(statement);
    if(if_ast) return false;

    return is_last_container_statement(statement);
}

NextBipSolver::NextBipSolver(SimpleRoot ast, 
    std::shared_ptr<NextQuerySolver> next_solver, 
    std::shared_ptr<CallsQuerySolver> calls_solver) :
    _ast(ast), _next_solver(next_solver),
    _calls_solver(calls_solver)
{ }

StatementSet NextBipSolver::solve_next_bip(StatementAst *statement) {
    StatementSet result = _next_solver->solve_next_statement(statement);

    if(is_last_statement(statement)) {
        CallSet call_statements = _calls_solver->solve_calling_statements(
            statement->get_proc());

        for(auto it=call_statements.begin(); it!=call_statements.end(); ++it) {
            CallAst *call_statement = *it;
            union_set(result, solve_next_bip(call_statement));
        }
    }

    return result;
}

StatementSet NextBipSolver::solve_next_statement(StatementAst *statement) {
    CallAst *calls = statement_cast<CallAst>(statement);

    if(calls) {
        StatementSet result;
        result.insert(calls->get_proc_called()->get_statement());
        return result;
    } else {
        return solve_next_bip(statement);
    }
}

StatementSet NextBipSolver::solve_prev_statement(StatementAst *statement) {
    StatementSet result = _next_solver->solve_prev_statement(statement);

    if(is_first_statement(statement)) {
        CallSet call_statements = _calls_solver->solve_calling_statements(
            statement->get_proc());

        for(auto it=call_statements.begin(); it!=call_statements.end(); ++it) {
            CallAst *call_statement = *it;
            result.insert(call_statement);
        }
    }

    return result;
}

template <>
ConditionSet NextBipSolver::solve_right<StatementAst>(StatementAst *statement) {
    StatementSet result = solve_next_statement(statement);
    return statement_set_to_condition_set(result);
}

template <>
ConditionSet NextBipSolver::solve_left<StatementAst>(StatementAst *statement) {
    StatementSet result = solve_prev_statement(statement);
    return statement_set_to_condition_set(result);
}

template <>
bool NextBipSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2)
{
    return solve_next_statement(statement1).count(statement2) > 0;
}

}
}