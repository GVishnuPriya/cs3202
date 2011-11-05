
#include <string>
#include "gtest/gtest.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "simple/condition_set.h"
#include "simple/util/condition_utils.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(ConditionTest, EqualityTest) {
    /*
     * Statement Condition
     */
    SimpleAssignmentAst statement1, statement2;

    ConditionPtr statement_condition1(new SimpleStatementCondition(&statement1));

    ConditionPtr statement_condition2(new SimpleStatementCondition(&statement1));

    EXPECT_TRUE((is_same_condition(statement_condition1.get(), statement_condition2.get())));
    EXPECT_EQ(statement_condition1, statement_condition2);
    EXPECT_TRUE(statement_condition1 == statement_condition2);
    EXPECT_FALSE(statement_condition1 != statement_condition2);
    EXPECT_TRUE(statement_condition1 <= statement_condition2);
    EXPECT_TRUE(statement_condition1 >= statement_condition2);
    EXPECT_FALSE(statement_condition1 < statement_condition2);
    EXPECT_FALSE(statement_condition1 > statement_condition2);

    ConditionPtr statement_condition3(new SimpleStatementCondition(&statement2));

    EXPECT_FALSE((is_same_condition(statement_condition1.get(), statement_condition3.get())));
    EXPECT_TRUE(statement_condition1 != statement_condition3);
    EXPECT_FALSE(statement_condition1 == statement_condition3);

    // statement2 is after statement1 in the memory address on stack
    // so because comparison between statement conditions is based on
    // pointer address of the statements, condition1 > condition3
    EXPECT_TRUE(&statement1 > &statement2);
    EXPECT_TRUE(statement_condition1 > statement_condition3);
    EXPECT_FALSE(statement_condition1 < statement_condition3);


    /*
     * Variable Condition
     */
    ConditionPtr var_condition1(new SimpleVariableCondition(SimpleVariable("x")));

    ConditionPtr var_condition2(new SimpleVariableCondition(SimpleVariable("x")));

    EXPECT_TRUE(is_same_condition(var_condition1.get(), var_condition2.get()));
    EXPECT_TRUE(var_condition1 == var_condition2);
    EXPECT_FALSE(var_condition1 < var_condition2);

    ConditionPtr var_condition3(new SimpleVariableCondition(SimpleVariable("y")));
    EXPECT_FALSE(is_same_condition(var_condition1.get(), var_condition3.get()));

    EXPECT_FALSE(is_same_condition(var_condition1.get(), statement_condition1.get()));
    EXPECT_FALSE(is_same_condition(statement_condition2.get(), var_condition3.get()));

    EXPECT_TRUE(var_condition2 != var_condition3);
    EXPECT_TRUE('x' < 'y');
    EXPECT_TRUE(var_condition2 < var_condition3);
    EXPECT_TRUE(var_condition3 > var_condition1);

    /*
     * Proc Condition
     */
    SimpleProcAst proc1("test"), proc2("test");

    ConditionPtr proc_condition1(new SimpleProcCondition(&proc1));

    ConditionPtr proc_condition2(new SimpleProcCondition(&proc1));

    EXPECT_TRUE(is_same_condition(proc_condition1.get(), proc_condition2.get()));
    EXPECT_TRUE(proc_condition1 == proc_condition2);
    EXPECT_FALSE(proc_condition1 < proc_condition2);

    ConditionPtr proc_condition3(new SimpleProcCondition(&proc2));
    
    EXPECT_FALSE(is_same_condition(proc_condition1.get(), proc_condition3.get()));

    EXPECT_FALSE(is_same_condition(proc_condition1.get(), statement_condition1.get()));
    EXPECT_FALSE(is_same_condition(var_condition3.get(), proc_condition2.get()));

    EXPECT_TRUE(proc_condition1 != proc_condition3);
    EXPECT_TRUE(&proc1 > &proc2);
    EXPECT_TRUE(proc_condition3 < proc_condition1);
    EXPECT_FALSE(proc_condition1 < proc_condition3);

    /*
     * Test on different condition types
     */
    EXPECT_TRUE(statement_condition1 < proc_condition1);
    EXPECT_TRUE(var_condition1 < statement_condition1);
    EXPECT_TRUE(var_condition1 < proc_condition1);
    EXPECT_FALSE(proc_condition2 < statement_condition2);
    EXPECT_FALSE(statement_condition2 < var_condition2);
    EXPECT_FALSE(proc_condition2 < var_condition2);
}

TEST(ConditionTest, ConditionSetTest) {
    ConditionSet set1, set2, set3;
    
    SimpleAssignmentAst statement1, statement2, statement3;

    ConditionPtr statement_condition1 = new SimpleStatementCondition(&statement1);
    ConditionPtr statement_condition2 = new SimpleStatementCondition(&statement1);
    ConditionPtr statement_condition3 = new SimpleStatementCondition(&statement2);
    ConditionPtr statement_condition4 = new SimpleStatementCondition(&statement3);

    set1.insert(statement_condition1);
    EXPECT_TRUE(set1.has_element(statement_condition1));
    EXPECT_TRUE(set1.has_element(statement_condition2));
    EXPECT_FALSE(set1.has_element(statement_condition3));
    
    EXPECT_EQ(set1.get_size(), (size_t) 1);
    set1.insert(statement_condition2);
    EXPECT_EQ(set1.get_size(), (size_t) 1);
    set1.insert(statement_condition3);
    EXPECT_EQ(set1.get_size(), (size_t) 2);

    set2.insert(statement_condition1);
    EXPECT_FALSE(set2.has_element(statement_condition3));
    EXPECT_NE(set2, set1);
    
    set2.union_with(set1); // do union
    
    EXPECT_TRUE(set2.has_element(statement_condition3));
    EXPECT_EQ(set2, set1);
    EXPECT_EQ(set2.get_size(), (size_t) 2);

    set3.insert(statement_condition3);
    set3.insert(statement_condition4);
    EXPECT_TRUE(set3.has_element(statement_condition4));
    EXPECT_FALSE(set3.has_element(statement_condition1));
    EXPECT_NE(set3, set1);

    set3.intersect_with(set1); // do intersection
    
    EXPECT_FALSE(set3.has_element(statement_condition4));
    EXPECT_FALSE(set3.has_element(statement_condition1));
    EXPECT_NE(set3, set1);
    EXPECT_EQ(set3.get_size(), (size_t) 1);
}

}
}
