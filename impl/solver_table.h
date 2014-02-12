
#pragma once

#include "simple/ast.h"
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;

SolverTable create_solver_table(SimpleRoot ast);

}
}