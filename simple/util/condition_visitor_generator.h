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

    typename VisitorTraits::ResultType return_result() {
        return _result;
    }

  private:
    typename VisitorTraits::ResultType _result;
    SimpleCondition *_condition2;
};

} // namespace util
} // namespace simple
