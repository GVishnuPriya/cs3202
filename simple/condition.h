
#pragma once

#include "simple/expr.h"

class ConditionVisitor;

class SimpleCondition {
  public:
    virtual void accept_condition_visitor(ConditionVisitor *visitor) = 0;
};

class StatementCondition : public SimpleCondition {
  public:
    virtual StatementAst* get_statement_ast() = 0;
};

class ProcCondition : public SimpleCondition {
  public:
    virtual ProcAst* get_proc_ast() = 0;
};

class VariableCondition : public SimpleCondition {
  public:
    virtual SimpleVariable* get_variable() = 0;
};

class PatternCondition : public SimpleCondition {
  public:
    virtual ExprAst* get_expr_ast() = 0;
};

class ConditionVisitor {
  public:
    virtual visit_statement_condition(StatementCondition*) = 0;
    virtual visit_proc_condition(ProcCondition*) = 0;
    virtual visit_variable_condition(VariableCondition*) = 0;
    virtual visit_pattern_condition(PatternCondition*) = 0;
};


class ConditionSet {
  public:
    ConditionSet() : _set() { }
    ConditionSet(const ConditionSet& other) : _set(other.set) { }
    ConditionSet(ConditionSet&& other) : _set(std::move(other.set)) { }

    void insert(SimpleCondition *condition) {
        _set.insert(condition);
    }

    void insert(std::shared_ptr<SimpleCondition> condition) {
        _set.insert(condition);
    }

    void union_with(const ConditionSet& other) {

    }

    void intersect_with(const ConditionSet& other) {

    }

  private:
    std::set< std::shared_ptr<SimpleCondition> > _set;
}
