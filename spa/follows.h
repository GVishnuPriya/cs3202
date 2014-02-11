
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

class Follows {
  public:
    Follows(SolverPtr follows_solver, StatementTable *statement_table);

    bool validate_follows(StatementLine s1, StatementLine s2);

    StatementResults get_follows(StatementLine s1);

    StatementResults get_preceeding(StatementLine s2);

  private:
    SolverPtr _follows_solver;
    StatementTable *_statement_table;
};

}