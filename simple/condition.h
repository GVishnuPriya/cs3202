
#pragma once

#include "simple/ast.h"

namespace simple {

class ConditionVisitor;

class SimpleCondition {
  public:
    virtual void accept_condition_visitor(ConditionVisitor *visitor) = 0;
    virtual ~SimpleCondition() { }
};

class StatementCondition : public SimpleCondition {
  public:
    virtual StatementAst* get_statement_ast() = 0;

    virtual ~StatementCondition() { }
};

class ProcCondition : public SimpleCondition {
  public:
    virtual ProcAst* get_proc_ast() = 0;

    virtual ~ProcCondition() { }
};

class VariableCondition : public SimpleCondition {
  public:
    virtual SimpleVariable* get_variable() = 0;
    
    virtual ~VariableCondition() { }
};

class PatternCondition : public SimpleCondition {
  public:
    virtual ExprAst* get_expr_ast() = 0;

    virtual ~PatternCondition() { }
};

class ConditionVisitor {
  public:
    virtual void visit_statement_condition(StatementCondition*) = 0;
    virtual void visit_proc_condition(ProcCondition*) = 0;
    virtual void visit_variable_condition(VariableCondition*) = 0;
    virtual void visit_pattern_condition(PatternCondition*) = 0;

    virtual ~ConditionVisitor() { }
};


} // namespace simple
