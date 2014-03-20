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

bool same_expr(ExprAst* expr1, ExprAst * expr2);
bool same_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2);
bool same_expr_var(VariableAst *expr1, ExprAst *expr2);
bool same_expr_const(ConstAst *expr1, ExprAst *expr2);

bool is_same_expr(ExprAst *expr1, ExprAst *expr2);

bool has_sub_expr(ExprAst *expr, ExprAst *sub_expr);

ExprAst* clone_expr(ExprAst *expr);

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