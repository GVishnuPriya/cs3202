
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class Affects {
  public:
    Affects(SolverPtr affects_solver, StatementTable *statement_table);

    bool is_affects(StatementLine s1, StatementLine s2);

    StatementResults get_affected(StatementLine s1);

    StatementResults get_affecter(StatementLine s2);

  private:
    SolverPtr _affects_solver;
    StatementTable *_statement_table;
};

}