
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class Modifies {
  public:
    Modifies(SolverPtr modifies_solver, StatementTable *statement_table);

    bool validate_modifies(StatementLine s, Var v);

    VarResults get_modified_var(StatementLine s);

    StatementResults get_modifiying_statements(Var v);

  private:
    SolverPtr _modifies_solver;
    StatementTable *_statement_table;
};

}