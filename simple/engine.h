
#pragma once

class QueryEngine {
  public:
    virtual void init(std::shared_ptr<SimpleRoot> ast) = 0;
};
