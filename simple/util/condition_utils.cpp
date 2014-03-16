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

#include <sstream>
#include "simple/util/expr.h"
#include "simple/util/ast_utils.h"
#include "simple/util/condition_utils.h"

namespace simple {
namespace util {

class ConditionTypeVisitor : public ConditionVisitor {
  public:
    ConditionTypeVisitor() { }

    void visit_statement_condition(StatementCondition*) {
        result = StatementCT;
    }

    void visit_proc_condition(ProcCondition*) {
        result = ProcCT;
    }

    void visit_variable_condition(VariableCondition*) {
        result = VariableCT;
    }

    void visit_constant_condition(ConstantCondition*) {
        result = ConstantCT;
    }

    void visit_pattern_condition(PatternCondition*) {
        result = PatternCT;
    }

    ConditionType result;
};

ConditionType get_condition_type(SimpleCondition *condition) {
    ConditionTypeVisitor visitor;

    condition->accept_condition_visitor(&visitor);
    return visitor.result;
}

class SameConditionVisitorTraits {
  public:
    typedef bool ResultType;

    template <typename Condition1, typename Condition2>
    static bool visit_condition(Condition1 *condition1, Condition2 *condition2) {
        return is_same_condition<Condition1, Condition2>(condition1, condition2);
    }
};

class LessThanConditionVisitorTraits {
  public:
    typedef bool ResultType;

    template <typename Condition1, typename Condition2>
    static bool visit_condition(Condition1 *condition1, Condition2 *condition2) {
        return is_less_than_condition<Condition1, Condition2>(condition1, condition2);
    }
};

class ConditionPrinter : public ConditionVisitor {
  public:
    ConditionPrinter(SimpleCondition *condition) :
        _result()
    {
        condition->accept_condition_visitor(this);
    }

    void visit_proc_condition(ProcCondition *condition) {
        _result = condition->get_proc_ast()->get_name();
    }

    void visit_statement_condition(StatementCondition *condition) {
        std::stringstream out;
        out << condition->get_statement_ast()->get_statement_line();
        _result = out.str();
    }

    void visit_variable_condition(VariableCondition *condition) {
        _result += condition->get_variable()->get_name();
    }

    void visit_constant_condition(ConstantCondition *condition) {
        std::stringstream out;
        out << condition->get_constant()->get_int();
        _result = out.str();
    }

    void visit_pattern_condition(PatternCondition *condition) {
        _result = expr_to_string(condition->get_expr_ast());
    }

    std::string return_result() {
        return std::move(_result);
    }
  private:
    std::string _result;
};


std::string condition_to_string(SimpleCondition *condition) {
    ConditionPrinter printer(condition);
    return printer.return_result();
}

template <>
bool is_same_condition<SimpleCondition, SimpleCondition>(
        SimpleCondition *condition1, SimpleCondition *condition2) 
{
    FirstConditionVisitorGenerator<SameConditionVisitorTraits> 
    visitor(condition2);
    
    condition1->accept_condition_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool is_same_condition<StatementCondition, StatementCondition>(
        StatementCondition *condition1, StatementCondition *condition2)
{
    return condition1->get_statement_ast() == condition2->get_statement_ast();
}

template <>
bool is_same_condition<VariableCondition, VariableCondition>(
        VariableCondition *condition1, VariableCondition *condition2)
{
    return condition1->get_variable()->get_name() ==
        condition2->get_variable()->get_name();
}

template <>
bool is_same_condition<ProcCondition, ProcCondition>(
        ProcCondition *condition1, ProcCondition *condition2)
{
    return condition1->get_proc_ast() == condition2->get_proc_ast();
}

template <>
bool is_same_condition<PatternCondition, PatternCondition>(
        PatternCondition *condition1, PatternCondition *condition2)
{
    return same_expr(condition1->get_expr_ast(), condition2->get_expr_ast());
}


template <>
bool is_less_than_condition<SimpleCondition, SimpleCondition>(
        SimpleCondition *condition1, SimpleCondition *condition2)
{
    FirstConditionVisitorGenerator<LessThanConditionVisitorTraits> 
    visitor(condition2);

    condition1->accept_condition_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool is_less_than_condition<StatementCondition, ProcCondition>(
        StatementCondition *condition1, ProcCondition *condition2)
{
    return true;
}

template <>
bool is_less_than_condition<VariableCondition, StatementCondition>(
        VariableCondition *condition1, StatementCondition *condition2)
{
    return true;
}

template <>
bool is_less_than_condition<VariableCondition, ProcCondition>(
        VariableCondition *condition1, ProcCondition *condition2)
{
    return true;
}

template <>
bool is_less_than_condition<PatternCondition, VariableCondition>(
        PatternCondition *condition1, VariableCondition *condition2)
{
    return true;
}

template <>
bool is_less_than_condition<PatternCondition, StatementCondition>(
        PatternCondition *condition1, StatementCondition *condition2)
{
    return true;
}


template <>
bool is_less_than_condition<PatternCondition, ProcCondition>(
        PatternCondition *condition1, ProcCondition *condition2)
{
    return true;
}


template <>
bool is_less_than_condition<StatementCondition, StatementCondition>(
        StatementCondition *condition1, StatementCondition *condition2)
{
    return condition1->get_statement_ast() < condition2->get_statement_ast();
}

template <>
bool is_less_than_condition<VariableCondition, VariableCondition>(
        VariableCondition *condition1, VariableCondition *condition2)
{
    return  condition1->get_variable()->get_name() < 
            condition2->get_variable()->get_name();
}

template <>
bool is_less_than_condition<ProcCondition, ProcCondition>(
        ProcCondition *condition1, ProcCondition *condition2)
{
    return condition1->get_proc_ast() < condition2->get_proc_ast();
}

template <>
bool is_less_than_condition<PatternCondition, PatternCondition>(
        PatternCondition *condition1, PatternCondition *condition2)
{
    return expr_to_string(condition1->get_expr_ast()) < expr_to_string(condition2->get_expr_ast());
}


} // namespace util
} // namespace simple
