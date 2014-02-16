
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

class ParentStar {
  public:
    ParentStar(SolverPtr parent_star_solver, StatementTable *statement_table);

    bool is_parent_star(StatementLine s1, StatementLine s2);

    StatementResults get_children_star(StatementLine s1);

    StatementResults get_parent_star(StatementLine s2);

  private:
    SolverPtr _parent_star_solver;
    StatementTable *_statement_table;
};

}