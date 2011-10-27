
#include "gtest/gtest.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "simple/condition_utils.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(ConditionTest, EqualityTest) {
    std::unique_ptr<StatementAst> 
    statement1(new SimpleAssignmentAst());
    
    std::unique_ptr<StatementAst> 
    statement2(new SimpleAssignmentAst());

    std::unique_ptr<SimpleCondition> 
    statement_condition1(new SimpleStatementCondition(statement1.get()));

    std::unique_ptr<SimpleCondition> 
    statement_condition2(new SimpleStatementCondition(statement1.get()));

    EXPECT_TRUE((is_same_condition(statement_condition1.get(), statement_condition2.get())));

    std::unique_ptr<SimpleCondition> 
    statement_condition3(new SimpleStatementCondition(statement2.get()));

    EXPECT_FALSE((is_same_condition(statement_condition1.get(), statement_condition3.get())));


    std::unique_ptr<SimpleCondition>
    var_condition1(new SimpleVariableCondition("x"));

    std::unique_ptr<SimpleCondition>
    var_condition2(new SimpleVariableCondition("x"));

    EXPECT_TRUE(is_same_condition(var_condition1.get(), var_condition2.get()));

    std::unique_ptr<SimpleCondition>
    var_condition3(new SimpleVariableCondition("y"));

    EXPECT_FALSE(is_same_condition(var_condition1.get(), var_condition3.get()));

    EXPECT_FALSE(is_same_condition(var_condition1.get(), statement_condition1.get()));
    EXPECT_FALSE(is_same_condition(statement_condition2.get(), var_condition3.get()));


    std::unique_ptr<ProcAst> 
    proc1(new SimpleProcAst("test"));

    std::unique_ptr<ProcAst> 
    proc2(new SimpleProcAst("test"));

    std::unique_ptr<SimpleCondition>
    proc_condition1(new SimpleProcCondition(proc1.get()));

    std::unique_ptr<SimpleCondition>
    proc_condition2(new SimpleProcCondition(proc1.get()));

    EXPECT_TRUE(is_same_condition(proc_condition1.get(), proc_condition2.get()));

    std::unique_ptr<SimpleCondition>
    proc_condition3(new SimpleProcCondition(proc2.get()));
    
    EXPECT_FALSE(is_same_condition(proc_condition1.get(), proc_condition3.get()));

    EXPECT_FALSE(is_same_condition(proc_condition1.get(), statement_condition1.get()));
    EXPECT_FALSE(is_same_condition(var_condition3.get(), proc_condition2.get()));
}

}
}
