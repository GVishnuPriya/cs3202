
#include "gtest/gtest.h"
#include "impl/ast.h"

using namespace simple;
using namespace simple::impl;

TEST(AstTest, BasicTest) {
    /*
     * proc test {
     *   x = 1;
     * }
     */
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
    
    EXPECT_EQ(proc->get_statement(), assign);
    EXPECT_TRUE(assign->get_variable()->equals(var));

    EXPECT_EQ(*root.begin(), proc);
}
