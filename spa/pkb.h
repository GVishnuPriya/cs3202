
#pragma once

#include <memory>
#include "simple/ast.h"
#include "simple/solver.h"
#include "spa/statement.h"
#include "spa/follows.h"
#include "spa/uses.h"
#include "spa/cfg.h"

namespace spa {

using namespace simple;

class PKB {
  public:
    PKB(SimpleRoot ast, LineTable line_table, SolverTable solver_table);

    Follows* get_follows();

    Uses* get_uses();

    CFG* get_cfg();

  private:
    SimpleRoot _ast;
    LineTable _line_table;
    SolverTable _solver_table;
    StatementTable _statement_table;
    Follows _follows;
    Uses _uses;
    CFG _cfg;
};

}