
#pragma once

#include "impl/parser/token.h"

namespace simple {
namespace parser {

class SimpleTokenizer {
  public:
    virtual SimpleToken* next_token() = 0;
    virtual ~SimpleTokenizer() { }
};

}
}
