
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

class FollowsStar {
  public:
    FollowsStar(SolverPtr follows_star_solver, StatementTable *statement_table);

    bool is_follows_star(StatementLine s1, StatementLine s2);

    StatementResults get_follows_star(StatementLine s1);

    StatementResults get_preceding_star(StatementLine s2);

  private:
    SolverPtr _follows_star_solver;
    StatementTable *_statement_table;
};

}