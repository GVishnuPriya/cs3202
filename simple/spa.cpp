
#include <memory>
#include <fstream>
#include <iterator>
#include <sstream>
#include "simple/spa.h"
#include "impl/frontend.h"

namespace simple {

class SimpleProgramAnalyzerImpl : public SimpleProgramAnalyzer {
  public:
    SimpleProgramAnalyzerImpl() { }

    void parse(const std::string& filename) {
      std::ifstream source(filename);
      std::istreambuf_iterator<char> source_begin(source);
      std::istreambuf_iterator<char> source_end;

      _frontend.reset(new SimplePqlFrontEnd(source_begin, source_end));
    }

    std::vector<std::string> evaluate(const std::string& query) {
      return _frontend->process_query(query.begin(), query.end());
    }

  private:
    std::unique_ptr<SimplePqlFrontEnd> _frontend;
};

} // namespace

SimpleProgramAnalyzer* create_simple_program_analyzer() {
  return new simple::SimpleProgramAnalyzerImpl();
}
