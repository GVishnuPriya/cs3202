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

#include <memory>
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;

template <typename ConcreteSolver>
class SimpleSolverGenerator : public QuerySolver  {
  private:
 
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

        void visit_constant_condition(ConstantCondition *condition) {
            _result = _solver->template solve_left<SimpleConstant>(condition->get_constant());
        }

        void visit_pattern_condition(PatternCondition *condition) {
            _result = _solver->template solve_left<ExprAst>(condition->get_expr_ast());
        }

        void visit_operator_condition(OperatorCondition* condition) {
            _result = _solver->template solve_left<OperatorCondition>(condition);
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

        void visit_constant_condition(ConstantCondition *condition) {
            _result = _solver->template solve_right<SimpleConstant>(condition->get_constant());
        }

        void visit_pattern_condition(PatternCondition *condition) {
            _result = _solver->template solve_right<ExprAst>(condition->get_expr_ast());
        }

        void visit_operator_condition(OperatorCondition* condition) {
            _result = _solver->template solve_right<OperatorCondition>(condition);
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        ConcreteSolver *_solver;
        ConditionSet _result;
    };

    template <typename Condition>
    class SecondSolverVisitor : public ConditionVisitor {
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

        void visit_constant_condition(ConstantCondition *right_condition) {
            _result = _solver->template validate<Condition, SimpleConstant>(
                    _left_condition, right_condition->get_constant());
        }

        void visit_pattern_condition(PatternCondition *right_condition) {
            _result = _solver->template validate<Condition, ExprAst>(
                    _left_condition, right_condition->get_expr_ast());
        }

        void visit_operator_condition(OperatorCondition* right_condition) {
            _result = _solver->template validate<Condition, OperatorCondition>(
                    _left_condition, right_condition);
        }

        bool return_result() {
            return std::move(_result);
        }

      private:
        ConcreteSolver *_solver;
        Condition *_left_condition;
        bool _result;

    };

    class FirstSolverVisitor : public ConditionVisitor {
      public:
        FirstSolverVisitor(ConcreteSolver *solver, SimpleCondition *right_condition) :
            _solver(solver), 
            _right_condition(right_condition)
        { }

        void visit_statement_condition(StatementCondition *condition) {
            SecondSolverVisitor<StatementAst> visitor(_solver, condition->get_statement_ast());
            _right_condition->accept_condition_visitor(&visitor);
            _result = visitor.return_result();
        }

        void visit_proc_condition(ProcCondition *condition) {
            SecondSolverVisitor<ProcAst> visitor(_solver, condition->get_proc_ast());
            _right_condition->accept_condition_visitor(&visitor);
            _result = visitor.return_result();
        }

        void visit_variable_condition(VariableCondition *condition) {
            SecondSolverVisitor<SimpleVariable> visitor(_solver, condition->get_variable());
            _right_condition->accept_condition_visitor(&visitor);
            _result = visitor.return_result();
        }

        void visit_constant_condition(ConstantCondition *condition) {
            SecondSolverVisitor<SimpleConstant> visitor(_solver, condition->get_constant());
            _right_condition->accept_condition_visitor(&visitor);
            _result = visitor.return_result();
        }

        void visit_pattern_condition(PatternCondition *condition) {
            SecondSolverVisitor<ExprAst> visitor(_solver, condition->get_expr_ast());
            _right_condition->accept_condition_visitor(&visitor);
            _result = visitor.return_result();
        }

        void visit_operator_condition(OperatorCondition* right_condition) {
            SecondSolverVisitor<OperatorCondition> visitor(_solver, right_condition);
            _right_condition->accept_condition_visitor(&visitor);
            _result = visitor.return_result();
        }

        bool return_result() {
            return std::move(_result);
        }

      private:
        bool _result;
        ConcreteSolver  *_solver;
        SimpleCondition *_right_condition;
    };

  public:
    SimpleSolverGenerator(ConcreteSolver *solver) : _solver(solver) { }
    
    SimpleSolverGenerator(std::shared_ptr<ConcreteSolver> solver) : 
        _solver(solver) 
    { }

    virtual ConditionSet solve_left(SimpleCondition *right_condition) {
        SolverLeftVisitor visitor(_solver.get());
        right_condition->accept_condition_visitor(&visitor);
        return visitor.return_result();
    }

    virtual ConditionSet solve_right(SimpleCondition *left_condition) {
        SolverRightVisitor visitor(_solver.get());
        left_condition->accept_condition_visitor(&visitor);
        return visitor.return_result();
    }

    virtual bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition) {
        FirstSolverVisitor visitor(_solver.get(), right_condition);
        left_condition->accept_condition_visitor(&visitor);
        return visitor.return_result();
    }

    ConcreteSolver* get_solver() {
        return _solver.get();
    }

  private:
    std::shared_ptr<ConcreteSolver> _solver;

};
    
} // namespace impl
} // namespace simple
