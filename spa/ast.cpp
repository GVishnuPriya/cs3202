
#include "spa/ast.h"
#include "simple/util/statement_visitor_generator.h"

namespace spa {

using namespace simple;
using namespace simple::util;

AST::AST(SimpleRoot ast, StatementTable *statement_table) :
    _ast(ast), _statement_table(statement_table)
{
    for(ProcAst *proc : _ast) {
        _procs.insert(proc->get_name());
    }
}

StatementAst* AST::get_statement(StatementLine line) {
    return _statement_table->get_statement(line);
}

ProcAst* AST::get_proc(Proc proc_name) {
    ProcAst *proc = _ast.get_proc(proc_name);

    if(!proc) throw std::runtime_error("Procedure not found");

    return proc;
}

StatementLine AST::get_proc_body(Proc proc_name) {
    return get_proc(proc_name)->get_statement()->get_statement_line();
}

ProcResults AST::get_all_procs() {
    return _procs;
}

Proc AST::get_proc_from_statement(StatementLine statement) {
    return get_statement(statement)->get_proc()->get_name();
}

StatementLine AST::get_parent(StatementLine statement) {
    return get_statement(statement)->get_parent()->get_statement_line();
}

StatementType AST::get_statement_type(StatementAst *statement) {
    return simple::util::get_statement_type(statement);
}

bool AST::is_container_statement(StatementAst *statement) {
    StatementType type = get_statement_type(statement);
    return type == IfST || type == WhileST;
}

StatementType AST::get_statement_type(StatementLine line) {
    return get_statement_type(get_statement(line));
}

bool AST::is_container_statement(StatementLine statement) {
    return is_container_statement(get_statement(statement));
}


StatementLine AST::get_then_branch(StatementLine statement) {
    IfAst *ast = statement_cast<IfAst>(get_statement(statement));
    if(!ast) throw new std::runtime_error("Statement is not If statement");

    return ast->get_then_branch()->get_statement_line();
}

StatementLine AST::get_else_branch(StatementLine statement) {
    IfAst *ast = statement_cast<IfAst>(get_statement(statement));
    if(!ast) throw new std::runtime_error("Statement is not If statement");

    return ast->get_else_branch()->get_statement_line();
}

StatementLine AST::get_while_body(StatementLine statement) {
    WhileAst *ast = statement_cast<WhileAst>(get_statement(statement));
    if(!ast) throw new std::runtime_error("Statement is not While statement");

    return ast->get_body()->get_statement_line();
}

SimpleRoot AST::get_root() {
    return _ast;
}


}