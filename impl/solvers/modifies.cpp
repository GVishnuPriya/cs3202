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

template <typename Solver>
class IndexModifiesVisitorTraits {
  public:
    typedef VariableSet ResultType;
    typedef int ContextType;

    template <typename Ast>
    static VariableSet visit(
            Solver *solver, Ast *ast, int *context = NULL) 
    {
        return solver->template index_modifies<Ast>(ast);
    }
  private:
    IndexModifiesVisitorTraits();
};

ModifiesSolver::ModifiesSolver(const SimpleRoot& ast) : 
    _ast(ast) 
{
   for(SimpleRoot::iterator it = _ast.begin(); it != _ast.end(); ++it) {
       index_modifies<ProcAst>(*it);
   } 
}

/*
 * solve_left() definitions
 */
template <>
ConditionSet ModifiesSolver::solve_left<SimpleVariable>(SimpleVariable *variable) {
    return _modifying_condition_index[*variable];
}

/*
 * validate() definitions
 */
template <>
bool ModifiesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *statement, SimpleVariable *var) 
{
    return _modified_by_statement_index[statement].count(*var) > 0;
}

template <>
bool ModifiesSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *proc, SimpleVariable *var)
{
    return _modified_by_proc_index[proc].count(*var) > 0;
}

/*
 * solve_right() definitions
 */
template <>
ConditionSet ModifiesSolver::solve_right<StatementAst>(StatementAst *ast) {
    return variable_set_to_condition_set(_modified_by_statement_index[ast]);
}

template <>
ConditionSet ModifiesSolver::solve_right<ProcAst>(ProcAst *proc) {
    return variable_set_to_condition_set(_modified_by_proc_index[proc]);
}

/*
 * index_variable()
 */

void ModifiesSolver::index_statement(StatementAst *statement, const SimpleVariable& variable) {
    _modified_by_statement_index[statement].insert(variable);
    _modifying_condition_index[variable].insert(new SimpleStatementCondition(statement));
}

void ModifiesSolver::index_container_statement(StatementAst *statement, const VariableSet& variables) {
    _modified_by_statement_index[statement] = variables;

    ConditionPtr statement_condition(new SimpleStatementCondition(statement));

    for(auto var : variables) {
        _modifying_condition_index[var].insert(statement_condition);
    }
}

VariableSet ModifiesSolver::index_statement_list(StatementAst *statement) {
    VariableSet result;

    while(statement != NULL) {
        VariableSet current_result = index_modifies<StatementAst>(statement);
        union_set(result, current_result);
        statement = statement->next();
    }

    return result;
}

template <>
VariableSet ModifiesSolver::index_modifies<ProcAst>(ProcAst *proc) {
    VariableSet result = index_statement_list(proc->get_statement());

    _modified_by_proc_index[proc] = result;

    ConditionPtr proc_condition(new SimpleProcCondition(proc));
    for(auto var : result) {
        _modifying_condition_index[var].insert(new SimpleProcCondition(proc));
    }

    return result;
}

template <>
VariableSet ModifiesSolver::index_modifies<AssignmentAst>(AssignmentAst *assign) {
    SimpleVariable modified_var = *assign->get_variable();

    index_statement(assign, modified_var);

    VariableSet result;
    result.insert(modified_var);
    
    return result;
}

template <>
VariableSet ModifiesSolver::index_modifies<WhileAst>(WhileAst *ast) {
    VariableSet result = index_statement_list(ast->get_body());

    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet ModifiesSolver::index_modifies<IfAst>(IfAst *ast) {
    VariableSet result = index_statement_list(ast->get_then_branch());
    VariableSet else_result = index_statement_list(ast->get_else_branch());

    union_set(result, else_result);
    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet ModifiesSolver::index_modifies<CallAst>(CallAst *ast) {
    VariableSet result = index_modifies<ProcAst>(ast->get_proc_called());

    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet ModifiesSolver::index_modifies<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<ModifiesSolver,
        IndexModifiesVisitorTraits<ModifiesSolver> > 
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

} // namespace impl
} // namespace simple
