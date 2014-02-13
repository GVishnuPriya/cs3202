
#pragma once

#include <map>
#include <stdexcept>
#include "simple/ast.h"
#include "simple/util/ast_utils.h"
#include "spa/statement.h"

namespace spa {

using namespace simple;
using namespace simple::util;

enum StatementType {
    Assign,
    Call,
    While,
    If
};

class AST {
  public:
    AST(SimpleRoot ast, StatementTable *statement_table);

    StatementAst* get_statement(StatementLine line);

    ProcAst* get_proc(Proc proc_name);
    StatementLine get_proc_body(Proc proc_name);

    ProcResults get_all_procs();

    Proc get_proc_from_statement(StatementLine statement);

    StatementLine get_parent(StatementLine statement);

    StatementType get_statement_type(StatementAst *statement);
    StatementType get_statement_type(StatementLine line);

    bool is_container_statement(StatementAst *statement);
    bool is_container_statement(StatementLine statement);

    StatementLine get_then_branch(StatementLine statement);
    
    StatementLine get_else_branch(StatementLine statement);
    
    StatementLine get_while_body(StatementLine statement);

  private:
    SimpleRoot _ast;
    StatementTable *_statement_table;
    ProcResults _procs;
};

}