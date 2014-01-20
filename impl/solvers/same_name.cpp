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

#include "simple/condition_set.h"
#include "impl/solvers/same_name.h"
#include "simple/util/condition_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class SameNameStatementVisitor : public StatementVisitor {
  public:
    SameNameStatementVisitor(SameNameSolver *solver) : _solver(solver) { }

    void visit_assignment(AssignmentAst *assign) {
        _solver->index_name<AssignmentAst>(assign);
    }

    void visit_call(CallAst *call) {
        _solver->index_name<CallAst>(call);
    }

    void visit_if(IfAst *condition) {
        _solver->index_name<IfAst>(condition);
    }

    void visit_while(WhileAst *loop) {
        _solver->index_name<WhileAst>(loop);
    }

  private:
    SameNameSolver *_solver;
};

class SameNameExprVisitor : public ExprVisitor{
  public:
    SameNameExprVisitor(SameNameSolver *solver) : _solver(solver) { }

    void visit_variable(VariableAst *var) {
        _solver->index_name<VariableAst>(var);
    }

    void visit_const(ConstAst *constant) {
        _solver->index_name<ConstAst>(constant);
    }

    void visit_binary_op(BinaryOpAst *binop) {
        binop->get_lhs()->accept_expr_visitor(this);
        binop->get_rhs()->accept_expr_visitor(this);
    }

  private:
    SameNameSolver *_solver;
};

class GetNameVisitorTraits {
  public:
    typedef std::string ResultType;

    template <typename Condition>
    static std::string visit(SameNameSolver *solver, Condition *condition) {
        return solver->get_name<Condition>(condition);
    }
};

class HasNameVisitorTraits {
  public:
    typedef bool ResultType;

    template <typename Condition>
    static bool visit(SameNameSolver *solver, Condition *condition) {
        return solver->has_name<Condition>(condition);
    }
};

class SolveNameVisitorTraits {
  public:
    typedef ConditionSet ResultType;

    template <typename Condition>
    static ConditionSet visit(SameNameSolver *solver, Condition *condition) {
        return solver->solve_name<Condition>(condition);
    }
};


template <>
ConditionSet SameNameSolver::solve_name<ProcAst>(ProcAst *proc) {
    return _name_table[proc->get_name()];
}

template <>
ConditionSet SameNameSolver::solve_name<SimpleVariable>(SimpleVariable *var) {
    return _name_table[var->get_name()];
}

template <>
ConditionSet SameNameSolver::solve_name<SimpleCondition>(SimpleCondition *condition) {
    ConditionVisitorGenerator<SameNameSolver, SolveNameVisitorTraits>
    visitor(this);

    condition->accept_condition_visitor(&visitor);

    return visitor.return_result();
}

template <>
void SameNameSolver::index_name<ProcAst>(ProcAst *proc) {
    _name_table[proc->get_name()].insert(new SimpleProcCondition(proc));
    index_statement_list(proc->get_statement());
}

template <>
void SameNameSolver::index_name<SimpleVariable>(SimpleVariable *var) {
    _name_table[var->get_name()].insert(new SimpleVariableCondition(*var));
}

template <>
void SameNameSolver::index_name<VariableAst>(VariableAst *var) {
    index_name<SimpleVariable>(var->get_variable());
}

template <>
void SameNameSolver::index_name<AssignmentAst>(AssignmentAst *assign) {
    index_name<SimpleVariable>(assign->get_variable());
    index_name<ExprAst>(assign->get_expr());
}

template <>
void SameNameSolver::index_name<IfAst>(IfAst *condition) {
    index_name<SimpleVariable>(condition->get_variable());
    index_statement_list(condition->get_then_branch());
    index_statement_list(condition->get_else_branch());
}

template <>
void SameNameSolver::index_name<WhileAst>(WhileAst *loop) {
    index_name<SimpleVariable>(loop->get_variable());
    index_statement_list(loop->get_body());
}

template <>
void SameNameSolver::index_name<StatementAst>(StatementAst *statement) {
    SameNameStatementVisitor visitor(this);
    statement->accept_statement_visitor(&visitor);
}

template <>
void SameNameSolver::index_name<ExprAst>(ExprAst *expr) {
    SameNameExprVisitor visitor(this);
    expr->accept_expr_visitor(&visitor);
}

void SameNameSolver::index_statement_list(StatementAst *statement) {
    while(statement != NULL) {
        index_name<StatementAst>(statement);
        statement = statement->next();
    }
}

template <>
std::string SameNameSolver::get_name<ProcAst>(ProcAst *proc) {
    return proc->get_name();
}

template <>
std::string SameNameSolver::get_name<SimpleVariable>(SimpleVariable *var) {
    return var->get_name();
}

template <>
std::string SameNameSolver::get_name<SimpleCondition>(SimpleCondition *condition) {
    ConditionVisitorGenerator<SameNameSolver, GetNameVisitorTraits>
    visitor(this);

    condition->accept_condition_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool SameNameSolver::has_name<ProcAst>(ProcAst *proc) {
    return true;
}

template <>
bool SameNameSolver::has_name<SimpleVariable>(SimpleVariable *var) {
    return true;
}

template <>
bool SameNameSolver::has_name<SimpleCondition>(SimpleCondition *condition) {
    ConditionVisitorGenerator<SameNameSolver, HasNameVisitorTraits>
    visitor(this);

    condition->accept_condition_visitor(&visitor);
    return visitor.return_result();
}



}
}
