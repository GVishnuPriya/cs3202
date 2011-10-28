
#include "gtest/gtest.h"
#include "simple/ast.h"
#include "impl/ast.h"
#include "impl/solvers/next.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;

TEST(NextTest, BasicTest) {
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleRoot ast(proc);
    NextSolver solver(ast, NULL);
}

} // namespace test
} // namespace simple
