
#pragma once

#include "simple/condition_set.h"

namespace simple {

class SimplePredicate {
  public:
    virtual ConditionSet global_set() = 0;
    virtual void filter_set(ConditionSet& conditions) = 0;

    virtual ~SimplePredicate() { }
};

}
