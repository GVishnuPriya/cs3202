/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <iostream>
#include "simple/condition.h"
#include "simple/util/condition_visitor_generator.h"

namespace simple {
namespace util {

enum ConditionType {
    StatementCT,
    ProcCT,
    VariableCT,
    PatternCT,
    ConstantCT,
    OperatorCT,
};

ConditionType get_condition_type(SimpleCondition *condition);

std::string condition_to_string(SimpleCondition *condition);

SimpleCondition* clone_condition(SimpleCondition *condition);

template <typename Condition>
Condition* condition_cast(SimpleCondition *condition);

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

template <>
bool is_same_condition<ConstantCondition, ConstantCondition>(
        ConstantCondition *condition1, ConstantCondition *condition2);

template <>
bool is_same_condition<OperatorCondition, OperatorCondition>(
        OperatorCondition *condition1, OperatorCondition *condition2);

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

template <>
bool is_less_than_condition<ConstantCondition, ConstantCondition>(
        ConstantCondition *condition1, ConstantCondition *condition2);


template <typename SourceCondition, typename TargetCondition>
struct ConditionCaster {
    static TargetCondition* condition_cast(SourceCondition *condition) {
        return NULL;
    }
};

template <typename Condition>
struct ConditionCaster<Condition, Condition> 
{
    static Condition* condition_cast(Condition *condition) {
        return condition;
    }
};

template <typename TargetCondition>
class ConditionCastVisitorTraits {
  public:
    typedef TargetCondition* ResultType;

    template <typename SourceCondition>
    static TargetCondition* visit(SimpleCondition *c, SourceCondition *condition) {
        return ConditionCaster<SourceCondition, TargetCondition>::
            condition_cast(condition);
    }
};

template <typename Condition>
Condition* condition_cast(SimpleCondition *condition) {
    ConditionVisitorGenerator<SimpleCondition, 
        ConditionCastVisitorTraits<Condition> > 
        visitor(condition);

    condition->accept_condition_visitor(&visitor);
    return visitor.return_result();
}


} // namespace util
} // namespace simple
