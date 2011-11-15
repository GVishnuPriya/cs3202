/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "simple/util/ast_utils.h"

namespace simple {
namespace util {

template <typename Ast>
class SecondSameStatementVisitor : public StatementVisitor {
  public:
    SecondSameStatementVisitor(Ast *stat1) : _stat1(stat1) { }

    void visit_assignment(AssignmentAst *stat2) {
        _result = is_same_statement<Ast, AssignmentAst>(_stat1, stat2);
    }

    void visit_call(CallAst *stat2) {
        _result = is_same_statement<Ast, CallAst>(_stat1, stat2);
    }

    void visit_conditional(ConditionalAst *stat2) {
        _result = is_same_statement<Ast, ConditionalAst>(_stat1, stat2);
    }

    void visit_while(WhileAst *stat2) {
        _result = is_same_statement<Ast, WhileAst>(_stat1, stat2);
    }

    bool return_result() {
        return _result;
    }

  private:
    Ast *_stat1;
    bool _result;
};

class FirstSameStatementVisitor : public StatementVisitor {
  public:
    FirstSameStatementVisitor(StatementAst *stat2) :
        _stat2(stat2)
    { }

    void visit_assignment(AssignmentAst *stat1) {
        SecondSameStatementVisitor<AssignmentAst> visitor(stat1);
        _stat2->accept_statement_visitor(&visitor);
        _result = visitor.return_result();
    }

    void visit_call(CallAst *stat1) {
        SecondSameStatementVisitor<CallAst> visitor(stat1);
        _stat2->accept_statement_visitor(&visitor);
        _result = visitor.return_result();
    }

    void visit_conditional(ConditionalAst *stat1) {
        SecondSameStatementVisitor<ConditionalAst> visitor(stat1);
        _stat2->accept_statement_visitor(&visitor);
        _result = visitor.return_result();
    }

    void visit_while(WhileAst *stat1) {
        SecondSameStatementVisitor<WhileAst> visitor(stat1);
        _stat2->accept_statement_visitor(&visitor);
        _result = visitor.return_result();
    }

    bool return_result() {
        return _result;
    }

  private:
    StatementAst *_stat2;
    bool _result;
};

template <>
bool is_same_statement<StatementAst, StatementAst>(StatementAst *stat1, StatementAst *stat2) {
    FirstSameStatementVisitor visitor(stat2);
    stat1->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool is_same_statement<AssignmentAst, AssignmentAst>(AssignmentAst *assign1, AssignmentAst *assign2) {
    return *assign1->get_variable() == *assign2->get_variable() &&
        is_same_expr<ExprAst, ExprAst>(assign1->get_expr(), assign2->get_expr());
}

template <>
bool is_same_statement<CallAst, CallAst>(CallAst *call1, CallAst *call2) {
    return call1->get_proc_called()->get_name() == 
           call2->get_proc_called()->get_name();
}

template <>
bool is_same_statement<WhileAst, WhileAst>(WhileAst *loop1, WhileAst *loop2) {
    return (*loop1->get_variable() == *loop2->get_variable()) &&
        is_same_statement_list(loop1->get_body(), loop2->get_body());
}

template <>
bool is_same_statement<ConditionalAst, ConditionalAst>(ConditionalAst *condition1, ConditionalAst *condition2) {
    return (*condition1->get_variable() == *condition2->get_variable()) &&
        is_same_statement_list(condition1->get_then_branch(), condition2->get_then_branch()) &&
        is_same_statement_list(condition1->get_else_branch(), condition2->get_else_branch());
}

bool is_same_statement_list(StatementAst *statement1, StatementAst *statement2) {
    while(statement1 != NULL) {
        if(statement2 == NULL) {
            return false;
        }

        if(!is_same_statement<StatementAst, StatementAst>(statement1, statement2)) {
            return false;
        }

        statement1 = statement1->next();
        statement2 = statement2->next();
    }

    if(statement2 != NULL) {
        return false;
    } else {
        return true;
    }
}

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
