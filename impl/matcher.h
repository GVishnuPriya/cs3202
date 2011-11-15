
#pragma once

#include <exception>
#include <memory>
#include <string>
#include "simple/solver.h"
#include "simple/matcher.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::impl;

class SimpleQueryMatcher : public QueryMatcher {
  public:
    SimpleQueryMatcher(QuerySolver *solver);

    std::vector<ConditionPair> solve_both(QueryVariable *left, QueryVariable *right);

    void solve_right(SimpleCondition *left, QueryVariable *right);

    void solve_left(QueryVariable *left, SimpleCondition *right);

    bool validate(SimpleCondition *left, SimpleCondition *right);

    virtual ~SimpleQueryMatcher();

  private:
    std::unique_ptr<QuerySolver> _solver;
};


}
}
