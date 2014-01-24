
#include "simple/spa.h"
#include "impl/frontend.h"

class SimpleProgramAnalyzerImpl : public SimpleProgramAnalyzer {
  public:

  private:
    SimplePqlFrontEnd *frontend;
};

SimpleProgramAnalyzer* create_simple_program_analyzer() {

}
