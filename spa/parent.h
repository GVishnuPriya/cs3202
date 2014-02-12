
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

class Parent {
  public:
    Parent(SolverPtr parent_solver, StatementTable *statement_table);

    bool is_parent(StatementLine s1, StatementLine s2);

    StatementResults get_children(StatementLine s1);

    StatementResults get_parent(StatementLine s2);

  private:
    SolverPtr _parent_solver;
    StatementTable *_statement_table;
};

}