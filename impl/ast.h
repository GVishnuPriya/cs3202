
#pragma once

#include "simple/ast.h"

namespace simple {
namespace impl {

using namespace simple::ast;

class SimpleProcAst : public ProcAst {
  public:
    SimpleProcAst(std::string name) : _name(name) { }

    void set_first_statement(StatementAst* statement) {
        if(_statement) { throw InconsistentAstError(); }
        _statement.reset(statement);
    }

    virtual std::string get_name() {
        return _name;
    }

    virtual StatementAst* get_statement() {
        return _statement.get();
    }

  private:
    std::string _name;

    // ProcAst owns the first statement ast.
    std::unique_ptr<StatementAst> _statement;
};

/*
 * Abstract Class
 */
template <typename ParentType>
class SimpleStatementAst : public ParentType {
  public:
    SimpleStatementAst() :
        _line(0), _proc(0), _container(0),
        _next(0), _prev(0)
    { }

    void set_line(unsigned int line) {
        _line = line;
    }

    void set_proc(ProcAst *proc) {
        _proc = proc;
    }

    void set_container(ContainerAst *container) {
        _container = container;
    }

    void set_next(StatementAst *statement) {
        _next.reset(statement);
    }

    void set_prev(StatementAst *statement) {
        _prev = statement;
    }

    unsigned int get_line() {
        return _line;
    }
    
    StatementAst* next() {
        return _next.get();
    }

    StatementAst* prev() {
        return _prev;
    }

    ContainerAst* get_parent() {
        return _container;
    }

    ProcAst* get_proc() {
        return _proc;
    }

    ~SimpleStatementAst() { }

  protected:
    unsigned int _line;
    ProcAst *_proc;
    ContainerAst *_container;
    StatementAst *_prev;

    // StatementAst owns the next statement after itself.
    std::unique_ptr<StatementAst> _next;
};

class SimpleConditionalAst : public SimpleStatementAst<ConditionalAst> {
  public:
    SimpleConditionalAst() :
        SimpleStatementAst<ConditionalAst>(),
        _then_branch(0), _else_branch(0), _var()
    { }

    void set_variable(SimpleVariable var) {
        _var = var;
    }

    void set_then_branch(StatementAst *then_branch) {
        _then_branch.reset(then_branch);
    }

    void set_else_branch(StatementAst *else_branch) {
        _else_branch.reset(else_branch);
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    StatementAst* get_then_branch() {
        return _then_branch.get();
    }

    StatementAst* get_else_branch() {
        return _else_branch.get();
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_conditional(this);
    }

    void accept_container_visitor(ContainerVisitor *visitor) {
        visitor->visit_conditional(this);
    }

    ~SimpleConditionalAst() { }

  private:
    std::unique_ptr<StatementAst>   _then_branch;
    std::unique_ptr<StatementAst>   _else_branch;
    SimpleVariable                  _var;
};


} // namespace impl
} // namespace simple
