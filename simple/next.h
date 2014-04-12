
#pragma once

#include <utility>
#include "simple/solver.h"

namespace simple {

typedef std::stack<CallAst*> CallStack;

typedef std::pair<StatementAst*, CallStack> StackedStatement;

typedef std::set<StackedStatement> StackedStatementSet;

class NextBipQuerySolver {
  public:
    virtual StackedStatementSet solve_next_bip_statement(
        StatementAst *statement, CallStack callstack) = 0;

    virtual StackedStatementSet solve_prev_bip_statement(
        StatementAst *statement, CallStack callstack) = 0;

    virtual ~NextBipQuerySolver() { }
};

class SimpleNextQuerySolver : 
    public NextBipQuerySolver, 
    public NextQuerySolver 
{
  public:
    virtual StackedStatementSet solve_next_bip_statement(
        StatementAst *statement, CallStack callstack);

    virtual StackedStatementSet solve_prev_bip_statement(
        StatementAst *statement, CallStack callstack);

    virtual ~SimpleNextQuerySolver() { }
};

StatementSet to_statement_set(const StackedStatementSet& statement_set);
StackedStatementSet to_stacked_statement_set(const StatementSet& statement_set);

}