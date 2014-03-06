#include "../gtest/gtest.h"
#include "../../simple/spa.h"

#include <stdio.h>
#include <stdexcept>

using std::cout;
using std::cin;
using std::endl;

// To use a test fixture, derive a class from testing::Test.
class SystemModifiesTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.
 SimpleProgramAnalyzer *spa = create_simple_program_analyzer();
 std::vector<std::string> expected;
 std::vector<std::string> result;

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
 virtual void SetUp() {
 	std::string source_file ("modifies.simple");
 	spa->parse(source_file);
 }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  // virtual void TearDown() {
  // }
};

TEST_F(SystemModifiesTest, SimpleProcedure) {
	std::string query ("procedure p;Select p");

	try {
		result = spa->evaluate(query);
	}
	catch(std::runtime_error& e) {
		cout << "Error evaluating query. " << e.what() << endl;
	}

	expected.push_back("First");
	expected.push_back("Second");
	expected.push_back("b");

	EXPECT_EQ(expected, result);
}

TEST_F(SystemModifiesTest, SimpleAssignStatementNum) {
	std::string query ("assign a;\nSelect a.stmt#");

	try {
		result = spa->evaluate(query);
	}
	catch(std::runtime_error& e) {
		cout << "Error evaluating query. " << e.what() << endl;
	}

	EXPECT_EQ(expected, result);
}