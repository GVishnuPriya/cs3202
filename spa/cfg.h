
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class CFG {
  public:
    CFG(SolverPtr next_solver, StatementTable *statement_table);

    bool validate_next(StatementLine s1, StatementLine s2);

    StatementResults get_next(StatementLine s1);

    StatementResults get_prev(StatementLine s2);

  private:
    SolverPtr _next_solver;
    StatementTable *_statement_table;
};

}