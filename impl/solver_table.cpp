
#include "impl/solver_table.h"

#include "impl/solvers/affects.h"
#include "impl/solvers/iaffects.h"
#include "impl/solvers/follows.h"
#include "impl/solvers/ifollows.h"
#include "impl/solvers/parent.h"
#include "impl/solvers/iparent.h"
#include "impl/solvers/call.h"
#include "impl/solvers/icall.h"
#include "impl/solvers/modifies.h"
#include "impl/solvers/uses.h"
#include "impl/solvers/next.h"
#include "impl/solvers/inext.h"
#include "impl/solvers/same_name.h"

#include "simple/util/solver_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

SolverTable create_solver_table(SimpleRoot ast) {
    SolverTable solver_table;

    solver_table["follows"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<FollowSolver>(new FollowSolver(ast)));

    solver_table["ifollows"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<IFollowSolver>(new IFollowSolver(ast)));

    solver_table["parent"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<ParentSolver>(new ParentSolver(ast)));

    solver_table["iparent"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<IParentSolver>(new IParentSolver(ast)));

    solver_table["calls"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<CallSolver>(new CallSolver(ast)));

    solver_table["icalls"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<ICallSolver>(new ICallSolver(ast)));

    std::shared_ptr<ModifiesSolver> modifies_solver(new ModifiesSolver(ast));

    solver_table["modifies"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<ModifiesSolver>(modifies_solver));

    solver_table["uses"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<UsesSolver>(new UsesSolver(ast)));

    std::shared_ptr<NextSolver> next_solver(new NextSolver(ast));

    solver_table["next"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<NextSolver>(next_solver));

    solver_table["inext"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<INextSolver>(new INextSolver(ast, next_solver)));

    solver_table["affects"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<AffectsSolver>(new AffectsSolver(next_solver, modifies_solver)));

    solver_table["iaffects"] = std::shared_ptr<QuerySolver>(
        new SimpleSolverGenerator<IAffectsSolver>(new IAffectsSolver(next_solver, modifies_solver)));

    return solver_table;
}

}
}