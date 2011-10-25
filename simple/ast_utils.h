
#pragma once

#include "simple/ast.h"

namespace simple {
namespace util {

using namespace simple;

template <typename Ast>
bool is_statement_type(StatementAst *ast) {
    return false; // stub
}

template <typename Ast1, typename Ast2>
bool is_same_expr(Ast1 *ast1, Ast2 *ast2) {
    return false; // stub
}

} // namespace util 
} // namespace simple
