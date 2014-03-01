
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/ast.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class Uses {
  public:
    Uses(SolverPtr uses_solver, AST *ast);

    bool validate_uses(StatementLine s, Var v);
    bool validate_uses(Proc p, Var v);

    VarResults get_used_vars(StatementLine s);
    VarResults get_used_vars(Proc p);

    StatementResults get_using_statements(Var v);
    //ProcResults get_using_procs(Var v);

  private:
    SolverPtr _uses_solver;
    AST *_ast;
};

}