
#pragma once

#include <set>
#include <memory>
#include <stdexcept>
#include "simple/ast.h"
#include "simple/condition_set.h"
#include "simple/util/condition_utils.h"

namespace spa {

using namespace simple;
using namespace simple::util;

typedef std::string Var;
typedef std::set<Var> VarResults;

typedef int StatementLine;
typedef std::set<int> StatementResults;

class StatementTable {
  public:
    StatementTable(LineTable table);

    bool is_valid_statement(StatementLine line);

    StatementAst* get_statement(StatementLine line);

  private:
    LineTable _table;
};

StatementResults condition_to_statement_results(const ConditionSet& conditions);

VarResults condition_to_variable_results(const ConditionSet& conditions);

}