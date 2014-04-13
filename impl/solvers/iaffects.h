
#pragma once

#include "impl/solvers/affects.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class IAffectsSolver : public AffectsSolver {
  public:
    IAffectsSolver(std::shared_ptr<NextBipQuerySolver> next_solver,
        std::shared_ptr<ModifiesSolver> modifies_solver);

    virtual StackedStatementSet solve_affected_by_var_assignment(
        SimpleVariable var, AssignmentAst *statement, CallStack callstack);

    virtual StackedStatementSet solve_affecting_with_var_assignment(
        SimpleVariable var, AssignmentAst *statement, CallStack callstack);
};

}
}