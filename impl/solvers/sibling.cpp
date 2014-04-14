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
#include "impl/solvers/sibling.h"
#include "simple/util/statement_visitor_generator.h"
#include "simple/util/ast_utils.h"
#include "simple/util/set_convert.h"
#include "simple/util/condition_utils.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace std;

SiblingSolver::SiblingSolver(SimpleRoot ast) : _ast(ast) {
    for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
        ProcAst *proc = *it;

        index_proc(proc);
    }
}

bool SiblingSolver::validate(
    SimpleCondition *left_condition, SimpleCondition *right_condition)
{
    ConditionPtr left(clone_condition(right_condition));
    ConditionPtr right(clone_condition(left_condition));

    return _sibling_index[left].has_element(right);
}

ConditionSet SiblingSolver::solve_left(SimpleCondition *right_condition) {
    ConditionPtr right(clone_condition(right_condition));

    return _sibling_index[right];
}

ConditionSet SiblingSolver::solve_right(SimpleCondition *left_condition) {
    ConditionPtr left(clone_condition(left_condition));

    return _sibling_index[left];
}

void SiblingSolver::index_siblings(
    ConditionPtr condition1, ConditionPtr condition2) 
{
    _sibling_index[condition1].insert(condition2);
    _sibling_index[condition2].insert(condition1);
}

void SiblingSolver::index_proc(ProcAst *proc) {
    ConditionPtr condition(new SimpleProcCondition(proc));
    ConditionSet& siblings = _sibling_index[condition];

    for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
        ProcAst *sibling_proc = *it;

        if(sibling_proc == proc) continue;

        ConditionPtr sibling_condition(new SimpleProcCondition(sibling_proc));
        siblings.insert(sibling_condition);
    }

    index_statement_list(proc->get_statement());
}

void SiblingSolver::index_statement_list(StatementAst *statement_list) {
    StatementAst *current_statement = statement_list;

    while(current_statement != NULL) {
        index_statement(current_statement);

        ConditionPtr condition(new SimpleStatementCondition(current_statement));
        ConditionSet& siblings = _sibling_index[condition];

        StatementAst *sibling_statement = statement_list;

        while(sibling_statement != NULL) {
            if(sibling_statement != current_statement) {
                ConditionPtr sibling_condition(
                    new SimpleStatementCondition(sibling_statement));

                siblings.insert(sibling_condition);
            }

            sibling_statement = sibling_statement->next();
        }

        current_statement = current_statement->next();
    }
}

void SiblingSolver::index_statement(StatementAst *statement) {
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

void SiblingSolver::index_while(WhileAst *while_ast) {
    ConditionPtr var_condition(new SimpleVariableCondition(
        *while_ast->get_variable()));

    ConditionPtr body_condition(new SimpleStatementCondition(
        while_ast->get_body()));

    index_siblings(var_condition, body_condition);
    index_statement_list(while_ast->get_body());
}

void SiblingSolver::index_if(IfAst *if_ast) {
    StatementAst *then_branch = if_ast->get_then_branch();
    StatementAst *else_branch = if_ast->get_else_branch();

    ConditionPtr var_condition(new SimpleVariableCondition(
        *if_ast->get_variable()));

    ConditionPtr then_condition(new SimpleStatementCondition(
        then_branch));

    ConditionPtr else_condition(new SimpleStatementCondition(
        else_branch));

    index_siblings(var_condition, then_condition);
    index_siblings(var_condition, else_condition);
    index_siblings(then_condition, else_condition);

    index_statement_list(then_branch);
    index_statement_list(else_branch);
}

void SiblingSolver::index_assign(AssignmentAst *assign) {
    ConditionPtr var_condition(new SimpleVariableCondition(
        *assign->get_variable()));

    ConditionPtr sibling_condition = index_expr(assign->get_expr());
    index_siblings(var_condition, sibling_condition);
}

ConditionPtr SiblingSolver::index_expr(ExprAst* expr) {
    switch(get_expr_type(expr)) {
        case BinaryOpET:
            return index_binary_op_expr(expr_cast<BinaryOpAst>(expr));
        break;
        case VariableET:
            return new SimpleVariableCondition(
                *expr_cast<VariableAst>(expr)->get_variable());
        break;
        case ConstantET:
            return new SimpleConstantCondition(
                *expr_cast<ConstAst>(expr)->get_constant());
        break;
        default:
            return NULL;
        break;
    }
}

ConditionPtr SiblingSolver::index_binary_op_expr(BinaryOpAst* expr) {
    ConditionPtr op_condition(new SimpleOperatorCondition(expr->get_op()));

    ConditionPtr left_condition(index_expr(expr->get_lhs()));
    ConditionPtr right_condition(index_expr(expr->get_rhs()));

    index_siblings(op_condition, left_condition);
    index_siblings(op_condition, right_condition);
    index_siblings(left_condition, right_condition);

    return op_condition;
}

}
}