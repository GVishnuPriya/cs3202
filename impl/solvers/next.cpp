
#include "impl/condition.h"
#include "impl/solvers/next.h"
#include "simple/util/statement_visitor_generator.h"
#include "simple/util/ast_utils.h"

namespace simple {
namespace impl {

using namespace simple;
using simple::util::is_statement_type;

template <typename Condition>
ConditionSet NextSolver::solve_right(Condition *condition) {
    return ConditionSet();
}

template <>
ConditionSet NextSolver::solve_right<StatementAst>(StatementAst *ast) {
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
            result.insert(new SimpleStatementCondition(
                        ast->get_parent()->next()));

        } else if(is_statement_type<WhileAst>(ast->get_parent())) {
            /*
             * If the statement is the last statement in a while clause,
             * the the next statement is to back to the while clause 
             * itself.
             */
            result.insert(new SimpleStatementCondition(ast->get_parent()));
        }
    }

    result.union_with(visitor.return_result());
}

template <>
ConditionSet NextSolver::solve_right<ConditionalAst>(ConditionalAst *ast) {
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
ConditionSet NextSolver::solve_right<WhileAst>(WhileAst *ast) {
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
ConditionSet NextSolver::solve_right<AssignmentAst>(AssignmentAst *ast) {
    ConditionSet result;
    if(ast->next()) {
        result.insert(new SimpleStatementCondition(ast->next()));
    }
    return result;
}

template <>
ConditionSet NextSolver::solve_right<CallAst>(CallAst *ast) {
    ConditionSet result;
    if(ast->next()) {
        result.insert(new SimpleStatementCondition(ast->next()));
    }
    return result;
}


template <typename Condition>
ConditionSet NextSolver::solve_previous(Condition *condition) {
    return ConditionSet();
}

class SolvePreviousVisitorTraits {
  public:
    typedef ConditionSet ResultType;

    template <typename Ast>
    static ConditionSet visit(NextSolver *solver, Ast *ast) {
        return solver->solve_previous<Ast>(ast);
    }
};

template <>
ConditionSet NextSolver::solve_previous<StatementAst>(StatementAst *ast) {
    StatementVisitorGenerator<NextSolver, SolvePreviousVisitorTraits>
    visitor(this);

    ast->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
ConditionSet NextSolver::solve_previous<WhileAst>(WhileAst *ast) {
    ConditionSet result;
    result.insert(new SimpleStatementCondition(ast));
    return result;
}

template <>
ConditionSet NextSolver::solve_previous<AssignmentAst>(AssignmentAst *ast) {
    ConditionSet result;
    result.insert(new SimpleStatementCondition(ast));
    return result;
}

template <>
ConditionSet NextSolver::solve_previous<CallAst>(CallAst *ast) {
    ConditionSet result;
    result.insert(new SimpleStatementCondition(ast));
    return result;
}

template <>
ConditionSet NextSolver::solve_previous<ConditionalAst>(ConditionalAst *ast) {
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


template <typename Condition>
ConditionSet NextSolver::solve_left(Condition *condition) {
    return ConditionSet(); // empty set
}

template <>
ConditionSet NextSolver::solve_left<StatementAst>(StatementAst *ast) {
    ConditionSet result;
    
    /*
     * If the statement is not the first, then we look at it's previous
     * statement and see if the previous statement happen to be a conditional
     * statement, which becomes handy according to the Next() specification.
     */
    if(ast->prev()) {
        result.union_with(solve_previous<StatementAst>(ast->prev()));
    /*
     * else the statement is the first statement and if it has a container,
     * then the container is the previous statement.
     */
    } else if(ast->get_parent()) {
        result.insert(new SimpleStatementCondition(ast->get_parent()));
    }

    StatementVisitorGenerator<NextSolver,
        SolveLeftVisitorTraits<NextSolver> > visitor(this);
    ast->accept_statement_visitor(&visitor);

    result.union_with(visitor.return_result());
}


template <typename Condition1, typename Condition2>
bool NextSolver::validate(Condition1 *condition1, Condition2 *condition2) {
    return false;
}

template <>
bool NextSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2)
{
    ValidateNextStatementVisitor visitor(this, statement2);
    statement1->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool NextSolver::validate_next<ConditionalAst>(
        ConditionalAst *condition, StatementAst *statement)
{
    return (condition->get_then_branch() == statement ||
            condition->get_else_branch() == statement);
}

template <typename Condition>
bool NextSolver::validate_next(Condition *condition, StatementAst *statement) {
    return false;
}

template <>
bool NextSolver::validate_next<WhileAst>(
        WhileAst *loop, StatementAst *statement)
{
    return (loop->get_body() == statement || loop->next() == statement);
}

template <>
bool NextSolver::validate_next<AssignmentAst>(
        AssignmentAst *assign, StatementAst *statement)
{
    return assign->next() == statement;
}

template <>
bool NextSolver::validate_next<CallAst>(
        CallAst *call, StatementAst *statement)
{
    return call->next() == statement;
}

void NextSolver::ValidateNextStatementVisitor::visit_conditional(ConditionalAst *ast) {
    _result = _solver->validate_next<ConditionalAst>(ast, _statement);
}

void NextSolver::ValidateNextStatementVisitor::visit_while(WhileAst *ast) {
    _result = _solver->validate_next<WhileAst>(ast, _statement);
}

void NextSolver::ValidateNextStatementVisitor::visit_assignment(AssignmentAst *ast) {
    _result = _solver->validate_next<AssignmentAst>(ast, _statement);
}

void NextSolver::ValidateNextStatementVisitor::visit_call(CallAst *ast) {
    _result = _solver->validate_next<CallAst>(ast, _statement);
}



} // namespace impl
} // namespace simple
