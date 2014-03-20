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

#include "impl/solvers/expr.h"
#include "simple/util/expr_util.h"
#include "simple/util/ast_utils.h"
#include "simple/util/set_convert.h"
#include "simple/util/condition_utils.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

ExprSolver::ExprSolver(SimpleRoot ast) : _ast(ast) {
    for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
        index_proc(*it);
    }
}

ConditionSet ExprSolver::solve_left(SimpleCondition *right_condition) {
    PatternCondition *condition = condition_cast<PatternCondition>(right_condition);
    if(!condition) return ConditionSet();

    ExprAst *expr = condition->get_expr_ast();
    StatementSet result = solve_left_statement(expr);
    return statement_set_to_condition_set(result);
}

ConditionSet ExprSolver::solve_right(SimpleCondition *left_condition) {
    StatementCondition *condition = condition_cast<StatementCondition>(left_condition);
    if(!condition) return ConditionSet();

    StatementAst *statement = condition->get_statement_ast();
    ExprSet result = solve_right_statement_expr(statement);
    return expr_set_to_condition_set(result);
}

bool ExprSolver::validate(SimpleCondition *left_condition, SimpleCondition *right_condition) {
    StatementCondition *left = condition_cast<StatementCondition>(left_condition);
    PatternCondition *right = condition_cast<PatternCondition>(right_condition);

    if(!left || !right) return false;
    
    StatementAst *statement = left->get_statement_ast();
    ExprAst *expr = right->get_expr_ast();

    return validate_statement_expr(statement, expr);
}

bool ExprSolver::validate_statement_expr(StatementAst *statement, ExprAst *pattern) {
    AssignmentAst *assign = statement_cast<AssignmentAst>(statement);
    
    if(assign) {
        return validate_assign_expr(assign, pattern);
    } else {
        return false;
    }
}

bool ExprSolver::validate_assign_expr(AssignmentAst *assign_ast, ExprAst *pattern) {
    return same_expr(assign_ast->get_expr(),pattern);
}

StatementSet ExprSolver::solve_left_statement(ExprAst *pattern) {
    std::string key = expr_to_string(pattern);
    return _pattern_index[key];
}

ExprSet ExprSolver::solve_right_statement_expr(StatementAst *statement) {
    AssignmentAst *assign = statement_cast<AssignmentAst>(statement);

    if(assign) {
        return solve_right_assign_expr(assign);
    } else {
        return ExprSet();
    }
}

ExprSet ExprSolver::solve_right_assign_expr(AssignmentAst *assign_ast) {
    ExprSet result;
    result.insert(assign_ast->get_expr());
    return result;
}

void ExprSolver::index_proc(ProcAst *proc) {
    index_statement_list(proc->get_statement());
}

void ExprSolver::index_statement_list(StatementAst *statement) {
    while(statement!=NULL) {
        index_statement(statement);
        statement = statement->next();
    }
}

void ExprSolver::index_statement(StatementAst *statement) {
    switch(get_statement_type(statement)) {
        case AssignST:
            index_assign(statement_cast<AssignmentAst>(statement));
            break;

        case WhileST:
            index_while(statement_cast<WhileAst>(statement));
            break;

        case IfST:
            index_if(statement_cast<IfAst>(statement));
            break;

        default:
            // noop
            break;
    }
}

void ExprSolver::index_while(WhileAst *while_ast) {
    index_statement_list(while_ast->get_body());
}

void ExprSolver::index_if(IfAst *if_ast) {
    index_statement_list(if_ast->get_then_branch());
    index_statement_list(if_ast->get_else_branch());
}

void ExprSolver::index_assign(AssignmentAst *assign_ast) {
    std::string key = expr_to_string(assign_ast->get_expr());
    _pattern_index[key].insert(assign_ast);
}

}
}
