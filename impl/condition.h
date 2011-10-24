
#pragma once

#include "simple/condition.h"

namespace simple {
namespace impl {

using namespace simple::ast;
using namespace simple::condition;

class SimpleStatementCondition : public StatementCondition {
  public:
    SimpleStatementCondition(StatementAst *ast) : _ast(ast) { }

    StatementAst* get_statement_ast() {
        return _ast;
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_statement_condition(this);
    }

    ~SimpleStatementCondition() { }

  private:
    StatementAst *_ast;
};

class SimpleProcCondition : public ProcCondition {
  public:
    SimpleProcCondition(ProcAst *ast) : _ast(ast) { }

    ProcAst* get_proc_ast() {
        return _ast;
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_proc_condition(this);
    }

    ~SimpleProcCondition() { }

  private:
    ProcAst *_ast;
};

class SimpleVariableCondition : public VariableCondition {
  public:
    SimpleVariableCondition(const SimpleVariable& var) : 
        _var(var) 
    { }

    SimpleVariableCondition(const std::string name) : 
        _var(name) 
    { }

    SimpleVariable* get_variable() {
        return &_var;
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_variable_condition(this);
    }

    ~SimpleVariableCondition() { }
  private:
    SimpleVariable _var;
};

class SimplePatternCondition : public PatternCondition {
  public:
    SimplePatternCondition(ExprAst *pattern) : _pattern(pattern) { }

    ExprAst* get_expr_ast() {
        return _pattern.get();
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_pattern_condition(this);
    }

    ~SimplePatternCondition() { }
  private:
    std::unique_ptr<ExprAst> _pattern;
};

} // namespace impl
} // namespace simple
