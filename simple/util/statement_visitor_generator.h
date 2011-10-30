
#pragma once

#include <set>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {
namespace impl {

using namespace simple;

template <typename Solver>
class PartialValidateVisitorTraits {
  public:
    typedef bool                ResultType;
    typedef StatementAst        ContextType;

    template <typename Ast>
    static bool visit(Solver *solver, Ast *ast, 
                StatementAst *statement) 
    {
        return solver->template validate<Ast, StatementAst>(ast, statement);
    }
  private:
    PartialValidateVisitorTraits();
};

template <typename Solver>
class IndexVariableVisitorTraits {
  public:
    typedef std::set<SimpleVariable> ResultType;
    typedef int                    ContextType;

    template <typename Ast>
    static std::set<SimpleVariable> visit(
            Solver *solver, Ast *ast, int *context = NULL) 
    {
        return solver->template index_variables<Ast>(ast);
    }
  private:
    IndexVariableVisitorTraits();
};

template <typename Solver>
class SolveVariableVisitorTraits {
  public:
    typedef ConditionSet ResultType;
    typedef int            ContextType;

    template <typename Ast>
    static ConditionSet visit(Solver *solver, Ast *ast, int *context = NULL) {
        return solver->template solve_variable<Ast>(ast);
    }
  private:
    SolveVariableVisitorTraits();
};

template <typename Solver>
class SolveRightVisitorTraits {
  public:
    typedef ConditionSet ResultType;
    typedef int        ContextType;

    template <typename Ast>
    static ConditionSet visit(Solver *solver, Ast *ast, int *context = NULL) {
        return solver->template solve_right<Ast>(ast);
    }
  private:
    SolveRightVisitorTraits();
};

template <typename Solver>
class SolveLeftVisitorTraits {
  public:
    typedef ConditionSet ResultType;
    typedef int        ContextType;

    template <typename Ast>
    static ConditionSet visit(Solver *solver, Ast *ast, int *context = NULL) {
        return solver->template solve_left<Ast>(ast);
    }

  private:
    SolveLeftVisitorTraits();
};

template <typename Visitor, typename VisitorTraits>
class StatementVisitorGenerator : public StatementVisitor {
  public:
    typedef typename VisitorTraits::ResultType  ResultType;
    typedef typename VisitorTraits::ContextType ContextType;

    StatementVisitorGenerator(Visitor *visitor, ContextType *context = NULL) : 
        _visitor(visitor), _context(context) 
    { }

    virtual void visit_conditional(ConditionalAst *ast) {
        _result = VisitorTraits::template visit<ConditionalAst>(_visitor, ast, _context);
    }

    virtual void visit_while(WhileAst *ast) {
        _result = VisitorTraits::template visit<WhileAst>(_visitor, ast, _context);
    }

    virtual void visit_assignment(AssignmentAst *ast) {
        _result = VisitorTraits::template visit<AssignmentAst>(_visitor, ast, _context);
    }

    virtual void visit_call(CallAst *ast) {
        _result = VisitorTraits::template visit<CallAst>(_visitor, ast, _context);
    }

    typename VisitorTraits::ResultType return_result() {
        return std::move(_result);
    }

  private:
    Visitor *_visitor;
    ResultType _result;
    ContextType *_context;
};


}
}
