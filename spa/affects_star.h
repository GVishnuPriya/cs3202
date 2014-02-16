
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class AffectsStar {
  public:
    AffectsStar(SolverPtr affects_star_solver, StatementTable *statement_table);

    bool is_affects_star(StatementLine s1, StatementLine s2);

    StatementResults get_affected_star(StatementLine s1);

    StatementResults get_affecter_star(StatementLine s2);

  private:
    SolverPtr _affects_star_solver;
    StatementTable *_statement_table;
};

}