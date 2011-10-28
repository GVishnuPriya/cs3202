
#pragma once

#include "simple/ast.h"

namespace simple {
namespace util {

using namespace simple;

template <typename Ast>
bool is_statement_type(StatementAst *ast) {
    return false; // stub
}

template <typename Ast1, typename Ast2>
bool is_same_expr(Ast1 *ast1, Ast2 *ast2) {
    return false;
}

template <typename Ast1, typename Ast2>
bool is_less_than_expr(Ast1 *ast1, Ast2 *ast2) {
    return false;
}

template <typename Ast, typename Proc>
void set_proc(Ast *ast, Proc *proc) {
    ast->set_proc(proc);
    proc->set_first_statement(ast);
}

template <typename Ast1, typename Ast2>
void set_next(Ast1 *ast1, Ast2 *ast2) {
    ast1->set_next(ast2);
    ast2->set_prev(ast1);
    ast2->set_proc(ast1->get_proc());
    ast2->set_container(ast1->get_parent());
}

template <typename Ast, typename While>
void set_while_body(Ast *ast, While *loop) {
    loop->set_body(ast);
    set_parent(ast, loop);
}

template <typename Ast, typename Conditional>
void set_then_branch(Ast *ast, Conditional *condition) {
    condition->set_then_branch(ast);
    set_parent(ast, condition);
}

template <typename Ast, typename ConditionalASt>
void set_else_branch(Ast *ast, ConditionalASt *condition) {
    condition->set_else_branch(ast);
    set_parent(ast, condition);
}

template <typename Ast>
void set_parent(Ast *ast, ContainerAst *parent) {
    ast->set_container(parent);
    ast->set_proc(parent->get_proc());
}

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
