
#include "simple/condition_utils.h"
#include "simple/ast_utils.h"

namespace simple {
namespace util {

template <>
bool is_same_condition<SimpleCondition, SimpleCondition>(
        SimpleCondition *condition1, SimpleCondition *condition2) 
{
    FirstSameConditionVisitor visitor(condition2);
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
    return condition1->get_variable()->equals(*condition2->get_variable());
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
    return is_same_expr(condition1->get_expr_ast(), condition2->get_expr_ast());
}


template <>
bool is_less_than_condition<SimpleCondition, SimpleCondition>(
        SimpleCondition *condition1, SimpleCondition *condition2)
{

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
bool is_less_than_condition<PatternCondition, VariableCondition>(
        PatternCondition *condition1, VariableCondition *condition2)
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
    return  condition1->get_variable()->get_name() > 
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
    return is_less_than_expr(condition1->get_expr_ast(), condition2->get_expr_ast());
}



FirstSameConditionVisitor::FirstSameConditionVisitor(SimpleCondition *condition2) : 
    _condition2(condition2) 
{ }

void FirstSameConditionVisitor::visit_statement_condition(
        StatementCondition *condition1) 
{
    SecondSameConditionVisitor<StatementCondition> visitor(condition1);
    _condition2->accept_condition_visitor(&visitor);
    _result = visitor.return_result();
}

void FirstSameConditionVisitor::visit_proc_condition(
        ProcCondition *condition1) 
{
    SecondSameConditionVisitor<ProcCondition> visitor(condition1);
    _condition2->accept_condition_visitor(&visitor);
    _result = visitor.return_result();
}

void FirstSameConditionVisitor::visit_variable_condition(
        VariableCondition *condition1)
{
    SecondSameConditionVisitor<VariableCondition> visitor(condition1);
    _condition2->accept_condition_visitor(&visitor);
    _result = visitor.return_result();
}

void FirstSameConditionVisitor::visit_pattern_condition(
        PatternCondition *condition1) 
{
    SecondSameConditionVisitor<PatternCondition> visitor(condition1);
    _condition2->accept_condition_visitor(&visitor);
    _result = visitor.return_result();
}

bool FirstSameConditionVisitor::return_result() {
    return _result;
}

template <typename FirstCondition>
SecondSameConditionVisitor<FirstCondition>::
SecondSameConditionVisitor(FirstCondition *condition1) :
    _condition1(condition1)
{ }

template <typename FirstCondition>
void SecondSameConditionVisitor<FirstCondition>::
visit_statement_condition(StatementCondition *condition2) 
{
    _result = is_same_condition<
        FirstCondition, StatementCondition>(_condition1, condition2);
}

template <typename FirstCondition>
void SecondSameConditionVisitor<FirstCondition>::
visit_proc_condition(ProcCondition *condition2) 
{
    _result = is_same_condition<
        FirstCondition, ProcCondition>(_condition1, condition2);
}

template <typename FirstCondition>
void SecondSameConditionVisitor<FirstCondition>::
visit_variable_condition(VariableCondition *condition2) 
{
    _result = is_same_condition<
        FirstCondition, VariableCondition>(_condition1, condition2);
}

template <typename FirstCondition>
void SecondSameConditionVisitor<FirstCondition>::
visit_pattern_condition(PatternCondition *condition2) 
{
    _result = is_same_condition<
        FirstCondition, PatternCondition>(_condition1, condition2);
}

template <typename FirstCondition>
bool SecondSameConditionVisitor<FirstCondition>::
return_result() {
    return _result;
}

} // namespace util
} // namespace simple
