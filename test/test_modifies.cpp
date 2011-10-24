
#include "gtest/gtest.h"
#include "impl/solvers/modifies.h"
#include "impl/ast.h"

namespace simple {
namespace test {

using namespace simple::ast;
using namespace simple::solver;
using namespace simple::impl::solver;

TEST(ModifiesTest, BasicTest) {
    /*
     * proc test {
     *   x = 1;
     * }
     *
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    ASSERT_TRUE(assign);
    SimpleVariable var("x");

    assign->set_variable(var);
    assign->set_expr(new SimpleConstAst(1));
    assign->set_line(1);
    assign->set_proc(proc);

    proc->set_first_statement(assign);

    SimpleRoot root(proc);
    ModifiesSolver solver(root, NULL);

    ASSERT_TRUE(assign);
    //EXPECT_TRUE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var)));
    */
}

}
}
