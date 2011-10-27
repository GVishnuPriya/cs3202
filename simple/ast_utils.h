
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
    return false;
}

template <typename Ast1, typename Ast2>
bool is_less_than_expr(Ast1 *ast1, Ast2 *ast2) {
    return false;
}


template <>
bool is_same_expr<ExprAst, ExprAst>(ExprAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<VariableAst, ExprAst>(VariableAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<ConstAst, ExprAst>(ConstAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<BinaryOpAst, ExprAst>(BinaryOpAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<VariableAst, VariableAst>(VariableAst *ast1, VariableAst *ast2);

template <>
bool is_same_expr<ConstAst, ConstAst>(ConstAst *ast1, ConstAst *ast2);

template <>
bool is_same_expr<BinaryOpAst, BinaryOpAst>(BinaryOpAst *ast1, BinaryOpAst *ast2);



} // namespace util 
} // namespace simple
