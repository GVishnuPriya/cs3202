
#pragma once

#include <vector>
#include <string>

class SimpleProgramAnalyzer {
  public:
    virtual void parse(const std::string& filename) = 0;
    virtual std::vector<std::string> evaluate(const std::string& query) = 0;
};

SimpleProgramAnalyzer* create_simple_program_analyzer();