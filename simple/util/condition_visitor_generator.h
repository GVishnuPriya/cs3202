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

#include "simple/condition.h"

namespace simple {
namespace util {

using namespace simple;

template <typename Visitor, typename VisitorTraits>
class ConditionVisitorGenerator : public ConditionVisitor {
  public:
    ConditionVisitorGenerator(Visitor *visitor) : _visitor(visitor) { }

    void visit_statement_condition(StatementCondition *condition) {
        _result = VisitorTraits::template visit<StatementCondition>(_visitor, condition);
    }

    void visit_proc_condition(ProcCondition *condition) {
        _result = VisitorTraits::template visit<ProcCondition>(_visitor, condition);
    }

    void visit_variable_condition(VariableCondition *condition) {
        _result = VisitorTraits::template visit<VariableCondition>(_visitor, condition);
    }

    void visit_constant_condition(ConstantCondition *condition) {
        _result = VisitorTraits::template visit<ConstantCondition>(_visitor, condition);
    }

    void visit_pattern_condition(PatternCondition *condition) {
        _result = VisitorTraits::template visit<PatternCondition>(_visitor, condition);
    }

    void visit_operator_condition(OperatorCondition* condition) {
        _result = VisitorTraits::template visit<OperatorCondition>(_visitor, condition);
    }

    typename VisitorTraits::ResultType return_result() {
        return std::move(_result);
    }

  private:
    Visitor *_visitor;
    typename VisitorTraits::ResultType _result;
};

template <typename FirstCondition, typename VisitorTraits>
class SecondConditionVisitorGenerator : public ConditionVisitor {
  public:
    SecondConditionVisitorGenerator(FirstCondition *condition1) :
        _condition1(condition1)
    { }

    void visit_statement_condition(StatementCondition *condition2) {
        _result = VisitorTraits::template visit_condition<
            FirstCondition, StatementCondition>(_condition1, condition2);
    }

    void visit_proc_condition(ProcCondition *condition2) {
        _result = VisitorTraits::template visit_condition<
            FirstCondition, ProcCondition>(_condition1, condition2);
    }
    
    void visit_variable_condition(VariableCondition *condition2) {
        _result = VisitorTraits::template visit_condition<
            FirstCondition, VariableCondition>(_condition1, condition2);
    }

    void visit_constant_condition(ConstantCondition *condition2) {
        _result = VisitorTraits::template visit_condition<
            FirstCondition, ConstantCondition>(_condition1, condition2);
    }

    void visit_pattern_condition(PatternCondition *condition2) {
        _result = VisitorTraits::template visit_condition<
            FirstCondition, PatternCondition>(_condition1, condition2);
    }

    void visit_operator_condition(OperatorCondition* condition2) {
        _result = VisitorTraits::template visit_condition<
            FirstCondition, OperatorCondition>(_condition1, condition2);
    }

    typename VisitorTraits::ResultType return_result() {
        return _result;
    }

  private:
    typename VisitorTraits::ResultType _result;
    FirstCondition *_condition1;
};

template <typename VisitorTraits>
class FirstConditionVisitorGenerator : public ConditionVisitor {
  public:
    FirstConditionVisitorGenerator(SimpleCondition *condition2) :
        _condition2(condition2) 
    { }

    void visit_statement_condition(StatementCondition *condition1) {
        SecondConditionVisitorGenerator<StatementCondition, VisitorTraits> 
        visitor(condition1);

        _condition2->accept_condition_visitor(&visitor);
        _result = visitor.return_result();
    }
    void visit_proc_condition(ProcCondition *condition1){
        SecondConditionVisitorGenerator<ProcCondition, VisitorTraits> 
        visitor(condition1);

        _condition2->accept_condition_visitor(&visitor);
        _result = visitor.return_result();
    }

    void visit_variable_condition(VariableCondition *condition1) {
        SecondConditionVisitorGenerator<VariableCondition, VisitorTraits> 
        visitor(condition1);

        _condition2->accept_condition_visitor(&visitor);
        _result = visitor.return_result();
    }

    void visit_constant_condition(ConstantCondition *condition1) {
       SecondConditionVisitorGenerator<ConstantCondition, VisitorTraits> 
        visitor(condition1);

        _condition2->accept_condition_visitor(&visitor);
        _result = visitor.return_result(); 
    }

    void visit_pattern_condition(PatternCondition *condition1) {
        SecondConditionVisitorGenerator<PatternCondition, VisitorTraits> 
        visitor(condition1);

        _condition2->accept_condition_visitor(&visitor);
        _result = visitor.return_result();
    }

    void visit_operator_condition(OperatorCondition* condition1) {
        SecondConditionVisitorGenerator<OperatorCondition, VisitorTraits> 
        visitor(condition1);

        _condition2->accept_condition_visitor(&visitor);
        _result = visitor.return_result();
    }

    typename VisitorTraits::ResultType return_result() {
        return _result;
    }

  private:
    typename VisitorTraits::ResultType _result;
    SimpleCondition *_condition2;
};

} // namespace util
} // namespace simple
