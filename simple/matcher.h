
#pragma once

#include <vector>
#include "simple/qvar.h"
#include "simple/predicate.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

struct ConditionPair {
  public:
    ConditionPair(ConditionPtr first_, ConditionPtr second_) :
        first(first_), second(second_)
    { }

    ConditionPtr first;
    ConditionPtr second;
};

class QueryMatcher {
  public:
    virtual std::vector<ConditionPair> solve_both(QueryVariable *left, QueryVariable *right) = 0;
    virtual void solve_right(SimpleCondition *condition, QueryVariable *var) = 0;
    virtual void solve_left(QueryVariable *var, SimpleCondition *condition) = 0;
    virtual bool validate(SimpleCondition *left, SimpleCondition *right) = 0;

    virtual ~QueryMatcher() { }
};

class MatcherTable {
  public:
    virtual QueryMatcher* get_matcher(const std::string& name) const = 0;
    virtual void insert(const std::string& name, QueryMatcher *matcher) = 0;

    virtual ~MatcherTable() { }
};


}
