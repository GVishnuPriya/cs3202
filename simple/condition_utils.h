
#pragma once

#include "simple/condition.h"

namespace simple {
namespace util {


template <typename Condition1, typename Condition2>
bool is_same_condition(Condition1 *condition1, Condition2 *condition2);

class FirstSameConditionVisitor : public ConditionVisitor {
  public:
    FirstSameConditionVisitor(SimpleCondition *condition2);

    void visit_statement_condition(StatementCondition *condition1);
    void visit_proc_condition(ProcCondition *condition1);
    void visit_variable_condition(VariableCondition *condition1);
    void visit_pattern_condition(PatternCondition *condition1);

    bool return_result();

  private:
    bool _result;
    SimpleCondition *_condition2;
};

template <typename FirstCondition>
class SecondSameConditionVisitor : public ConditionVisitor {
  public:
    SecondSameConditionVisitor(FirstCondition *condition1);

    void visit_statement_condition(StatementCondition *condition2);
    void visit_proc_condition(ProcCondition *condition2);
    void visit_variable_condition(VariableCondition *condition2);
    void visit_pattern_condition(PatternCondition *condition2);

    bool return_result();

  private:
    bool _result;
    FirstCondition *_condition1;
};

} // namespace util
} // namespace simple
