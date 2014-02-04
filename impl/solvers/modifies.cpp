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

/*
 * Helper classes
 */
class ModifiesValidateStatementVisitor : public StatementVisitor {
  public:
    ModifiesValidateStatementVisitor(
        ModifiesSolver *solver, SimpleVariable *var) : 
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
    ModifiesSolver *_solver;
    SimpleVariable *_var;
    bool _result;
};

ModifiesSolver::ModifiesSolver(const SimpleRoot& ast) : 
    _ast(ast) 
{
   for(SimpleRoot::iterator it = _ast.begin(); it != _ast.end(); ++it) {
       index_variables<ProcAst>(*it);
   } 
}

VariableSet ModifiesSolver::solve_modified_vars(StatementAst *statement) {
    return solve_modified<StatementAst>(statement);
}

StatementSet ModifiesSolver::solve_modifying_statements(const SimpleVariable& variable) {
    if(_modifying_statement_index.count(variable)) {
        return _modifying_statement_index[variable];
    } else {
        return StatementSet();
    }
}

/*
 * solve_left() definitions
 */
template <>
ConditionSet ModifiesSolver::solve_left<SimpleVariable>(SimpleVariable *variable) {
    if(_modifying_condition_index.count(*variable)) {
        return _modifying_condition_index[*variable];
    } else {
        return ConditionSet();
    }
}

/*
 * validate() definitions
 */
template <>
bool ModifiesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *ast, SimpleVariable *var) 
{
    ModifiesValidateStatementVisitor visitor(this, var);
    ast->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool ModifiesSolver::validate<ProcAst, SimpleVariable>(
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
bool ModifiesSolver::validate<AssignmentAst, SimpleVariable>(
        AssignmentAst *ast, SimpleVariable *var) 
{
    SimpleVariable *v = ast->get_variable();
    return v->equals(*var);
}

template <>
bool ModifiesSolver::validate<IfAst, SimpleVariable>(
        IfAst *ast, SimpleVariable *var) 
{
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
bool ModifiesSolver::validate<WhileAst, SimpleVariable>(
        WhileAst *ast, SimpleVariable *var)
{
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
bool ModifiesSolver::validate<CallAst, SimpleVariable>(
        CallAst *ast, SimpleVariable *var)
{
    return validate<ProcAst, SimpleVariable>(ast->get_proc_called(), var);
}


/*
 * solve_right() definitions
 */
template <>
ConditionSet ModifiesSolver::solve_right<StatementAst>(StatementAst *ast) {
    StatementVisitorGenerator<ModifiesSolver, 
        SolveRightVisitorTraits<ModifiesSolver> > visitor(this);

    ast->accept_statement_visitor(&visitor);

    return visitor.return_result();
}

template <>
ConditionSet ModifiesSolver::solve_right<IfAst>(IfAst *ast) {
    ConditionSet result;
    
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
ConditionSet ModifiesSolver::solve_right<WhileAst>(WhileAst *ast) {
    ConditionSet result;

    StatementAst *body = ast->get_body();
    while(body != NULL) {
        result.union_with(solve_right<StatementAst>(body));
        body = body->next();
    }

    return result;
}

template <>
ConditionSet ModifiesSolver::solve_right<ProcAst>(ProcAst *ast) {
    ConditionSet result;
    
    StatementAst *body = ast->get_statement();
    while(body != NULL) {
        result.union_with(solve_right<StatementAst>(body));
        body = body->next();
    }

    return result;
}

template <>
ConditionSet ModifiesSolver::solve_right<AssignmentAst>(AssignmentAst *ast) {
    ConditionSet result;
    result.insert(new SimpleVariableCondition(
            *(ast->get_variable())));
    return result;
}

template <>
ConditionSet ModifiesSolver::solve_right<CallAst>(CallAst *ast) {
    return solve_right<ProcAst>(ast->get_proc_called());
}

/*
 * index_variable()
 */

void ModifiesSolver::index_statement(StatementAst *statement, const SimpleVariable& variable) {
    _modifying_statement_index[variable].insert(statement);
    _modifying_condition_index[variable].insert(new SimpleStatementCondition(statement));
}

void ModifiesSolver::index_container_statement(StatementAst *statement, const VariableSet& variables) {
    for(auto it = variables.begin(); it != variables.end(); ++it) {
        index_statement(statement, *it);
    }
}

void ModifiesSolver::index_container_condition(ConditionPtr condition, const VariableSet& variables) {
    for(auto it = variables.begin(); it != variables.end(); ++it) {
        _modifying_condition_index[*it].insert(condition);
    }
}

VariableSet ModifiesSolver::index_statement_list(StatementAst *statement) {
    VariableSet result;

    while(statement != NULL) {
        VariableSet current_result = index_variables<StatementAst>(statement);
        union_set(result, current_result);
        statement = statement->next();
    }

    return result;
}

template <>
VariableSet ModifiesSolver::index_variables<ProcAst>(ProcAst *proc) {
    VariableSet result = index_statement_list(proc->get_statement());

    index_container_condition(new SimpleProcCondition(proc), result);

    return result;
}

template <>
VariableSet ModifiesSolver::index_variables<AssignmentAst>(AssignmentAst *assign) {
    SimpleVariable modified_var = *assign->get_variable();

    index_statement(assign, modified_var);

    VariableSet result;
    result.insert(modified_var);
    
    return result;
}

template <>
VariableSet ModifiesSolver::index_variables<WhileAst>(WhileAst *ast) {
    VariableSet result = index_statement_list(ast->get_body());

    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet ModifiesSolver::index_variables<IfAst>(IfAst *ast) {
    VariableSet result = index_statement_list(ast->get_then_branch());
    VariableSet else_result = index_statement_list(ast->get_else_branch());

    union_set(result, else_result);
    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet ModifiesSolver::index_variables<CallAst>(CallAst *ast) {
    VariableSet result = index_variables<ProcAst>(ast->get_proc_called());

    index_container_statement(ast, result);

    return result;
}

template <>
VariableSet ModifiesSolver::index_variables<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<ModifiesSolver,
        IndexVariableVisitorTraits<ModifiesSolver> > 
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

} // namespace impl
} // namespace simple
