
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/predicate.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

template <typename Predicate>
class PredicateGenerator : 
    public SimplePredicate, public StatementVisitor,
    public ExprVisitor
{
  public:
    PredicateGenerator(SimpleRoot ast, Predicate *pred) : 
        _ast(ast), _global_set(), _pred(pred)
    {
        create_global_set();
    }

    ConditionSet global_set() {
        return _global_set;
    }

    void filter_set(ConditionSet& conditions) {
        conditions.intersect_with(_global_set);
    }

    void visit_assignment(AssignmentAst *assign) {
        if(_pred->template evaluate<AssignmentAst>(assign)) {
            _global_set.insert(new SimpleStatementCondition(assign));
        }

        if(_pred->template evaluate<SimpleVariable>(assign->get_variable())) {
            _global_set.insert(new SimpleVariableCondition(*assign->get_variable()));
        }

        if(_pred->template evaluate<ExprAst>(assign->get_expr())) {
            _global_set.insert(new SimplePatternCondition(assign->get_expr()->clone()));
        }

        assign->get_expr()->accept_expr_visitor(this);
    }

    void visit_conditional(ConditionalAst *condition) {
        if(_pred->template evaluate<ConditionalAst>(condition)) {
            _global_set.insert(new SimpleStatementCondition(condition));
        }

        if(_pred->template evaluate<SimpleVariable>(condition->get_variable())) {
            _global_set.insert(new SimpleVariableCondition(*condition->get_variable()));
        }

        filter_statement_list(condition->get_then_branch());
        filter_statement_list(condition->get_else_branch());
    }

    void visit_while(WhileAst *loop) {
        if(_pred->template evaluate<WhileAst>(loop)) {
            _global_set.insert(new SimpleStatementCondition(loop));
        }

        if(_pred->template evaluate<SimpleVariable>(loop->get_variable())) {
            _global_set.insert(new SimpleVariableCondition(*loop->get_variable()));
        }

        filter_statement_list(loop->get_body());
    }

    void visit_call(CallAst *call) {
        if(_pred->template evaluate<CallAst>(call)) {
            _global_set.insert(new SimpleStatementCondition(call));
        }
    }

    void visit_variable(VariableAst *var) {
        if(_pred->template evaluate<SimpleVariable>(var->get_variable())) {
            _global_set.insert(new SimpleVariableCondition(*var->get_variable()));
        }
    }

    void visit_const(ConstAst *val) {
        // no-op
    }

    void visit_binary_op(BinaryOpAst *bin) {
        bin->get_lhs()->accept_expr_visitor(this);
        bin->get_rhs()->accept_expr_visitor(this);
    }

  private:
    void create_global_set() {
        for(SimpleRoot::iterator it = _ast.begin();
            it != _ast.end(); ++it)
        {
            ProcAst *proc = *it;
            if(_pred->template evaluate<ProcAst>(proc)) {
                _global_set.insert(new SimpleProcCondition(proc));
            }

            filter_statement_list(proc->get_statement());
        }
    }

    void filter_statement_list(StatementAst *statement) {
        while(statement != NULL) {
            statement->accept_statement_visitor(this);
            statement = statement->next();
        }
    }

    SimpleRoot _ast;
    ConditionSet _global_set;
    std::unique_ptr<Predicate> _pred;
};

/*
 * A predicate that accepts everything.
 */
class WildCardPredicate {
  public:
    WildCardPredicate() { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return true;
    }
};

class ProcPredicate {
  public:
    ProcPredicate() { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return false;
    }
};

template <>
bool ProcPredicate::evaluate<ProcAst>(ProcAst *proc) {
    return true;
}

class ProcPredicateWithName {
  public:
    ProcPredicateWithName(const std::string& name) : _name(name) { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return false;
    }


  private:
    std::string _name;
};

template <>
bool ProcPredicateWithName::evaluate<ProcAst>(ProcAst *proc) {
    return proc->get_name() == _name;
}

class StatementPredicate {
  public:
    StatementPredicate() { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return false;
    }

};

template <>
bool StatementPredicate::evaluate<AssignmentAst>(AssignmentAst *assign) {
    return true;
}

template <>
bool StatementPredicate::evaluate<WhileAst>(WhileAst *loop) {
    return true;
}

template <>
bool StatementPredicate::evaluate<ConditionalAst>(ConditionalAst *condition) {
    return true;
}

template <>
bool StatementPredicate::evaluate<CallAst>(CallAst *call) {
    return true;
}

class AssignPredicate {
  public:
    AssignPredicate() { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return false;
    }
};

template <>
bool AssignPredicate::evaluate<AssignmentAst>(AssignmentAst *assign) {
    return true;
}

class ConditionalPredicate {
  public:
    ConditionalPredicate() { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return false;
    }
};

template <>
bool ConditionalPredicate::evaluate<ConditionalAst>(ConditionalAst *condition) {
    return true;
}

class WhilePredicate {
  public:
    WhilePredicate() { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return false;
    }

};

template <>
bool WhilePredicate::evaluate<WhileAst>(WhileAst *loop) {
    return true;
}

class VariablePredicate {
  public:
    VariablePredicate() { }

    template <typename Condition>
    bool evaluate(Condition *condition) {
        return false;
    }

};

template <>
bool VariablePredicate::evaluate<SimpleVariable>(SimpleVariable *variable) {
    return true;
}

typedef PredicateGenerator<WildCardPredicate> SimpleWildCardPredicate;
typedef PredicateGenerator<StatementPredicate> SimpleStatementPredicate;
typedef PredicateGenerator<AssignPredicate> SimpleAssignmentPredicate;

}
}
