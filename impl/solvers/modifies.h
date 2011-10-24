
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"
#include "impl/visitor_generator.h"

namespace simple {
namespace impl {
namespace solver {

using namespace simple::ast;
using namespace simple::condition;
using namespace simple::solver;
using namespace simple::impl;

class ModifiesSolver {
  public:
    ModifiesSolver(SimpleRoot ast, SolverTable *table) : _ast(ast) { }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2);

    template <typename Condition>
    ConditionSet solve_right(Condition *condition);

    template <typename Condition>
    ConditionSet solve_left(Condition *condition);

    template <typename Condition>
    ConditionSet solve_variable(Condition *condition, SimpleVariable *variable);

  private:
    SimpleRoot _ast;

    class ModifiesValidateStatementVisitor : public StatementVisitor {
      public:
        ModifiesValidateStatementVisitor(
            ModifiesSolver *solver, SimpleVariable *var) : 
            _solver(solver), _var(var) { }

        virtual void visit_conditional(ConditionalAst *ast);
        virtual void visit_while(WhileAst *ast);
        virtual void visit_assignment(AssignmentAst *ast);
        virtual void visit_call(CallAst *ast);

        bool return_result() {
            return _result;
        }

      private:
        ModifiesSolver *_solver;
        SimpleVariable *_var;
        bool _result;
    };

};

} // namespace solver
} // namespace impl
} // namespace simple
