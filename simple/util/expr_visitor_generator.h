
#pragma once

#include "simple/ast.h"

namespace simple {
namespace util {

template <typename DispatchTraits, typename Expr1>
class SecondExprVisitor : public ExprVisitor {
  public:
    typedef typename DispatchTraits::ResultType ResultType;

    SecondExprVisitor(Expr1 *expr1) : _expr1(expr1) { }

    template <typename Expr2>
    void do_double_dispatch(Expr2 *expr2) {
        _result = DispatchTraits::template double_dispatch<Expr1, Expr2>(
            _expr1, expr2);
    }

    void visit_const(ConstAst *ast) {
        do_double_dispatch<ConstAst>(ast);
    }

    void visit_variable(VariableAst *ast) {
        do_double_dispatch<VariableAst>(ast);
    }

    void visit_binary_op(BinaryOpAst *ast) {
        do_double_dispatch<BinaryOpAst>(ast);
    }

    ResultType return_result() {
        return _result;
    }

  private:
    Expr1 *_expr1;
    ResultType _result;
};

template <typename DispatchTraits>
class FirstExprVisitor : public ExprVisitor {
  public:
    typedef typename DispatchTraits::ResultType ResultType;

    FirstExprVisitor(ExprAst *expr2) : _expr2(expr2) { }

    template <typename Expr>
    void do_second_visit(Expr *expr1) {
        SecondExprVisitor<DispatchTraits, Expr> visitor(expr1);
        _expr2->accept_expr_visitor(&visitor);
        _result = visitor.return_result();
    }

    void visit_const(ConstAst *ast) {
        do_second_visit<ConstAst>(ast);
    }

    void visit_variable(VariableAst *ast) {
        do_second_visit<VariableAst>(ast);
    }

    void visit_binary_op(BinaryOpAst *ast) {
        do_second_visit<BinaryOpAst>(ast);
    }

    ResultType return_result() {
        return _result;
    }

  private:
    ExprAst *_expr2;
    ResultType _result;
};

template <typename DispatchTraits>
typename DispatchTraits::ResultType
double_dispatch_expr(ExprAst *expr1, ExprAst *expr2) {
    FirstExprVisitor<DispatchTraits> visitor(expr2);
    expr1->accept_expr_visitor(&visitor);
    return visitor.return_result();
}

}
}