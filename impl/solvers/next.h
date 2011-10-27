
#pragma once

#include "simple/ast.h"
#include "simple/util/ast_utils.h"
#include "simple/condition.h"
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;

class NextSolver {
  public:
    NextSolver(SimpleRoot ast, SolverTable *table) : _ast(ast) { }

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    ConditionSet solve_previous(Condition *condition);

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    bool validate_next(Condition *condition, StatementAst *statement);

   
  private:
    SimpleRoot _ast;

    class ValidateNextStatementVisitor : public StatementVisitor {
      public:
        ValidateNextStatementVisitor(
            NextSolver *solver, StatementAst *statement) : 
            _solver(solver), _statement(statement), 
            _result(false) 
        { }

        void visit_conditional(ConditionalAst *ast);
        void visit_while(WhileAst *ast);
        void visit_assignment(AssignmentAst *ast);
        void visit_call(CallAst *ast);

        bool return_result() {
            return _result;
        }

      private:
        bool            _result;
        NextSolver      *_solver;
        StatementAst    *_statement;
    };
};

} // namespace impl
} // namespace simple
