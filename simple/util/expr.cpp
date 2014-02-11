
#include <sstream>
#include "simple/util/expr.h"

namespace simple{
namespace util {

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

}
}