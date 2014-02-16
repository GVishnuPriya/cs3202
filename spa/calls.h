
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/ast.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class Calls {
  public:
    Calls(SolverPtr calls_solver, AST *ast);

    bool validate_calls(Proc p1, Proc p2);

    ProcResults get_called(Proc p1);

    ProcResults get_caller(Proc p2);

  private:
    SolverPtr _calls_solver;
    AST *_ast;
};

}