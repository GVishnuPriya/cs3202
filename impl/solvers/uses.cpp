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
#include "impl/solvers/uses.h"
#include "simple/util/set_convert.h"
#include "simple/util/statement_visitor_generator.h"
#include "simple/util/expr.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

template <typename Solver>
class IndexUsesVisitorTraits {
  public:
    typedef VariableSet ResultType;
    typedef int                    ContextType;

    template <typename Ast>
    static VariableSet visit(
            Solver *solver, Ast *ast, int *context = NULL) 
    {
        return solver->template index_uses<Ast>(ast);
    }
  private:
    IndexUsesVisitorTraits();
};

UsesSolver::UsesSolver(const SimpleRoot& ast) : 
    _ast(ast) 
{
   for(SimpleRoot::iterator it = _ast.begin(); it != _ast.end(); ++it) {
       index_uses<ProcAst>(*it);
   } 
}

VariableSet UsesSolver::solve_used_vars(StatementAst *statement) {
    return _used_by_statement_index[statement];
}

StatementSet UsesSolver::solve_using_statements(const SimpleVariable& variable) {
    return _using_statement_index[variable];
}

/*
 * solve_left() definitions
 */
template <>
ConditionSet UsesSolver::solve_left<SimpleVariable>(SimpleVariable *variable) {
    return _using_condition_index[*variable];
}

/*
 * validate() definitions
 */
template <>
bool UsesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *statement, SimpleVariable *var) 
{
    return _used_by_statement_index[statement].count(*var) > 0;
}

template <>
bool UsesSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *ast, SimpleVariable *var)
{
    return _using_condition_index[*var].has_element(new SimpleProcCondition(ast));
}

/*
 * solve_right() definitions
 */
template <>
ConditionSet UsesSolver::solve_right<StatementAst>(StatementAst *ast) {
    return variable_set_to_condition_set(_used_by_statement_index[ast]);
}

template <>
ConditionSet UsesSolver::solve_right<ProcAst>(ProcAst *ast) {
    return _used_by_condition_index[new SimpleProcCondition(ast)];
}

/*
 * index_variable()
 */

void UsesSolver::index_statement(StatementAst *statement, const SimpleVariable& variable) {
    _using_statement_index[variable].insert(statement);
    _used_by_statement_index[statement].insert(variable);

    index_condition(new SimpleStatementCondition(statement), variable);
}

void UsesSolver::index_condition(ConditionPtr condition, const SimpleVariable& variable) {
    _using_condition_index[variable].insert(condition);
    _used_by_condition_index[condition].insert(new SimpleVariableCondition(variable));
}

void UsesSolver::index_container_statement(StatementAst *statement, const VariableSet& variables) {
    ConditionPtr condition = new SimpleStatementCondition(statement);

    for(auto it = variables.begin(); it != variables.end(); ++it) {
        index_statement(statement, *it);
    }
}

void UsesSolver::index_container_condition(ConditionPtr condition, const VariableSet& variables) {
    for(auto it = variables.begin(); it != variables.end(); ++it) {
        index_condition(condition, *it);
    }
}

VariableSet UsesSolver::index_statement_list(StatementAst *statement) {
    VariableSet result;

    while(statement != NULL) {
        VariableSet current_result = index_uses<StatementAst>(statement);
        union_set(result, current_result);
        statement = statement->next();
    }

    return result;
}

template <>
VariableSet UsesSolver::index_uses<ProcAst>(ProcAst *proc) {
    VariableSet result = index_statement_list(proc->get_statement());

    index_container_condition(new SimpleProcCondition(proc), result);

    return result;
}

template <>
VariableSet UsesSolver::index_uses<AssignmentAst>(AssignmentAst *assign) {
    ExprAst* used_expr = assign->get_expr();
    VariableSet result = get_expr_vars(used_expr);

    for(VariableSet::iterator it = result.begin(); it != result.end(); ++it) {
        index_statement(assign, *it);
    }

    return result;
}

template <>
VariableSet UsesSolver::index_uses<WhileAst>(WhileAst *ast) {
    VariableSet result = index_statement_list(ast->get_body());
    SimpleVariable var_used = *ast->get_variable();
    result.insert(var_used);
    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet UsesSolver::index_uses<IfAst>(IfAst *ast) {
    VariableSet result = index_statement_list(ast->get_then_branch());
    VariableSet else_result = index_statement_list(ast->get_else_branch());
    SimpleVariable var_used = *ast->get_variable();
    
    union_set(result, else_result); 
    result.insert(var_used);
    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet UsesSolver::index_uses<CallAst>(CallAst *ast) {
    VariableSet result = index_uses<ProcAst>(ast->get_proc_called());

    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet UsesSolver::index_uses<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<UsesSolver,
        IndexUsesVisitorTraits<UsesSolver> > 
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

} // namespace impl
} // namespace simple
