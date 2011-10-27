
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"

namespace simple {
namespace impl {

using namespace simple;

template <typename Solver>
class SolveRightVisitorTraits {
  public:
    typedef ConditionSet ResultType;

    template <typename Ast>
    static ConditionSet visit(Solver *solver, Ast *ast) {
        return solver->template solve_right<Ast>(ast);
    }
  private:
    SolveRightVisitorTraits();
};

template <typename Solver>
class SolveLeftVisitorTraits {
  public:
    typedef ConditionSet ResultType;

    template <typename Ast>
    static ConditionSet visit(Solver *solver, Ast *ast) {
        return solver->template solve_left<Ast>(ast);
    }

  private:
    SolveLeftVisitorTraits();
};

template <typename Visitor, typename VisitorTraits>
class StatementVisitorGenerator : public StatementVisitor {
      public:
        StatementVisitorGenerator(Visitor *visitor) : 
            _visitor(visitor) { }

        virtual void visit_conditional(ConditionalAst *ast) {
            _result = VisitorTraits::template visit<ConditionalAst>(_visitor, ast);
        }

        virtual void visit_while(WhileAst *ast) {
            _result = VisitorTraits::template visit<WhileAst>(_visitor, ast);
        }

        virtual void visit_assignment(AssignmentAst *ast) {
            _result = VisitorTraits::template visit<AssignmentAst>(_visitor, ast);
        }

        virtual void visit_call(CallAst *ast) {
            _result = VisitorTraits::template visit<CallAst>(_visitor, ast);
        }

        typename VisitorTraits::ResultType return_result() {
            return std::move(_result);
        }

      private:
        Visitor *_visitor;
        typename VisitorTraits::ResultType _result;
};


}
}
