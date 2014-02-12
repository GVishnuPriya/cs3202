
#include "impl/solver_table.h"
#include "spa/pkb.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

PKB::PKB(SimpleRoot ast, LineTable line_table) :
  _ast(ast), _line_table(line_table), _solver_table(create_solver_table(_ast)),
  _statement_table(line_table), _spa_ast(_ast, &_statement_table),
  _follows(_solver_table["follows"], &_statement_table),
  _uses(_solver_table["uses"], &_statement_table),
  _cfg(_solver_table["next"], &_statement_table)
{ }

Follows* PKB::get_follows() {
    return &_follows;
}

Uses* PKB::get_uses() {
    return &_uses;
}

CFG* PKB::get_cfg() {
    return &_cfg;
}

}