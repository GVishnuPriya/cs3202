
#include "simple/util/ast_utils.h"

namespace simple {
namespace util {

class FirstSameExprVisitor : public ExprVisitor {
  public:
    FirstSameExprVisitor(ExprAst *ast2);

    void visit_const(ConstAst*);
    void visit_variable(VariableAst*);
    void visit_binary_op(BinaryOpAst*);

    bool return_result();

    ~FirstSameExprVisitor();
  private:
    ExprAst *_ast2;
    bool    _result;
};

template <typename FirstExpr>
class SecondSameExprVisitor : public ExprVisitor {
  public:
    SecondSameExprVisitor(FirstExpr *ast1);

    void visit_const(ConstAst*);
    void visit_variable(VariableAst*);
    void visit_binary_op(BinaryOpAst*);

    bool return_result();

    ~SecondSameExprVisitor();
  private:
    FirstExpr   *_ast1;
    bool        _result;
};

template <>
bool is_same_expr<ExprAst, ExprAst>(ExprAst *ast1, ExprAst *ast2) {
    FirstSameExprVisitor visitor(ast2);
    ast1->accept_expr_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool is_same_expr<VariableAst, ExprAst>(VariableAst *ast1, ExprAst *ast2) {
    SecondSameExprVisitor<VariableAst> visitor(ast1);
    ast2->accept_expr_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool is_same_expr<ConstAst, ExprAst>(ConstAst *ast1, ExprAst *ast2) {
    SecondSameExprVisitor<ConstAst> visitor(ast1);
    ast2->accept_expr_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool is_same_expr<BinaryOpAst, ExprAst>(BinaryOpAst *ast1, ExprAst *ast2) {
    SecondSameExprVisitor<BinaryOpAst> visitor(ast1);
    ast2->accept_expr_visitor(&visitor);
    return visitor.return_result();
}
template <>
bool is_same_expr<VariableAst, VariableAst>(VariableAst *ast1, VariableAst *ast2) {
    return ast1->get_variable()->equals(*ast2->get_variable());
}

template <>
bool is_same_expr<ConstAst, ConstAst>(ConstAst *ast1, ConstAst *ast2) {
    return ast1->get_value() == ast2->get_value();
}

template <>
bool is_same_expr<BinaryOpAst, BinaryOpAst>(BinaryOpAst *ast1, BinaryOpAst *ast2) {
    return ast1->get_op() == ast2->get_op() &&
            is_same_expr<ExprAst, ExprAst>(ast1->get_lhs(), ast2->get_lhs()) &&
            is_same_expr<ExprAst, ExprAst>(ast1->get_rhs(), ast2->get_rhs());
}


FirstSameExprVisitor::FirstSameExprVisitor(ExprAst *ast2) : 
    _ast2(ast2), _result(false)
{ }

void FirstSameExprVisitor::visit_const(ConstAst *ast1) {
    _result = is_same_expr<ConstAst, ExprAst>(ast1, _ast2);
}

void FirstSameExprVisitor::visit_variable(VariableAst *ast1) {
    _result = is_same_expr<VariableAst, ExprAst>(ast1, _ast2);
}

void FirstSameExprVisitor::visit_binary_op(BinaryOpAst *ast1) {
    _result = is_same_expr<BinaryOpAst, ExprAst>(ast1, _ast2);
}

bool FirstSameExprVisitor::return_result() {
    return _result;
}

FirstSameExprVisitor::~FirstSameExprVisitor() { }

template <typename FirstExpr>
SecondSameExprVisitor<FirstExpr>::SecondSameExprVisitor(FirstExpr *ast1) :
    _ast1(ast1), _result(false)
{ }

template <typename FirstExpr>
void SecondSameExprVisitor<FirstExpr>::visit_const(ConstAst *ast2) {
    _result = is_same_expr<FirstExpr, ConstAst>(_ast1, ast2);
}

template <typename FirstExpr>
void SecondSameExprVisitor<FirstExpr>::visit_variable(VariableAst *ast2) {
    _result = is_same_expr<FirstExpr, VariableAst>(_ast1, ast2);
}

template <typename FirstExpr>
void SecondSameExprVisitor<FirstExpr>::visit_binary_op(BinaryOpAst *ast2) {
    _result = is_same_expr<FirstExpr, BinaryOpAst>(_ast1, ast2);
}

template <typename FirstExpr>
bool SecondSameExprVisitor<FirstExpr>::return_result() {
    return _result;
}

template <typename FirstExpr>
SecondSameExprVisitor<FirstExpr>::~SecondSameExprVisitor() { }

} // namespace util 
} // namespace simple
