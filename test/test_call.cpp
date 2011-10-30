
#include "gtest/gtest.h"
#include "impl/solvers/call.h"

namespace simple {
namespace test {


TEST(CallTest, TrivialTest) {
    /*
     * proc test1 {
     *   call test2;
     *   if j {
     *     call test3
     *   } else {
     *     call test4
     *   }
     * }
     *
     * proc test2 {
     *   while i {
     *     call test3
     *   }
     * }
     *
     * proc test3 {
     *   call test2
     * }
     *
     * proc test4 {
     *   call test1
     * }
     *
    SimpleProcAst *proc1 = new SimpleProcAst("test1");
    SimpleProcAst *proc2 = new SimpleProcAst("test2");
    SimpleProcAst *proc3 = new SimpleProcAst("test3");
    SimpleProcAst *proc4 = new SimpleProcAst("test4");

    SimpleCallAst call1 = new SimpleCallAst(proc2);*/

}

}
}
