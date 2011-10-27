
#pragma once

#include "simple/condition.h"

namespace simple {
namespace util {


template <typename Condition1, typename Condition2>
bool is_same_condition(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <>
bool is_same_condition<SimpleCondition, SimpleCondition>(
        SimpleCondition *condition1, SimpleCondition *condition2);

template <>
bool is_same_condition<StatementCondition, StatementCondition>(
        StatementCondition *condition1, StatementCondition *condition2);

template <>
bool is_same_condition<VariableCondition, VariableCondition>(
        VariableCondition *condition1, VariableCondition *condition2);

template <>
bool is_same_condition<ProcCondition, ProcCondition>(
        ProcCondition *condition1, ProcCondition *condition2);

template <>
bool is_same_condition<PatternCondition, PatternCondition>(
        PatternCondition *condition1, PatternCondition *condition2);

/*
 * Condition Ordering:
 * Proc > Statement > Variable > Pattern
 */
template <typename Condition1, typename Condition2>
bool is_less_than_condition(
        Condition1 *condition1, Condition2 *condition2)
{
    return false;
}

template <>
bool is_less_than_condition<SimpleCondition, SimpleCondition>(
        SimpleCondition *condition1, SimpleCondition *condition2);

template <>
bool is_less_than_condition<StatementCondition, ProcCondition>(
        StatementCondition *condition1, ProcCondition *condition2);

template <>
bool is_less_than_condition<VariableCondition, StatementCondition>(
        VariableCondition *condition1, StatementCondition *condition2);

template <>
bool is_less_than_condition<PatternCondition, VariableCondition>(
        PatternCondition *condition1, VariableCondition *condition2);

template <>
bool is_less_than_condition<StatementCondition, StatementCondition>(
        StatementCondition *condition1, StatementCondition *condition2);

template <>
bool is_less_than_condition<VariableCondition, VariableCondition>(
        VariableCondition *condition1, VariableCondition *condition2);

template <>
bool is_less_than_condition<ProcCondition, ProcCondition>(
        ProcCondition *condition1, ProcCondition *condition2);

template <>
bool is_less_than_condition<PatternCondition, PatternCondition>(
        PatternCondition *condition1, PatternCondition *condition2);



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
