
#pragma once

#include <stdexcept>
#include "simple/condition_set.h"
#include "spa/types.h"

namespace spa {

using namespace simple;

class VarTable {
  public:
    typedef VariableSet::iterator VarIterator;

    VarTable(VariableSet variables);

    int getVarIndex(Var var);

    Var getVarFromIndex(int index);

  private:
    VariableSet _variables;
    VarIterator _begin;
    VarIterator _end;
};

}