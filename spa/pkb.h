
#pragma once

#include <memory>
#include "simple/ast.h"
#include "simple/solver.h"
#include "spa/ast.h"
#include "spa/statement.h"
#include "spa/follows.h"
#include "spa/follows_star.h"
#include "spa/parent.h"
#include "spa/parent_star.h"
#include "spa/uses.h"
#include "spa/modifies.h"
#include "spa/calls.h"
#include "spa/calls_star.h"
#include "spa/next.h"
#include "spa/next_star.h"
#include "spa/affects.h"
#include "spa/affects_star.h"

namespace spa {

using namespace simple;

class PKB {
  public:
    PKB(SimpleRoot ast, LineTable line_table);

    AST* get_ast();

    Follows* get_follows();
    FollowsStar* get_follows_star();
    
    Parent* get_parent();
    ParentStar* get_parent_star();

    Uses* get_uses();
  
    Modifies* get_modifies();
    
    Calls* get_calls();
    CallsStar* get_calls_star();

    Next* get_next();
    NextStar* get_next_star();
    
    Affects* get_affects();
    AffectsStar* get_affects_star();

  private:
    SimpleRoot _ast;
    LineTable _line_table;
    SolverTable _solver_table;
    StatementTable _statement_table;
    AST _spa_ast;
    Follows _follows;
    FollowsStar _follows_star;
    Parent _parent;
    ParentStar _parent_star;
    Uses _uses;
    Modifies _modifies;
    Calls _calls;
    CallsStar _calls_star;
    Next _next;
    NextStar _next_star;
    Affects _affects;
    AffectsStar _affects_star;
};

}