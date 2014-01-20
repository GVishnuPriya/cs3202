/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *  Contributor(s):
 *    Daniel Le <GreenRecycleBin@gmail.com>
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


#include "simple/util/statement_visitor_generator.h"
#include "impl/solvers/uses.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

/*
 * Helper classes
 */
class UsesValidateStatementVisitor : public StatementVisitor {
  public:
    UsesValidateStatementVisitor(
        UsesSolver *solver, SimpleVariable *var) : 
        _solver(solver), _var(var) { }

    void visit_if(IfAst *ast) {
        _result = _solver->validate<IfAst, SimpleVariable>(ast, _var);
    }

    void visit_while(WhileAst *ast) {
        _result = _solver->validate<WhileAst, SimpleVariable>(ast, _var);
    }

    void visit_assignment(AssignmentAst *ast) {
        _result = _solver->validate<AssignmentAst, SimpleVariable>(ast, _var);
    }

    void visit_call(CallAst *ast) {
        _result = _solver->validate<CallAst, SimpleVariable>(ast, _var);
    }

    bool return_result() {
        return _result;
    }

  private:
    UsesSolver *_solver;
    SimpleVariable *_var;
    bool _result;
};

class UsesValidateExprVisitor : public ExprVisitor {
  public:
    UsesValidateExprVisitor(UsesSolver *solver, SimpleVariable *var) :
    _solver(solver), _var(var) { }

    void visit_const(ConstAst* ast) {
      _result = _solver->validate<ConstAst, SimpleVariable>(ast, _var);
    }

    void visit_variable(VariableAst* ast) {
      _result = _solver->validate<VariableAst, SimpleVariable>(ast, _var);
    }

    void visit_binary_op(BinaryOpAst* ast) {
      _result = _solver->validate<BinaryOpAst, SimpleVariable>(ast, _var);
    }

    bool return_result() {
      return _result;
    }

  private:
    UsesSolver *_solver;
    SimpleVariable *_var;
    bool _result;
};

UsesSolver::UsesSolver(const SimpleRoot& ast) : 
    _ast(ast) 
{
   for(SimpleRoot::iterator it = _ast.begin(); it != _ast.end(); ++it) {
       index_variables<ProcAst>(*it);
   } 
}

/*
 * validate() definitions
 */
template <>
bool UsesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *ast, SimpleVariable *var) 
{
    UsesValidateStatementVisitor visitor(this, var);
    ast->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool UsesSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *ast, SimpleVariable *var)
{
    StatementAst *body = ast->get_statement();
    while(body != NULL) {
        if(validate<StatementAst, SimpleVariable>(body, var)) {
            return true;
        }
        body = body->next();
    }

    return false;
}

template <>
bool UsesSolver::validate<AssignmentAst, SimpleVariable>(
        AssignmentAst *ast, SimpleVariable *var) 
{
    return validate<ExprAst, SimpleVariable>(ast->get_expr(), var);
}

template <>
bool UsesSolver::validate<ExprAst, SimpleVariable>(
        ExprAst *ast, SimpleVariable *var)
{
    UsesValidateExprVisitor visitor(this, var);
    ast->accept_expr_visitor(&visitor);
  
    return visitor.return_result();
}

template <>
bool UsesSolver::validate<VariableAst, SimpleVariable>(
        VariableAst *ast, SimpleVariable *var) 
{
    return *ast->get_variable() == *var;
}

template <>
bool UsesSolver::validate<ConstAst, SimpleVariable>(
        ConstAst *ast, SimpleVariable *var)
{
    return false;
}

template <>
bool UsesSolver::validate<BinaryOpAst, SimpleVariable>(
        BinaryOpAst *ast, SimpleVariable *var) 
{
    if (validate<ExprAst, SimpleVariable>(ast->get_lhs(), var)) {
      return true;
    }

    if (validate<ExprAst, SimpleVariable>(ast->get_rhs(), var)) {
      return true;
    }

    return false;
}

