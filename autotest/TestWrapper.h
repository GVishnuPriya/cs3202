#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <vector>
#include <string>
#include <iostream>
#include <list>

#include "AbstractWrapper.h"

class SimpleProgramAnalyzer {
  public:
    virtual void parse(const std::string& filename) = 0;
    virtual std::vector<std::string> evaluate(const std::string& query) = 0;
};

SimpleProgramAnalyzer* create_simple_program_analyzer();

class TestWrapper : public AbstractWrapper {
 public:
  TestWrapper();

  virtual void parse(std::string filename);
  
  virtual void evaluate(std::string query, std::list<std::string>& results);

 private:
  SimpleProgramAnalyzer *spa;
};

#endif
