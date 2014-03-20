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

#include "simple/util/expr_util.h"
#include "simple/util/ast_utils.h"

namespace simple {
namespace util {

class StatementTypeVisitor : public StatementVisitor {
  public:
    StatementTypeVisitor() { }

    void visit_if(IfAst*) {
        result = IfST;
    }

    void visit_while(WhileAst*) {
        result = WhileST;
    }

    void visit_assignment(AssignmentAst*) {
        result = AssignST;
    }

    void visit_call(CallAst*) {
        result = CallST;
    }

    StatementType result;
};

StatementType get_statement_type(StatementAst *statement) {
    StatementTypeVisitor visitor;
    statement->accept_statement_visitor(&visitor);
    return visitor.result;
}

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

    void visit_if(IfAst *stat2) {
        _result = is_same_statement<Ast, IfAst>(_stat1, stat2);
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

    void visit_if(IfAst *stat1) {
        SecondSameStatementVisitor<IfAst> visitor(stat1);
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
        is_same_expr(assign1->get_expr(), assign2->get_expr());
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
bool is_same_statement<IfAst, IfAst>(IfAst *condition1, IfAst *condition2) {
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

} // namespace util 
} // namespace simple
