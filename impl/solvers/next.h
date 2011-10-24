
#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"

namespace simple {
namespace impl {
namespace solver {

class NextSolver {
  public:
    NextSolver(SimpleRoot ast, SolverTable *table) : _ast(ast) { }

    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return ConditionSet();
    }

    template <>
    ConditionSet solve_right<StatementAst>(StatementAst *ast) {
        ConditionSet result;
        
        StatementVisitorGenerator<NextSolver,
            SolveRightVisitorTraits<NextSolver> > visitor(this);
        ast->accept_statement_visitor(&visitor);

        /*
         * If the statement ast has a container statement and is
         * in the last of the list
         */
        if(ast->get_parent() && !ast->next()) {
            if(is_statement_type<ConditionalAst>(ast->get_parent())) {
                /*
                 * If the statement is the last statement in an if clause,
                 * then the next statement is the one following the 
                 * if statement.
                 */
                result.insert(ast->get_parent()->next());
            } else if(is_statement_type<WhileAst>(ast->get_parent())) {
                /*
                 * If the statement is the last statement in a while clause,
                 * the the next statement is to back to the while clause 
                 * itself.
                 */
                result.insert(ast->get_parent());
            }
        }

        result.union_with(visitor.return_result());
    }

    template <>
    ConditionSet solve_right<ConditionalAst>(ConditionalAst *ast) {
        ConditionSet result;

        /*
         * The next statement executed after an if clause is either the 
         * then branch or else branch
         */
        result.insert(new SimpleStatementCondition(ast->get_then_branch()));
        result.insert(new SimpleStatementCondition(ast->get_else_branch()));

        return result;
    }

    template <>
    ConditionSet solve_right<WhileAst>(WhileAst *ast) {
        ConditionSet result;

        /*
         * The next statement executed after a while clause is the first
         * body statement or the statement following the while statement.
         */
        result.insert(new SimpleStatementCondition(ast->get_body()));

        if(ast->next()) {
            result.insert(new SimpleStatementCondition(ast->next()));
        }
        
        return result;
    }

    template <>
    ConditionSet solve_right<AssignmentAst>(AssignmentAst *ast) {
        ConditionSet result;
        if(ast->next()) {
            result.insert(new SimpleStatementCondition(ast->next()));
        }
        return result;
    }
    
    template <>
    ConditionSet solve_right<CallAst>(CallAst *ast) {
        ConditionSet result;
        if(ast->next()) {
            result.insert(new SimpleStatementCondition(ast->next()));
        }
        return result;
    }

    /*
     * SOLVE LEFT PART
     */

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return ConditionSet(); // empty set
    }

    template <>
    ConditionSet solve_left<StatementAst>(StatementAst *ast) {
        ConditionSet result;
        
        if(ast->prev()) {
            result.union_with(solve_previous<StatementAst>(ast->prev()));
        /*
         * else the statement is the first statement and if it has a container.
         */
        } else if(ast->get_parent()) {
            result.insert(new SimpleStatementCondition(ast->get_parent()));
        }

        StatementVisitorGenerator<NextSolver,
            SolveLeftVisitorTraits<NextSolver> > visitor(this);
        ast->visit(&visitor);

        result.union_with(visitor.return_result());
    }

    template <>
    ConditionSet solve_left<WhileAst>(WhileAst *ast) {
        ConditionSet result;

        result.insert(new SimpleStatementCondition(ast));
    }

    template <typename Condition>
    ConditionSet solve_previous(Condition *condition) {
        return ConditionSet();
    }

    template <>
    ConditionSet solve_previous<StatementAst>(StatementAst *ast) {

    }

    template <>
    ConditionSet solve_previous<WhileAst>(WhileAst *ast) {
        ConditionSet result;
        result.insert(new SimpleStatementCondition(ast));
        return result;
    }

    template <>
    ConditionSet solve_previous<AssignmentAst>(AssignmentAst *ast) {
        ConditionSet result;
        result.insert(new SimpleStatementCondition(ast));
        return result;
    }

    template <>
    ConditionSet solve_previous<CallAst>(CallAst *ast) {
        ConditionSet result;
        result.insert(new SimpleStatementCondition(ast));
        return result;
    }

    template <>
    ConditionSet solve_previous<ConditionalAst>(ConditionalAst *ast) {
        ConditionSet result;
        StatementAst *then_branch = ast->get_then_branch();
        StatementAst *else_branch = ast->get_else_branch();

        /*
         * Go to the last statement in then and else branch
         */
        while(then_branch->next()) {
            then_branch = then_branch->next();
        }

        while(else_branch->next()) {
            else_branch = else_branch->next();
        }

        result.union_with(solve_previous<StatementAst>(then_branch));
        result.union_with(solve_previous<StatementAst>(else_branch));

        return result;
    }
   
  private:
    SimpleRoot _ast;
};

} // namespace solver
} // namespace impl
} // namespace simple
