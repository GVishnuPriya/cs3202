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


#include "impl/condition.h"
#include "impl/solvers/modifies.h"
#include "simple/util/set_convert.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class IndexVariablesVisitorTraits {
  public:
    typedef VariableSet ResultType;
    typedef int ContextType;

    template <typename Ast>
    static VariableSet visit(
            AssignmentSolver *solver, Ast *ast, int *context = NULL) 
    {
        return solver->template index_variables<Ast>(ast);
    }
};

AssignmentSolver::AssignmentSolver(const SimpleRoot& ast, 
    std::shared_ptr<AssignStatementSolver> assign_solver) : 
    _ast(ast), _assign_solver(assign_solver)
{
    for(SimpleRoot::iterator it = _ast.begin(); it != _ast.end(); ++it) {
        index_variables<ProcAst>(*it);
    }
}

VariableSet AssignmentSolver::get_right_vars_from_statement(StatementAst *statement) {
    return _left_statement_index[statement];
}

VariableSet AssignmentSolver::get_right_vars_from_proc(ProcAst *proc) {
    return _left_proc_index[proc];
}

/*
 * solve_left() definitions
 */
template <>
ConditionSet AssignmentSolver::solve_left<SimpleVariable>(SimpleVariable *variable) {
    return _right_condition_index[*variable];
}

/*
 * validate() definitions
 */
template <>
bool AssignmentSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *statement, SimpleVariable *var) 
{
    return _left_statement_index[statement].count(*var) > 0;
}

template <>
bool AssignmentSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *proc, SimpleVariable *var)
{
    return _left_proc_index[proc].count(*var) > 0;
}

/*
 * solve_right() definitions
 */
template <>
ConditionSet AssignmentSolver::solve_right<StatementAst>(StatementAst *ast) {
    return variable_set_to_condition_set(_left_statement_index[ast]);
}

template <>
ConditionSet AssignmentSolver::solve_right<ProcAst>(ProcAst *proc) {
    return variable_set_to_condition_set(_left_proc_index[proc]);
}

/*
 * index_variable()
 */

void AssignmentSolver::index_statement_variables(StatementAst *statement, const VariableSet& variables) {
    _left_statement_index[statement] = variables;

    ConditionPtr statement_condition(new SimpleStatementCondition(statement));

    for(auto var : variables) {
        _right_condition_index[var].insert(statement_condition);
    }
}

VariableSet AssignmentSolver::index_statement_list(StatementAst *statement) {
    VariableSet result;

    while(statement != NULL) {
        VariableSet current_result = index_variables<StatementAst>(statement);
        union_set(result, current_result);
        statement = statement->next();
    }

    return result;
}

template <>
VariableSet AssignmentSolver::index_variables<ProcAst>(ProcAst *proc) {
    VariableSet result = index_statement_list(proc->get_statement());

    _left_proc_index[proc] = result;

    ConditionPtr proc_condition(new SimpleProcCondition(proc));
    for(auto var : result) {
        _right_condition_index[var].insert(new SimpleProcCondition(proc));
    }

    return result;
}

template <>
VariableSet AssignmentSolver::index_variables<AssignmentAst>(AssignmentAst *assign) {
    VariableSet result = _assign_solver->solve_assignment(assign);
    index_statement_variables(assign, result);

    return result;
}

template <>
VariableSet AssignmentSolver::index_variables<WhileAst>(WhileAst *ast) {
    VariableSet result = index_statement_list(ast->get_body());

    index_statement_variables(ast, result);

    return result;
}

template <>
VariableSet AssignmentSolver::index_variables<IfAst>(IfAst *ast) {
    VariableSet result = index_statement_list(ast->get_then_branch());
    VariableSet else_result = index_statement_list(ast->get_else_branch());

    union_set(result, else_result);
    index_statement_variables(ast, result);

    return result;
}

template <>
VariableSet AssignmentSolver::index_variables<CallAst>(CallAst *ast) {
    VariableSet result = index_variables<ProcAst>(ast->get_proc_called());

    index_statement_variables(ast, result);

    return result;
}

template <>
VariableSet AssignmentSolver::index_variables<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<AssignmentSolver, IndexVariablesVisitorTraits> 
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

} // namespace impl
} // namespace simple
