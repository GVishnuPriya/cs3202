
#pragma once

#include "impl/solvers/affects.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class IAffectsSolver : public AffectsSolver {
  public:
    IAffectsSolver(std::shared_ptr<NextQuerySolver> next_solver);

    virtual StatementSet solve_affected_by_var_assignment(
        SimpleVariable var, AssignmentAst *statement);

    virtual StatementSet solve_affecting_with_var_assignment(
        SimpleVariable var, AssignmentAst *statement);
};

}
}