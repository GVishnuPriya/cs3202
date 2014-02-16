
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class NextStar {
  public:
    NextStar(SolverPtr next_star_solver, StatementTable *statement_table);

    bool is_next_star(StatementLine s1, StatementLine s2);

    StatementResults get_next_star(StatementLine s1);

    StatementResults get_prev_star(StatementLine s2);

  private:
    SolverPtr _next_star_solver;
    StatementTable *_statement_table;
};

}