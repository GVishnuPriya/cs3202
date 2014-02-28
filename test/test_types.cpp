
#include "gtest/gtest.h"
#include "simple/types.h"
#include "impl/ast.h"
#include "impl/condition.h"

using namespace simple;
using namespace simple::impl;

TEST(TypesTest, BasicTest) {
  SimpleAssignmentAst assign;
  StatementAst *statement = &assign;

  EXPECT_EQ(get_statement_type(statement), AssignST);
  EXPECT_EQ(statement_cast<AssignmentAst>(statement), &assign);
  EXPECT_EQ(statement_cast<IfAst>(statement), (StatementAst*) NULL);

  SimpleStatementCondition statement_cond(statement);
  SimpleCondition *cond = &statement_cond;

  EXPECT_EQ(get_condition_type(cond), StatementCT);
  EXPECT_EQ(condition_cast<StatementCondition>(cond), &statement_cond);
  EXPECT_EQ(condition_cast<ProcCondition>(cond), (SimpleCondition*) NULL);

  SimpleBinaryOpAst binop;
  ExprAst *expr = &binop;

  EXPECT_EQ(get_expr_type(expr), BinaryOpET);
  EXPECT_EQ(expr_cast<BinaryOpAst>(expr), &binop);
  EXPECT_EQ(expr_cast<VariableAst>(expr), (ExprAst*) NULL);
}