
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
bool is_less_than_condition<VariableCondition, ProcCondition>(
        VariableCondition *condition1, ProcCondition *condition2);

template <>
bool is_less_than_condition<PatternCondition, VariableCondition>(
        PatternCondition *condition1, VariableCondition *condition2);

template <>
bool is_less_than_condition<PatternCondition, StatementCondition>(
        PatternCondition *condition1, StatementCondition *condition2);

template <>
bool is_less_than_condition<PatternCondition, ProcCondition>(
        PatternCondition *condition1, ProcCondition *condition2);

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

} // namespace util
} // namespace simple
