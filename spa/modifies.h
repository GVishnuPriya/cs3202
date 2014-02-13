
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/ast.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class Modifies {
  public:
    Modifies(SolverPtr modifies_solver, AST *ast);

    bool validate_modifies(StatementLine s, Var v);
    bool validate_modifies(Proc p, Var v);

    VarResults get_modified_var(StatementLine s);
    VarResults get_modified_var(Proc p);

    StatementResults get_modifiying_statements(Var v);
    ProcResults get_modifiying_procs(Var v);

  private:
    SolverPtr _modifies_solver;
    AST *_ast;
};

}