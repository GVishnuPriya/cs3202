#pragma once

#include "simple/ast.h"
#include "simple/condition_set.h"
#include "simple/util/expr_visitor_generator.h"

namespace simple{
namespace util {

enum ExprType {
  BinaryOpET,
  VariableET,
  ConstantET
};

ExprType get_expr_type(ExprAst *expr);

VariableSet get_expr_vars(ExprAst *ast);

template <typename Expr>
Expr* expr_cast(ExprAst *expr);

std::string expr_to_string(ExprAst *ast);

bool is_same_expr(ExprAst *expr1, ExprAst *expr2);
bool has_sub_expr(ExprAst *expr, ExprAst *sub_expr);

template <typename Ast1, typename Ast2>
bool is_less_than_expr(Ast1 *ast1, Ast2 *ast2) {
    return false;
}

template <typename SourceExpr, typename TargetExpr>
struct ExprCaster {
    static TargetExpr* cast(SourceExpr *expr) {
      return NULL;
    }
};

template <typename Expr>
struct ExprCaster<Expr, Expr> {
    static Expr* cast(Expr *expr) {
      return expr;
    }
};

template <typename TargetExpr>
struct ExprCastVisitorTraits {
    typedef TargetExpr* ResultType;

    template <typename Expr>
    static TargetExpr* visit(Expr *expr) {
      return ExprCaster<Expr, TargetExpr>::cast(expr);
    }
};

template <typename Expr>
Expr* expr_cast(ExprAst *expr) {
  return single_dispatch_expr<
    ExprCastVisitorTraits<Expr> >(expr);
}


}
}