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

#pragma once

#include "simple/ast.h"
#include "impl/ast.h"

namespace simple {
namespace util {

using namespace simple;
using namespace simple::impl;

template <typename Ast>
bool is_statement_type(StatementAst *ast) {
    return false; // stub
}

template <typename Ast1, typename Ast2>
bool is_same_statement(Ast1 *ast1, Ast2 *ast2) {
    return false;
}

template <typename Ast1, typename Ast2>
bool is_same_expr(Ast1 *ast1, Ast2 *ast2) {
    return false;
}

template <typename Ast1, typename Ast2>
bool is_less_than_expr(Ast1 *ast1, Ast2 *ast2) {
    return false;
}

inline void set_proc(SimpleStatementAst *ast, SimpleProcAst *proc) {
    ast->set_proc(proc);
    proc->set_first_statement(ast->as_ast());
}

inline void set_parent(SimpleStatementAst *ast, ContainerAst *parent) {
    ast->set_container(parent);
    ast->set_proc(parent->get_proc());
}

inline void set_next(SimpleStatementAst *ast1, SimpleStatementAst *ast2) {
    ast1->set_next(ast2->as_ast());
    ast2->set_prev(ast1->as_ast());
    ast2->set_proc(ast1->as_ast()->get_proc());
    ast2->set_container(ast1->as_ast()->get_parent());
}

inline void set_while_body(SimpleStatementAst *ast, SimpleWhileAst *loop) {
    loop->set_body(ast->as_ast());
    set_parent(ast, loop);
}

inline void set_then_branch(SimpleStatementAst *ast, SimpleIfAst *condition) {
    condition->set_then_branch(ast->as_ast());
    set_parent(ast, condition);
}

inline void set_else_branch(SimpleStatementAst *ast, SimpleIfAst *condition) {
    condition->set_else_branch(ast->as_ast());
    set_parent(ast, condition);
}

template <>
bool is_same_statement<StatementAst, StatementAst>(StatementAst *stat1, StatementAst *stat2);

template <>
bool is_same_statement<AssignmentAst, AssignmentAst>(AssignmentAst *assign1, AssignmentAst *assign2);

template <>
bool is_same_statement<CallAst, CallAst>(CallAst *call1, CallAst *call2);

template <>
bool is_same_statement<WhileAst, WhileAst>(WhileAst *loop1, WhileAst *loop2);

template <>
bool is_same_statement<IfAst, IfAst>(IfAst *condition1, IfAst *condition2);

bool is_same_statement_list(StatementAst *statement1, StatementAst *statement2);



template <>
bool is_same_expr<ExprAst, ExprAst>(ExprAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<VariableAst, ExprAst>(VariableAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<ConstAst, ExprAst>(ConstAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<BinaryOpAst, ExprAst>(BinaryOpAst *ast1, ExprAst *ast2);

template <>
bool is_same_expr<VariableAst, VariableAst>(VariableAst *ast1, VariableAst *ast2);

template <>
bool is_same_expr<ConstAst, ConstAst>(ConstAst *ast1, ConstAst *ast2);

template <>
bool is_same_expr<BinaryOpAst, BinaryOpAst>(BinaryOpAst *ast1, BinaryOpAst *ast2);



} // namespace util 
} // namespace simple
