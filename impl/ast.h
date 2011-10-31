
#pragma once

#include "simple/ast.h"

namespace simple {
namespace impl {

using namespace simple;

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

class SimpleStatementAst {
  public:
    virtual void set_line(unsigned int line) = 0;
    virtual void set_proc(ProcAst *proc) = 0;
    virtual void set_container(ContainerAst *container) = 0;
    virtual void set_next(StatementAst *next) = 0;
    virtual void set_prev(StatementAst *prev) = 0;

    virtual StatementAst* as_ast() = 0;

    virtual ~SimpleStatementAst() { }
};

/*
 * Abstract Class
 */
template <typename ParentType>
class SimpleStatementBase : public ParentType, public SimpleStatementAst {
  public:
    SimpleStatementBase() :
        _line(0), _proc(0), _container(0),
        _next(nullptr), _prev(0)
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

    StatementAst* as_ast() {
        return this;
    }

    virtual ~SimpleStatementBase() { }

  protected:
    unsigned int _line;
    ProcAst *_proc;
    ContainerAst *_container;
    StatementAst *_prev;

    // StatementAst owns the next statement after itself.
    std::unique_ptr<StatementAst> _next;
};

class SimpleConditionalAst : public SimpleStatementBase<ConditionalAst> {
  public:
    SimpleConditionalAst() :
        SimpleStatementBase<ConditionalAst>(),
        _then_branch(nullptr), _else_branch(nullptr), _var()
    { }

    void set_variable(const SimpleVariable& var) {
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

class SimpleWhileAst : public SimpleStatementBase<WhileAst> {
  public:
    SimpleWhileAst() : 
        SimpleStatementBase<WhileAst>(),
        _body(nullptr), _var() 
    { }

    void set_variable(const SimpleVariable& var) {
        _var = var;
    }

    void set_body(StatementAst *body) {
        _body.reset(body);
    }

    StatementAst* get_body() {
        return _body.get();
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_while(this);
    }

    void accept_container_visitor(ContainerVisitor *visitor) {
        visitor->visit_while(this);
    }

    ~SimpleWhileAst() { }

  private:
    std::unique_ptr<StatementAst>   _body;
    SimpleVariable                  _var;
};

class SimpleCallAst : public SimpleStatementBase<CallAst> {
  public:
    SimpleCallAst() : 
        SimpleStatementBase<CallAst>(), 
        _proc_called(0) 
    { }

    SimpleCallAst(ProcAst *proc) : 
        SimpleStatementBase<CallAst>(), 
        _proc_called(proc) 
    { }

    void set_proc_called(ProcAst *proc) {
        _proc_called = proc;
    }

    ProcAst* get_proc_called() {
        return _proc_called;
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_call(this);
    }

    ~SimpleCallAst() { }

  private:
    ProcAst *_proc_called;
};

class SimpleAssignmentAst : public SimpleStatementBase<AssignmentAst> {
  public:
    SimpleAssignmentAst() : 
        SimpleStatementBase<AssignmentAst>(),
        _var(), _expr(nullptr) 
    { }

    void set_variable(const SimpleVariable& var) {
        _var = var;
    }

    void set_expr(ExprAst *expr) {
        _expr.reset(expr);
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    ExprAst* get_expr() {
        return _expr.get();
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_assignment(this);
    }

    ~SimpleAssignmentAst() { }

  private:
    SimpleVariable            _var;
    std::unique_ptr<ExprAst>  _expr;
};

class SimpleVariableAst : public VariableAst {
  public:
    SimpleVariableAst() : _var() { }
    SimpleVariableAst(const SimpleVariable& var) : _var(var) { }
    SimpleVariableAst(const char *var) : _var(var) { }

    void set_variable(const SimpleVariable& var) {
        _var = var;
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    void accept_expr_visitor(ExprVisitor *visitor) {
        visitor->visit_variable(this);
    }

    ~SimpleVariableAst() { }

  private:
    SimpleVariable  _var;
};

class SimpleConstAst : public ConstAst {
  public:
    SimpleConstAst() : _value(0) { }
    SimpleConstAst(int value) : _value(value) { }

    void set_value(int value) {
        _value = value;
    }
    int get_value() {
        return _value;
    }
    void accept_expr_visitor(ExprVisitor *visitor) {
        visitor->visit_const(this);
    }

    ~SimpleConstAst() { }

  private:
    int          _value;
};

class SimpleBinaryOpAst : public BinaryOpAst {
  public:
    SimpleBinaryOpAst() : 
        _lhs(nullptr), _rhs(nullptr), _op(' ')
    { }

    SimpleBinaryOpAst(char op, ExprAst *lhs, ExprAst *rhs) :
        _lhs(lhs), _rhs(rhs), _op(op)
    { }

    void set_lhs(ExprAst *lhs) {
        _lhs.reset(lhs);
    }

    void set_rhs(ExprAst *rhs) {
        _rhs.reset(rhs);
    }

    void set_op(char op) {
        _op = op;
    }

    ExprAst* get_lhs() {
        return _lhs.get();
    }

    ExprAst* get_rhs() {
        return _rhs.get();
    }

    char get_op() {
        return _op;
    }

    void accept_expr_visitor(ExprVisitor *visitor) {
        visitor->visit_binary_op(this);
    }

    ~SimpleBinaryOpAst() { }
  private:
    std::unique_ptr<ExprAst>    _lhs;
    std::unique_ptr<ExprAst>    _rhs;
    char                        _op;
};

} // namespace impl
} // namespace simple
