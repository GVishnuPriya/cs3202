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

#include "impl/solvers/iexpr.h"
#include "impl/solvers/expr.h"
#include "simple/util/ast_utils.h"
#include "simple/util/condition_utils.h"
#include "simple/util/expr.h"
#include "simple/util/set_utils.h"
#include "simple/util/set_convert.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

ExprSet get_sub_exprs(ExprAst *expr) {
    ExprSet results;
    results.insert(expr);

    if(get_expr_type(expr)==BinaryOpET) {
        BinaryOpAst *bin = expr_cast<BinaryOpAst>(expr);

        union_set(results, get_sub_exprs(bin->get_lhs()));
        union_set(results, get_sub_exprs(bin->get_rhs()));
    }
    
    return results;
}

bool is_sub_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2);

bool is_sub_expr(ExprAst *expr1, ExprAst *expr2) {
    switch (get_expr_type(expr1)) {
        case BinaryOpET:
            return is_sub_expr_bin_op(expr_cast<BinaryOpAst>(expr1),expr2);
        case VariableET:
            return same_expr_var(expr_cast<VariableAst>(expr1),expr2);
        case ConstantET:
            return same_expr_const(expr_cast<ConstAst>(expr1),expr2);
        default:
            return false;
    }
}

bool is_sub_expr_bin_op(BinaryOpAst *expr1, ExprAst *expr2) {
    if (same_expr_bin_op(expr1,expr2)) return true;

    return is_sub_expr(expr1->get_lhs(), expr2) || is_sub_expr(expr1->get_rhs(), expr2);
}

IExprSolver::IExprSolver(SimpleRoot ast) : _ast(ast) {
    for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
        index_proc(*it);
    }
}

template <>
ConditionSet IExprSolver::solve_right<StatementAst>(StatementAst *statement) {
    ExprSet expr_result = solve_right_statement_expr(statement);
    ConditionSet result;

    for(auto it=expr_result.begin(); it != expr_result.end(); ++it) {
        result.insert(new SimplePatternCondition(clone_expr(*it)));
    }

    return result;
}

template <>
ConditionSet IExprSolver::solve_left<ExprAst>(ExprAst *expr) {
    StatementSet result = solve_left_statement(expr);
    return statement_set_to_condition_set(result);
}

template <>
bool IExprSolver::validate<StatementAst, ExprAst>(
        StatementAst *statement, ExprAst *expr)
{
    return validate_statement_expr(statement, expr);
}

bool IExprSolver::validate_statement_expr(StatementAst *statement, ExprAst *pattern) {
    AssignmentAst *assign = statement_cast<AssignmentAst>(statement);
    
    if(assign) {
        return validate_assign_expr(assign, pattern);
    } else {
        return false;
    }
}

bool IExprSolver::validate_assign_expr(AssignmentAst *assign_ast, ExprAst *pattern) {
    return is_sub_expr(assign_ast->get_expr(), pattern);
}

ExprSet IExprSolver::solve_right_statement_expr(StatementAst *statement) {
    AssignmentAst *assign = statement_cast<AssignmentAst>(statement);

    if(assign) {
        return solve_right_assign_expr(assign);
    } else {
        return ExprSet();
    }
}

ExprSet IExprSolver::solve_right_assign_expr(AssignmentAst *assign_ast) {
    return get_sub_exprs(assign_ast->get_expr());
}

StatementSet IExprSolver::solve_left_statement(ExprAst *pattern) {
    std::string key = expr_to_string(pattern);

    return _pattern_index[key];
}

void IExprSolver::index_proc(ProcAst *proc) {
    index_statement_list(proc->get_statement());
}

void IExprSolver::index_statement_list(StatementAst *statement) {
    while(statement!=NULL) {
        index_statement(statement);
        statement = statement->next();
    }
}

void IExprSolver::index_statement(StatementAst *statement) {
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

void IExprSolver::index_while(WhileAst *while_ast) {
    index_statement_list(while_ast->get_body());
}

void IExprSolver::index_if(IfAst *if_ast) {
    index_statement_list(if_ast->get_then_branch());
    index_statement_list(if_ast->get_else_branch());
}

void IExprSolver::index_assign(AssignmentAst *assign_ast) {
    ExprSet sub_exprs = get_sub_exprs(assign_ast->get_expr());
    
    for (auto it = sub_exprs.begin(); it != sub_exprs.end(); ++it) {
        std::string key = expr_to_string(*it);
        _pattern_index[key].insert(assign_ast);
    }
}

}
}
