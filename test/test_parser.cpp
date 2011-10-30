
#include <list>
#include <memory>
#include "impl/parser/tokenizer.h"
#include "impl/parser/parser.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::parser;

class MockTokenizer : public SimpleTokenizer {
  public:
    MockTokenizer() : 
        _tokens(), _first_token(true)
    { }
    
    void insert(SimpleToken *token) {
        _tokens.push_back(std::unique_ptr<SimpleToken>(token));

        if(_first_token) {
            _first_token = false;
            _token_it = _tokens.begin();
        }
    }

    SimpleToken* next_token() {
        if(_token_it == _tokens.end()) {
            throw ParserError();
        } else {
            return (_token_it++)->get();
        }
    }

    virtual ~MockTokenizer() { }

  private:
    std::list< std::unique_ptr<SimpleToken> > _tokens;

    std::list< std::unique_ptr<SimpleToken> >::iterator
    _token_it;

    bool _first_token;
};



}
}
