
#include "simple/next.h"

namespace simple {

StackedStatementSet SimpleNextQuerySolver::solve_next_bip_statement(
    StatementAst *statement, CallStack callstack)
{
    StatementSet result = solve_next_statement(statement);
    return to_stacked_statement_set(result);
}

StackedStatementSet SimpleNextQuerySolver::solve_prev_bip_statement(
    StatementAst *statement, CallStack callstack)
{
    StatementSet result = solve_prev_statement(statement);
    return to_stacked_statement_set(result);
}

StatementSet to_statement_set(const StackedStatementSet& statement_set) {
    StatementSet result;

    for(auto it=statement_set.begin(); it != statement_set.end(); ++it) {
        result.insert(it->first);
    }

    return result;
}

StackedStatementSet to_stacked_statement_set(const StatementSet& statement_set) {
    StackedStatementSet result;

    for(auto it=statement_set.begin(); it != statement_set.end(); ++it) {
        result.insert(StackedStatement(*it, CallStack()));
    }

    return result;
}

}