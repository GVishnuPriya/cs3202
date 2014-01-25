
#pragma once

class SimpleProgramAnalyzer {
  public:
    virtual void parse(std::string filename) = 0;
    virtual std::string evaluate(std::string query) = 0;
};

SimpleProgramAnalyzer* create_simple_program_analyzer();