template <>
bool UsesSolver::validate<IfAst, SimpleVariable>(
        IfAst *ast, SimpleVariable *var) 
{
    if (ast->get_variable()->equals(*var)) {
      return true;
    }

    StatementAst *then_branch = ast->get_then_branch();
    while(then_branch != NULL) {
        if(validate<StatementAst, SimpleVariable>(then_branch, var)) {
            return true;
        }
        then_branch = then_branch->next();
    }

    StatementAst *else_branch = ast->get_else_branch();
    while(else_branch != NULL) {
        if(validate<StatementAst, SimpleVariable>(else_branch, var)) {
            return true;
        }
        else_branch = else_branch->next();
    }

    return false;
}

template <>
bool UsesSolver::validate<WhileAst, SimpleVariable>(
        WhileAst *ast, SimpleVariable *var)
{
    if (ast->get_variable()->equals(*var)) {
      return true;
    }

    StatementAst *body = ast->get_body();
    while(body != NULL) {
        if(validate<StatementAst, SimpleVariable>(body, var)) {
            return true;
        }
        body = body->next();
    }

    return false;
}

template <>
bool UsesSolver::validate<CallAst, SimpleVariable>(
        CallAst *ast, SimpleVariable *var)
{
    return validate<ProcAst, SimpleVariable>(ast->get_proc_called(), var);
}


/*
 * solve_right() definitions
 */
template <>
ConditionSet UsesSolver::solve_right<StatementAst>(StatementAst *ast) {
    StatementVisitorGenerator<UsesSolver, 
        SolveRightVisitorTraits<UsesSolver> > visitor(this);

    ast->accept_statement_visitor(&visitor);

    return visitor.return_result();
}

template <>
ConditionSet UsesSolver::solve_right<IfAst>(IfAst *ast) {
    ConditionSet result;
    result.insert(new SimpleVariableCondition(*ast->get_variable()));
    
    StatementAst *then = ast->get_then_branch();
    while(then != NULL) {
        result.union_with(solve_right<StatementAst>(then));
        then = then->next();
    }

    StatementAst *el = ast->get_else_branch();
    while(el != NULL) {
        result.union_with(solve_right<StatementAst>(el));
        el = el->next();
    }

    return result;
}

template <>
ConditionSet UsesSolver::solve_right<WhileAst>(WhileAst *ast) {
    ConditionSet result;
    result.insert(new SimpleVariableCondition(*ast->get_variable()));

    StatementAst *body = ast->get_body();
    while(body != NULL) {
        result.union_with(solve_right<StatementAst>(body));
        body = body->next();
    }

    return result;
}

template <>
ConditionSet UsesSolver::solve_right<ProcAst>(ProcAst *ast) {
    ConditionSet result;
    
    StatementAst *body = ast->get_statement();
    while(body != NULL) {
        result.union_with(solve_right<StatementAst>(body));
        body = body->next();
    }

    return result;
}

template <>
ConditionSet UsesSolver::solve_right<AssignmentAst>(AssignmentAst *ast) {
    return solve_right<ExprAst>(ast->get_expr());
}

template <>
ConditionSet UsesSolver::solve_right<ExprAst>(ExprAst *ast) {
    ExprVisitorGenerator<UsesSolver, 
        SolveRightVisitorTraits<UsesSolver> > visitor(this);

    ast->accept_expr_visitor(&visitor);

    return visitor.return_result();  
}

template <>
ConditionSet UsesSolver::solve_right<VariableAst>(VariableAst *ast) {
    ConditionSet result;
    result.insert(new SimpleVariableCondition(
            *(ast->get_variable())));

    return result;
}

template <>
ConditionSet UsesSolver::solve_right<ConstAst>(ConstAst *ast) {
    ConditionSet result;

    return result;
}

template <>
ConditionSet UsesSolver::solve_right<BinaryOpAst>(BinaryOpAst *ast) {
    ConditionSet result;

    result.union_with(solve_right<ExprAst>(ast->get_lhs()));
    result.union_with(solve_right<ExprAst>(ast->get_rhs()));

    return result;
}

template <>
ConditionSet UsesSolver::solve_right<CallAst>(CallAst *ast) {
    return solve_right<ProcAst>(ast->get_proc_called());
}

} // namespace impl
} // namespace simple
