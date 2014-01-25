
#include <iterator>
#include "TestWrapper.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool TestWrapper::GlobalStop = false;

TestWrapper::TestWrapper() {
  spa = create_simple_program_analyzer();
}

void TestWrapper::parse(std::string filename) {
  spa->parse(filename);
}

void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
  std::vector<std::string> result = spa->evaluate(query);
  std::copy(result.begin(), result.end(), std::back_inserter(results));
}
