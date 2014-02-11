#pragma once

#include "simple/ast.h"
#include "simple/condition_set.h"

namespace simple{
namespace util {

VariableSet get_expr_vars(ExprAst *ast);

std::string expr_to_string(ExprAst *ast);

bool same_expr(ExprAst *expr1, ExprAst *expr2);
bool has_sub_expr(ExprAst *expr, ExprAst *sub_expr);

}
}