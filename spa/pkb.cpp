
#include "impl/solver_table.h"
#include "spa/pkb.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

PKB::PKB(SimpleRoot ast, LineTable line_table) :
  _ast(ast), _line_table(line_table), _solver_table(create_solver_table(_ast)),
  _statement_table(line_table), _spa_ast(_ast, &_statement_table),
  _follows(_solver_table["follows"], &_statement_table),
  _follows_star(_solver_table["ifollows"], &_statement_table),
  _parent(_solver_table["parent"], &_statement_table),
  _parent_star(_solver_table["iparent"], &_statement_table),
  _uses(_solver_table["uses"], &_spa_ast),
  _modifies(_solver_table["modifies"], &_spa_ast),
  _calls(_solver_table["calls"], &_spa_ast),
  _calls_star(_solver_table["icalls"], &_spa_ast),
  _next(_solver_table["next"], &_statement_table),
  _next_star(_solver_table["inext"], &_statement_table),
  _affects(_solver_table["affects"], &_statement_table),
  _affects_star(_solver_table["iaffects"], &_statement_table)
{ }

AST* PKB::get_ast(){
    return &_spa_ast;
}

Follows* PKB::get_follows() {
    return &_follows;
}

FollowsStar* PKB::get_follows_star() {
    return &_follows_star;
}
  
Parent* PKB::get_parent() {
    return &_parent;
}
  
ParentStar* PKB::get_parent_star() {
    return &_parent_star;
}

Uses* PKB::get_uses() {
    return &_uses;
}

Modifies* PKB::get_modifies() {
    return &_modifies;
}
    
Calls* PKB::get_calls() {
    return &_calls;
}
      
CallsStar* PKB::get_calls_star() {
    return &_calls_star;
}
 
Next* PKB::get_next() {
    return &_next;
}
  
NextStar* PKB::get_next_star() {
    return &_next_star;
}
  
Affects* PKB::get_affects() {
    return &_affects;
}
  
AffectsStar* PKB::get_affects_star() {
    return &_affects_star;
}

}