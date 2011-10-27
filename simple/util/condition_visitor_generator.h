
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
