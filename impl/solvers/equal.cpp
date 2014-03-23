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

#include "impl/solvers/equal.h"
#include "simple/util/ast_utils.h"
#include "simple/util/expr_util.h"
#include "simple/util/condition_utils.h"

namespace simple {
namespace impl {

EqualSolver::EqualSolver(SimpleRoot ast) : _ast(ast)
{
    for(auto it=_ast.begin(); it!=_ast.end(); ++it) {
        index_proc(*it);
    }
}

void EqualSolver::index_proc(ProcAst *proc) {
    _name_index[proc->get_name()].insert(new SimpleProcCondition(proc));

    index_statement_list(proc->get_statement());
}

void EqualSolver::index_statement_list(StatementAst *statement) {
    while(statement != NULL) {
        index_statement(statement);
        statement = statement->next();
    }
}

void EqualSolver::index_statement(StatementAst *statement) {
    _number_index[statement->get_statement_line()].insert(
        new SimpleStatementCondition(statement));

    StatementType type = get_statement_type(statement);

    switch(type) {
        case AssignST:
            index_assign(statement_cast<AssignmentAst>(statement));
        break;
        case WhileST:
            index_while(statement_cast<WhileAst>(statement));
        break;
        case IfST:
            index_if(statement_cast<IfAst>(statement));
        break;
        case CallST:
            // noop
        break;
    }
}

void EqualSolver::index_if(IfAst *ast) {
    index_variable(ast->get_variable());

    index_statement_list(ast->get_then_branch());
    index_statement_list(ast->get_else_branch());
}

void EqualSolver::index_while(WhileAst *ast) {
    index_variable(ast->get_variable());

    index_statement_list(ast->get_body());
}

void EqualSolver::index_variable(SimpleVariable *var) {
    _name_index[var->get_name()].insert(new SimpleVariableCondition(*var));
}

void EqualSolver::index_assign(AssignmentAst *assign) {
    index_variable(assign->get_variable());

    index_expr(assign->get_expr());
}

void EqualSolver::index_expr(ExprAst *expr) {
    ExprType type = get_expr_type(expr);

    switch(type) {
        case BinaryOpET:
            index_binary_op_expr(expr_cast<BinaryOpAst>(expr));
        break;
        case VariableET:
            index_var_expr(expr_cast<VariableAst>(expr));
        break;
        case ConstantET:
            index_const_expr(expr_cast<ConstAst>(expr));
        break;
    }
}

void EqualSolver::index_binary_op_expr(BinaryOpAst *ast) {
    index_expr(ast->get_lhs());
    index_expr(ast->get_rhs());
}

void EqualSolver::index_var_expr(VariableAst *ast) {
    index_variable(ast->get_variable());
}

void EqualSolver::index_const_expr(ConstAst *ast) {
    SimpleConstant *constant = ast->get_constant();

    _number_index[constant->get_int()].insert(
        new SimpleConstantCondition(*constant));
}

template <>
ConditionSet EqualSolver::solve_equal<ProcAst>(ProcAst *proc)
{
    return _name_index[proc->get_name()];
}

template <>
ConditionSet EqualSolver::solve_equal<SimpleVariable>(SimpleVariable *var)
{
    return _name_index[var->get_name()];
}

template <>
ConditionSet EqualSolver::solve_equal<StatementAst>(StatementAst *statement)
{
    return _number_index[statement->get_statement_line()];
}

template <>
ConditionSet EqualSolver::solve_equal<SimpleConstant>(SimpleConstant *constant)
{
    return _number_index[constant->get_int()];
}

template <>
bool EqualSolver::validate<SimpleVariable, SimpleVariable>(
    SimpleVariable *var1, SimpleVariable *var2)
{
    return var1->get_name() == var2->get_name();
}

template <>
bool EqualSolver::validate<ProcAst, ProcAst>(
    ProcAst *proc1, ProcAst *proc2)
{
    return proc1->get_name() == proc2->get_name();
}

template <>
bool EqualSolver::validate<StatementAst, StatementAst>(
    StatementAst *statement1, StatementAst *statement2)
{
    return statement1->get_statement_line() == statement2->get_statement_line();
}

template <>
bool EqualSolver::validate<SimpleConstant, SimpleConstant>(
    SimpleConstant *constant1, SimpleConstant *constant2)
{
    return constant1->get_int() == constant2->get_int();
}

template <>
bool EqualSolver::validate<SimpleVariable, ProcAst>(
    SimpleVariable *var1, ProcAst *proc2)
{
    return var1->get_name() == proc2->get_name();
}

template <>
bool EqualSolver::validate<ProcAst, SimpleVariable>(
    ProcAst *proc1, SimpleVariable *var2)
{
    return proc1->get_name() == var2->get_name();
}

template <>
bool EqualSolver::validate<StatementAst, SimpleConstant>(
    StatementAst *statement1, SimpleConstant *constant2)
{
    return statement1->get_statement_line() == constant2->get_int();
}

template <>
bool EqualSolver::validate<SimpleConstant, StatementAst>(
    SimpleConstant *constant1, StatementAst *statement2)
{
    return constant1->get_int() == statement2->get_statement_line();
}

}
}