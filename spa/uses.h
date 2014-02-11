
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class Uses {
  public:
    Uses(SolverPtr uses_solver, StatementTable *statement_table);

    bool validate_uses(StatementLine s, Var v);

    VarResults get_used_vars(StatementLine s);

    StatementResults get_modifiying_statements(Var v);

  private:
    SolverPtr _uses_solver;
    StatementTable *_statement_table;
};

}