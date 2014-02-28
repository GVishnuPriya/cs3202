
#include <sstream>
#include "simple/util/expr.h"

namespace simple{
namespace util {

class ExprTypeVisitor : public ExprVisitor {    
  public:
    ExprTypeVisitor() { }

    void visit_const(ConstAst* ) { 
        result = ExprType::ConstantET;
    }

    void visit_variable(VariableAst* ast) {
        result = ExprType::VariableET;
    }
    
    void visit_binary_op(BinaryOpAst* ast) {
        result = ExprType::BinaryOpET;
    }

    ExprType result;
};

ExprType get_expr_type(ExprAst *expr) {
    ExprTypeVisitor visitor;
    expr->accept_expr_visitor(&visitor);

    return visitor.result;
}

class GetExprVarsVisitor : public ExprVisitor {    
  public:
    GetExprVarsVisitor() { }

    void visit_const(ConstAst* ) { }

    void visit_variable(VariableAst* ast) {
        result.insert(*ast->get_variable());
    }
    
    void visit_binary_op(BinaryOpAst* ast) {
        VariableSet lhs_result = get_expr_vars(ast->get_lhs());
        VariableSet rhs_result = get_expr_vars(ast->get_rhs());
        union_set(result, lhs_result);
        union_set(result, rhs_result);
    }

    VariableSet result;
};

VariableSet get_expr_vars(ExprAst *ast) {
    GetExprVarsVisitor visitor;
    ast->accept_expr_visitor(&visitor);
    return visitor.result;
}

class ExprToStringVisitor : public ExprVisitor {    
  public:
    ExprToStringVisitor() { }

    void visit_const(ConstAst* ast) {
        std::stringstream ss;
        ss << ast->get_constant();
        result = ss.str();
    }

    void visit_variable(VariableAst* ast) {
        result = *ast->get_variable();
    }
    
    void visit_binary_op(BinaryOpAst* ast) {
        char op = ast->get_op();
        std::string lhs = expr_to_string(ast->get_lhs());
        std::string rhs = expr_to_string(ast->get_rhs());

        std::stringstream ss;
        ss << "(" << lhs << " " << op << " " << rhs << ")";

        result = ss.str();
    }

    std::string result;
};

std::string expr_to_string(ExprAst *ast) {
    ExprToStringVisitor visitor;
    ast->accept_expr_visitor(&visitor);
    return visitor.result;
}

template <typename Expr1, typename Expr2>
bool solve_same_expr(Expr1 *expr1, Expr2 *expr2) {
    return false;
}

template <>
bool solve_same_expr<VariableAst, VariableAst>(
    VariableAst *expr1, VariableAst *expr2)
{
    return *expr1->get_variable() == *expr2->get_variable();
}

template <>
bool solve_same_expr<ConstAst, ConstAst>(
    ConstAst *expr1, ConstAst *expr2)
{
    return *expr1->get_constant() == *expr2->get_constant();
}

template <>
bool solve_same_expr<BinaryOpAst, BinaryOpAst>(
    BinaryOpAst *expr1, BinaryOpAst *expr2)
{
    if(expr1->get_op() != expr2->get_op()) return false;
    if(!is_same_expr(expr1->get_lhs(), expr2->get_lhs())) return false;
    return is_same_expr(expr1->get_rhs(), expr2->get_rhs());
}

struct SameExprDispatchTraits {
  public:
    typedef bool ResultType;

    template <typename Expr1, typename Expr2>
    static bool double_dispatch(Expr1 *expr1, Expr2 *expr2) {
        return solve_same_expr<Expr1, Expr2>(expr1, expr2);
    }
};

bool is_same_expr(ExprAst *expr1, ExprAst *expr2) {
    return double_dispatch_expr<SameExprDispatchTraits>(expr1, expr2);
}

template <typename Expr1, typename Expr2>
struct SubExprSolver {
    static bool solve_sub_expr(Expr1 *expr1, Expr2 *expr2) {
        return false;
    }
};

template <>
struct SubExprSolver<VariableAst, VariableAst> {
    static bool solve_sub_expr(VariableAst *expr1, VariableAst *expr2)
    {
        return solve_same_expr<VariableAst, VariableAst>(expr1, expr2);
    }
};

template <>
struct SubExprSolver<ConstAst, ConstAst> {
    static bool solve_sub_expr(ConstAst *expr1, ConstAst *expr2)
    {
        return solve_same_expr<ConstAst, ConstAst>(expr1, expr2);
    }
};

template <typename SubExpr>
struct SubExprSolver<BinaryOpAst, SubExpr> {
    static bool solve_sub_expr(BinaryOpAst *expr, SubExpr *sub_expr)
    {
        if(solve_same_expr<BinaryOpAst, SubExpr>(expr, sub_expr)) return true;

        return has_sub_expr(expr->get_lhs(), sub_expr) || 
            has_sub_expr(expr->get_rhs(), sub_expr);
    }
};

struct SubExprDispatchTraits {
  public:
    typedef bool ResultType;

    template <typename Expr1, typename Expr2>
    static bool double_dispatch(Expr1 *expr1, Expr2 *expr2) {
        return SubExprSolver<Expr1, Expr2>::solve_sub_expr(expr1, expr2);
    }
};

bool has_sub_expr(ExprAst *expr, ExprAst *sub_expr) {
    return double_dispatch_expr<SubExprDispatchTraits>(expr, sub_expr);
}

}
}