
#include "gtest/gtest.h"
#include "impl/solvers/modifies.h"
#include "impl/ast.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;

TEST(ModifiesTest, FixtureTest1) {
    /*
     * proc test {
     *   x = 1;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");

    assign->set_variable(var);
    assign->set_expr(new SimpleConstAst(1));
    assign->set_line(1);
    assign->set_proc(proc);

    proc->set_first_statement(assign);

    SimpleRoot root(proc);

    ModifiesSolver solver(root, NULL);

    EXPECT_TRUE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var)));
    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(assign, &var)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc, &var)));

    SimpleVariable var2("y"); // different name

    EXPECT_FALSE((solver.validate<StatementAst, SimpleVariable>(assign, &var2)));
    EXPECT_FALSE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var2)));
    EXPECT_FALSE((solver.validate<ProcAst, SimpleVariable>(proc, &var2)));

    SimpleVariable var3("x"); // same name different pointer

    EXPECT_TRUE((solver.validate<StatementAst, SimpleVariable>(assign, &var3)));
    EXPECT_TRUE((solver.validate<AssignmentAst, SimpleVariable>(assign, &var3)));
    EXPECT_TRUE((solver.validate<ProcAst, SimpleVariable>(proc, &var3)));
}

} // namespace test
} // namespace simple
