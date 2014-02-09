#pragma once

#include "simple/ast.h"
#include "simple/condition_set.h"

namespace simple{

VariableSet get_expr_vars(ExprAst *ast);

}