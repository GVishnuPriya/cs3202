
#include "impl/expr.h"

namespace simple{

class ExprVisitorHelper : public ExprVisitor {    
  public:
    ExprVisitorHelper() {}
    VariableSet result;

    void visit_const(ConstAst* ) {}

    void visit_variable(VariableAst* ast) {
        result.insert(*ast->get_variable());
    }
    
    void visit_binary_op(BinaryOpAst* ast) {
        VariableSet lhs_result = get_expr_vars(ast->get_lhs());
        VariableSet rhs_result = get_expr_vars(ast->get_rhs());
        union_set(result, lhs_result);
        union_set(result, rhs_result);
    }
};

VariableSet get_expr_vars(ExprAst *ast) {
    ExprVisitorHelper helper;
    ast->accept_expr_visitor(&helper);
    return helper.result;
}  

}