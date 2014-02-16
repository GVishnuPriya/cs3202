
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/ast.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;

class CallsStar {
  public:
    CallsStar(SolverPtr calls_star_solver, AST *ast);

    bool validate_calls_star(Proc p1, Proc p2);

    ProcResults get_called_star(Proc p1);

    ProcResults get_caller_star(Proc p2);

  private:
    SolverPtr _calls_star_solver;
    AST *_ast;
};

}