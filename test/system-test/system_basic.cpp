#include "../gtest/gtest.h"
#include "../../simple/spa.h"

#include <stdio.h>
#include <stdexcept>

using std::cout;
using std::cin;
using std::endl;

// To use a test fixture, derive a class from testing::Test.
class SystemBasicTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.
 SimpleProgramAnalyzer *spa = create_simple_program_analyzer();
 std::string output;
 std::vector<std::string> result;

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
 virtual void SetUp() {
 	std::string source_file ("modifies.simple");
 	spa->parse(source_file);
 }

 void CompileOutput(std::string query){
	try {
		result = spa->evaluate(query);
	}
	catch(std::runtime_error& e) {
		cout << "Error evaluating query. " << e.what() << endl;
	}

 	auto it = result.begin();
	auto end = result.end();

	while(it != end) {
		output = output + *it;

		if(++it == end) break;
		output = output + ", ";
	}

 }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  // virtual void TearDown() {
  // }
};

TEST_F(SystemBasicTest, Procedure_NoClause) {
	std::string query ("procedure p;\nSelect p");
	CompileOutput(query);
	EXPECT_EQ("First, Second, b", output);
}
/*
TEST_F(SystemBasicTest, ProcedureName_NoClause) {
	std::string query ("procedure p;\nSelect p.procName");
	CompileOutput(query);
	EXPECT_EQ("", output);
}

TEST_F(SystemBasicTest, StatementList_NoClause) {
	std::string query ("stmtLst s;\nSelect s");
	CompileOutput(query);
	EXPECT_EQ("", output);
}

TEST_F(SystemBasicTest, StatementListValue_NoClause) {
	std::string query ("stmtLst s;\nSelect s.value");
	CompileOutput(query);
	EXPECT_EQ("", output);
}

TEST_F(SystemBasicTest, StatementListStatementNum_NoClause) {
	std::string query ("stmtLst s;\nSelect s.stmt#");
	CompileOutput(query);
	EXPECT_EQ("", output);
}*/