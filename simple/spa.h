
#pragma once

class SimpleProgramAnalyzer {
  public:
    virtual void parse(std::string filename) = 0;
    virtual void evaluate(std::string query, std::list<std::string>& results) = 0;
};

SimpleProgramAnalyzer* create_simple_program_analyzer();