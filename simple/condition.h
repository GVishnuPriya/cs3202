
#pragma once

#include <list>
#include <memory>
#include "simple/ast.h"

namespace simple {

class ConditionVisitor;

class SimpleCondition {
  public:
    virtual void accept_condition_visitor(ConditionVisitor *visitor) = 0;
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
};


class ConditionPtr : public std::shared_ptr<SimpleCondition> {
  public:
    ConditionPtr(SimpleCondition *condition);
    ConditionPtr(const std::shared_ptr<SimpleCondition>& other);
    ConditionPtr(std::shared_ptr<SimpleCondition>&& other);
    ConditionPtr(const ConditionPtr& other);
    ConditionPtr(ConditionPtr&& other);

    bool equals(const ConditionPtr& other);
    bool less_than(const ConditionPtr& other);
    bool less_than_eq(const ConditionPtr& other);

    bool operator ==(const ConditionPtr& other);
    bool operator <(const ConditionPtr& other);
    bool operator <=(const ConditionPtr& other);

    ~ConditionPtr();
};

class ConditionSet {
  public:
    ConditionSet();
    ConditionSet(const ConditionSet& other);
    ConditionSet(ConditionSet&& other);

    void insert(std::shared_ptr<SimpleCondition> condition);
    void insert(SimpleCondition *condition);
    void union_with(const ConditionSet& other);
    void intersect_with(const ConditionSet& other);

    bool is_empty() const;
    bool equals(const ConditionSet& other) const;

    ~ConditionSet();

  private:
    std::list<ConditionPtr> _set;
};

} // namespace simple
