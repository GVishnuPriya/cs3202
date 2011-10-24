
#pragma once

#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple::solver;

template <typename Visitor, typename VisitorTraits>
class StatementVisitorGenerator : public StatementVisitor {
      public:
        StatementVisitorGenerator(Visitor *visitor) : 
            _visitor(visitor) { }

        virtual void visit_conditional(ConditionalAst *ast) {
            _result = VisitorTraits::visit<ConditionalAst>(_visitor, ast);
        }

        virtual void visit_while(WhileAst *ast) {
            _result = VisitorTraits::visit<WhileAst>(_visitor, ast);
        }

        virtual void visit_assignment(AssignmentAst *ast) {
            _result = VisitorTraits::visit<AssignmentAst>(_visitor, ast);
        }

        virtual void visit_call(CallAst *ast) {
            _result = VisitorTraits::visit<CallAst>(_visitor, ast);
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        Visitor *_visitor;
        ConditionSet _result;
};

template <typename Solver>
class SolveRightVisitorTraits {
  public:
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
    template <typename Ast>
    static ConditionSet visit(Solver *solver, Ast *ast) {
        return solver->template solve_left<Ast>(ast);
    }

  private:
    SolveLeftVisitorTraits();
};


template <typename ConcreteSolver>
class SimpleSolverGenerator : public QuerySolver  {
  private:
    class SolverLeftVisitor;
    class SolverRightVisitor;
    class FirstSolverVisitor;

    template <typename Condition>
    class SecondSolverVisitor;

  public:
    SimpleSolverGenerator(SimpleRoot ast, SolverTable *table) : _solver(ast, table) { }

    virtual ConditionSet solve_left(SimpleCondition *right_condition) {
        SolverLeftVisitor visitor(&_solver);
        right_condition->accept_condition_visitor(&visitor);
        return visitor.return_result();
    }

    virtual ConditionSet solve_right(SimpleCondition *left_condition) {
        SolverRightVisitor visitor(&_solver);
        left_condition->accept_condition_visitor(&visitor);
        return visitor.return_result();
    }

    virtual bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition) {
        FirstSolverVisitor visitor(&_solver, right_condition);
        left_condition->accept_condition_visitor(&visitor);
        return visitor.return_result();
    }

  private:
    ConcreteSolver _solver;

    class SolverLeftVisitor : public ConditionVisitor {
      public:
        SolverLeftVisitor(ConcreteSolver *solver) : _solver(solver) { }

        void visit_statement_condition(StatementCondition *condition) {
            _result = _solver->template solve_left<StatementAst>(condition->get_statement_ast());
        }

        void visit_proc_condition(ProcCondition *condition) {
            _result = _solver->template solve_left<ProcAst>(condition->get_proc_ast());
        }

        void visit_variable_condition(VariableCondition *condition) {
            _result = _solver->template solve_left<SimpleVariable>(condition->get_variable());
        }

        void visit_pattern_condition(PatternCondition *condition) {
            _result = _solver->template solve_left<ExprAst>(condition->get_expr_ast());
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        ConcreteSolver *_solver;
        ConditionSet _result;
    };

    class SolverRightVisitor : public ConditionVisitor {
      public:
        SolverRightVisitor(ConcreteSolver *solver) : _solver(solver) { }

        void visit_statement_condition(StatementCondition *condition) {
            _result = _solver->template solve_right<StatementAst>(condition->get_statement_ast());
        }

        void visit_proc_condition(ProcCondition *condition) {
            _result = _solver->template solve_right<ProcAst>(condition->get_proc_ast());
        }

        void visit_variable_condition(VariableCondition *condition) {
            _result = _solver->template solve_right<SimpleVariable>(condition->get_variable());
        }

        void visit_pattern_condition(PatternCondition *condition) {
            _result = _solver->template solve_right<ExprAst>(condition->get_expr_ast());
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        ConcreteSolver *_solver;
        ConditionSet _result;
    };

    class ConditionVisitorWithResult : public ConditionVisitor {
      public:
        virtual ConditionSet return_result() = 0;

        virtual ~ConditionVisitorWithResult() { }
    };

    class FirstSolverVisitor : public ConditionVisitor {
      public:
        FirstSolverVisitor(ConcreteSolver *solver, SimpleCondition *right_condition) :
            _solver(solver), 
            _right_condition(right_condition),
            _second_visitor(NULL)
        { }

        void visit_statement_condition(StatementCondition *condition) {
            _second_visitor = new SecondSolverVisitor<
                StatementAst>(_solver, condition->get_statement_ast());
        }

        void visit_proc_condition(ProcCondition *condition) {
            _second_visitor = new SecondSolverVisitor<
                ProcAst>(_solver, condition->get_proc_ast());
        }

        void visit_variable_condition(VariableCondition *condition) {
            _second_visitor = new SecondSolverVisitor<
                SimpleVariable>(_solver, condition->get_variable());
        }

        void visit_pattern_condition(PatternCondition *condition) {
            _second_visitor = new SecondSolverVisitor<
                ExprAst>(_solver, condition->get_expr_ast());
        }

        ConditionSet return_result() {
            return _second_visitor->return_result();
        }

      private:
        ConcreteSolver *_solver;
        SimpleCondition *_right_condition;
        std::unique_ptr<ConditionVisitorWithResult> _second_visitor;
    };

    template <typename Condition>
    class SecondSolverVisitor : public ConditionVisitorWithResult {
      public:
        SecondSolverVisitor(ConcreteSolver *solver, Condition *condition) :
            _solver(solver), 
            _left_condition(condition),
            _result()
        { }

        void visit_statement_condition(StatementCondition *right_condition) {
            _result = _solver->template validate<Condition, StatementAst>(
                    _left_condition, right_condition->get_statement_ast());
        }

        void visit_proc_condition(ProcCondition *right_condition) {
            _result = _solver->template validate<Condition, ProcAst>(
                    _left_condition, right_condition->get_proc_ast());
        }

        void visit_variable_condition(VariableCondition *right_condition) {
            _result = _solver->template validate<Condition, SimpleVariable>(
                    _left_condition, right_condition->get_variable());
        }

        void visit_pattern_condition(PatternCondition *right_condition) {
            _result = _solver->template validate<Condition, ExprAst>(
                    _left_condition, right_condition->get_expr_ast());
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        ConcreteSolver *_solver;
        Condition *_left_condition;
        ConditionSet _result;

    };
};

/*
 * This is a generic solver generator that adds memoization to
 * existing solver class
 */
template <typename ConcreteSolver>
class MemoizedSolverGenerator : public QuerySolver  {
  public:
    MemoizedSolverGenerator(SimpleRoot ast) : _solver(ast) { }

    virtual ConditionSet solve_left(SimpleCondition *right_condition) {

    }

    virtual ConditionSet solve_right(SimpleCondition *left_condition) {

    }

    virtual bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition) {

    }

  private:
    ConcreteSolver _solver;
    // table declarations here
};

    
} // namespace impl
} // namespace simple
