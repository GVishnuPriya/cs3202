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

TEST_F(SystemModifiesTest, Negative) {
	std::string query ("stmt s;\nSelect s such that Modifies(s, \"a\")");
	
	try {
		std::vector<std::string> result = spa->evaluate(query);
	}
	catch(std::runtime_error& e) {
		cout << "Error evaluating query. " << e.what() << endl;
	}
}