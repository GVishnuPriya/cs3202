
#include "gtest/gtest.h"
#include "impl/ast.h"

using namespace simple::ast;
using namespace simple::impl;

TEST(AstTest, BasicTest) {
    ConditionalAst *ast1 = new SimpleConditionalAst();
    StatementAst *ast2 = new SimpleConditionalAst();
    WhileAst *ast3 = new SimpleWhileAst();
    AssignmentAst *ast4 = new SimpleAssignmentAst();
    CallAst *ast5 = new SimpleCallAst();
    VariableAst *ast6 = new SimpleVariableAst();
    ConstAst *ast7 = new SimpleConstAst();
    BinaryOpAst *ast8 = new SimpleBinaryOpAst();
}
