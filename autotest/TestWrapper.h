#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <vector>
#include <string>
#include <iostream>
#include <list>

#include "simple/spa.h"
#include "AbstractWrapper.h"

class TestWrapper : public AbstractWrapper {
 public:
  TestWrapper();

  virtual void parse(std::string filename);
  
  virtual void evaluate(std::string query, std::list<std::string>& results);

 private:
  SimpleProgramAnalyzer *spa;
};

#endif
