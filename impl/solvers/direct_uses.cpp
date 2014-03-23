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

#include "impl/solvers/direct_uses.h"
#include "simple/util/ast_utils.h"
#include "simple/util/set_convert.h"
#include "simple/util/condition_utils.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

DirectUsesSolver::DirectUsesSolver(SimpleRoot ast) : _ast(ast) { 
    for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
        index_proc(*it);
    }
}

ConditionSet DirectUsesSolver::solve_left(SimpleCondition *right_condition) {
    VariableCondition *condition = condition_cast<VariableCondition>(right_condition);
    if(!condition) return ConditionSet();

    SimpleVariable *var = condition->get_variable();
    return statement_set_to_condition_set(solve_left_statement(var));
}

ConditionSet DirectUsesSolver::solve_right(SimpleCondition *left_condition) {
    StatementCondition *condition = condition_cast<StatementCondition>(left_condition);
    if(!condition) return ConditionSet();

    StatementAst *statement = condition->get_statement_ast();
    return variable_set_to_condition_set(solve_right_var(statement));
}

bool DirectUsesSolver::validate(SimpleCondition *left_condition, SimpleCondition *right_condition) {
    StatementCondition *left = condition_cast<StatementCondition>(left_condition);
    VariableCondition *right = condition_cast<VariableCondition>(right_condition);

    if(!left || !right) return false;
    
    StatementAst *statement = left->get_statement_ast();
    SimpleVariable *var = right->get_variable();

    return validate_statement_var(statement, var);
}

bool DirectUsesSolver::validate_statement_var(StatementAst *statement, SimpleVariable *var) {
    switch(get_statement_type(statement)) {
        case WhileST:
            return (statement_cast<WhileAst>(statement)->get_variable() == var);
        case IfST:
            return (statement_cast<IfAst>(statement)->get_variable() == var);
        default:
            return false;
    }
}

StatementSet DirectUsesSolver::solve_left_statement(SimpleVariable *var) {
    return _right_statement_index[*var];
}

VariableSet DirectUsesSolver::solve_right_var(StatementAst *statement) {
    VariableSet result;

    switch(get_statement_type(statement)) {
        case WhileST:
            result.insert(*statement_cast<WhileAst>(statement)->get_variable());
        break;
        case IfST:
            result.insert(*statement_cast<IfAst>(statement)->get_variable());
        break;
        case AssignST:
            result.insert(*statement_cast<AssignmentAst>(statement)->get_variable());
        break;
        default:
            // noop
        break;
    }

    return result;
}

void DirectUsesSolver::index_proc(ProcAst *proc) {
    index_statement_list(proc->get_statement());
}

void DirectUsesSolver::index_statement_list(StatementAst *statement) {
    while(statement!=NULL) {
        index_statement(statement);
        statement = statement->next();
    }
}

void DirectUsesSolver::index_statement(StatementAst *statement) {
    switch(get_statement_type(statement)) {
        case WhileST:
            index_while(statement_cast<WhileAst>(statement));
        break;
        case IfST:
            index_if(statement_cast<IfAst>(statement));
        break;
        case AssignST:
            index_assign(statement_cast<AssignmentAst>(statement));
        break;
        default:
        	// noop
        break;
    }
}

void DirectUsesSolver::index_while(WhileAst *while_ast) {
    _right_statement_index[*while_ast->get_variable()].insert(while_ast);
    index_statement_list(while_ast->get_body());
}

void DirectUsesSolver::index_if(IfAst *if_ast) {
    _right_statement_index[*if_ast->get_variable()].insert(if_ast);

    index_statement_list(if_ast->get_then_branch());
    index_statement_list(if_ast->get_else_branch());
}

void DirectUsesSolver::index_assign(AssignmentAst *assign) {
    _right_statement_index[*assign->get_variable()].insert(assign);
}

}
}