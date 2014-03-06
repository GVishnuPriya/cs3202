#include "../gtest/gtest.h"
#include "../../simple/spa.h"
// virtual void SetUp() will be called before each test is run.  You
// should define it if you need to initialize the varaibles.
// Otherwise, this can be skipped.
//To change this one to a before class set up

// To use a test fixture, derive a class from testing::Test.
class QueueTest : public testing::Test {
 protected:  
  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
  	SimpleProgramAnalyzer *simple_program_analyzer = create_simple_program_analyzer();
  	std:: string source_file ("modifies.simple");
  	simple_program_analyzer->parse(source_file);

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }

  // A helper function for testing Queue::Map().
/*  void MapTester(const Queue<int> * q) {
    // Creates a new queue, where each element is twice as big as the
    // corresponding one in q.
    const Queue<int> * const new_q = q->Map(Double);

    // Verifies that the new queue has the same size as q.
    ASSERT_EQ(q->Size(), new_q->Size());

    // Verifies the relationship between the elements of the two queues.
    for ( const QueueNode<int> * n1 = q->Head(), * n2 = new_q->Head();
          n1 != NULL; n1 = n1->next(), n2 = n2->next() ) {
    EXPECT_EQ(2 * n1->element(), n2->element());
    }

    delete new_q;
  }*/
};


