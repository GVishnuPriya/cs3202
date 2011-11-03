
#include <assert.h>
#include "impl/condition.h"
#include "impl/solvers/next.h"
#include "simple/util/statement_visitor_generator.h"
#include "simple/util/ast_utils.h"

namespace simple {
namespace impl {

using namespace simple;
using simple::util::is_statement_type;

class SolveNextVisitorTraits {
  public:
    typedef StatementSet ResultType;
    typedef int ContextType;

    template <typename Ast>
    static StatementSet visit(NextSolver *solver, Ast *ast, int *context = NULL) {
        return solver->solve_next<Ast>(ast);
    }
};

class SolvePreviousVisitorTraits {
  public:
    typedef StatementSet ResultType;
    typedef int ContextType;

    template <typename Ast>
    static StatementSet visit(NextSolver *solver, Ast *ast, int *context = NULL) {
        return solver->solve_previous<Ast>(ast);
    }
};

class SolvePreceedingPreviousVisitorTraits {
  public:
    typedef StatementSet ResultType;
    typedef int ContextType;

    template <typename Ast>
    static StatementSet visit(NextSolver *solver, Ast *ast, int *context = NULL) {
        return solver->solve_preceeding_previous<Ast>(ast);
    }
};

class SolveLastPreviousVisitorTraits {
  public:
    typedef StatementSet    ResultType;
    typedef int ContextType;

    template <typename Ast>
    static StatementSet visit(NextSolver *solver, Ast *ast, int *context = NULL) {
        return solver->solve_last_previous<Ast>(ast);
    }
};



class ValidateNextStatementVisitor : public StatementVisitor {
  public:
    ValidateNextStatementVisitor(
        NextSolver *solver, StatementAst *statement) : 
        _result(false), _solver(solver), 
        _statement(statement)
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

template <>
ConditionSet NextSolver::solve_right<StatementAst>(StatementAst *ast) {
    StatementSet statements = solve_next<StatementAst>(ast);
    ConditionSet result;

    for(StatementSet::iterator it = statements.begin(); 
            it!= statements.end(); ++it)
    {
        result.insert(new SimpleStatementCondition(*it));
    }
    return result;
}

template <>
ConditionSet NextSolver::solve_left<StatementAst>(StatementAst *ast) {
    StatementSet statements = solve_previous<StatementAst>(ast);
    ConditionSet result;

    for(StatementSet::iterator it = statements.begin(); 
            it!= statements.end(); ++it)
    {
        result.insert(new SimpleStatementCondition(*it));
    }
    return result;
}



template <>
StatementSet NextSolver::solve_next<StatementAst>(StatementAst *ast) {
    StatementSet result;
    
    StatementVisitorGenerator<NextSolver,
        SolveNextVisitorTraits> visitor(this);
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

    union_set(result, visitor.return_result());
    return result;
}

template <>
StatementSet NextSolver::solve_next<ConditionalAst>(ConditionalAst *ast) {
    StatementSet result;

    /*
     * The next statement executed after an if clause is either the 
     * then branch or else branch
     */
    result.insert(ast->get_then_branch());
    result.insert(ast->get_else_branch());

    return result;
}

template <>
StatementSet NextSolver::solve_next<WhileAst>(WhileAst *ast) {
    StatementSet result;

    /*
     * The next statement executed after a while clause is the first
     * body statement or the statement following the while statement.
     */
    result.insert(ast->get_body());

    if(ast->next()) {
        result.insert(ast->next());
    } else if(ast->get_parent()) {
        union_set(result, solve_container_next<ContainerAst>(ast->get_parent()));
    }
    
    return result;
}


template <>
StatementSet NextSolver::solve_next<AssignmentAst>(AssignmentAst *ast) {
    if(ast->next()) {
        StatementSet result;
        result.insert(ast->next());
        return result;
    } else if(ast->get_parent()) {
        return solve_container_next<ContainerAst>(ast->get_parent());
    } else {
        return StatementSet();
    }
}

template <>
StatementSet NextSolver::solve_next<CallAst>(CallAst *ast) {
    if(ast->next()) {
        StatementSet result;
        result.insert(ast->next());
        return result;
    } else if(ast->get_parent()) {
        return solve_container_next<ContainerAst>(ast->get_parent());
    } else {
        return StatementSet();
    }
}

class SolveNextContainerVisitor : public ContainerVisitor {
  public:
    SolveNextContainerVisitor(NextSolver *solver) : 
        _solver(solver), _result()
    { }

    void visit_conditional(ConditionalAst *container) {
        _result = _solver->solve_container_next<ConditionalAst>(container);
    }

    void visit_while(WhileAst *container) {
        _result = _solver->solve_container_next<WhileAst>(container);
    }

    StatementSet return_result() {
        return std::move(_result);
    }
  private:
    NextSolver *_solver;
    StatementSet _result;
};

template <>
StatementSet NextSolver::solve_container_next<ContainerAst>(ContainerAst *container) {
    SolveNextContainerVisitor visitor(this);
    container->accept_container_visitor(&visitor);
    return visitor.return_result();
}

template <>
StatementSet NextSolver::solve_container_next<ConditionalAst>(ConditionalAst *condition) {
    if(condition->next()) {
        StatementSet result;
        result.insert(condition->next());
        return result;
    } else if(condition->get_parent()){
        return solve_container_next<ContainerAst>(condition->get_parent());
    } else {
        return StatementSet();
    }
}

template <>
StatementSet NextSolver::solve_container_next<WhileAst>(WhileAst *loop) {
    StatementSet result;
    result.insert(loop);
    return result;
}

template <>
StatementSet NextSolver::solve_preceeding_previous<StatementAst>(StatementAst *ast) {
    StatementVisitorGenerator<NextSolver, SolvePreceedingPreviousVisitorTraits>
    visitor(this);

    ast->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
StatementSet NextSolver::solve_preceeding_previous<WhileAst>(WhileAst *ast) {
    StatementSet result;
    result.insert(ast);
    return result;
}

template <>
StatementSet NextSolver::solve_preceeding_previous<AssignmentAst>(AssignmentAst *ast) {
    StatementSet result;
    result.insert(ast);
    return result;
}

template <>
StatementSet NextSolver::solve_preceeding_previous<CallAst>(CallAst *ast) {
    StatementSet result;
    result.insert(ast);
    return result;
}

template <>
StatementSet NextSolver::solve_preceeding_previous<ConditionalAst>(ConditionalAst *ast) {
    StatementSet result;
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

    union_set(result, solve_preceeding_previous<StatementAst>(then_branch));
    union_set(result, solve_preceeding_previous<StatementAst>(else_branch));

    return result;
}

template <>
StatementSet NextSolver::solve_previous<StatementAst>(StatementAst *ast) {
    StatementSet result;
    
    /*
     * If the statement is not the first, then we look at it's previous
     * statement and see if the previous statement happen to be a conditional
     * statement, which becomes handy according to the Next() specification.
     */
    if(ast->prev()) {
        union_set(result, solve_preceeding_previous<StatementAst>(ast->prev()));
    /*
     * else the statement is the first statement and if it has a container,
     * then the container is the previous statement.
     */
    } else if(ast->get_parent()) {
        result.insert(ast->get_parent());
    }

    StatementVisitorGenerator<NextSolver,
        SolvePreviousVisitorTraits> visitor(this);
    ast->accept_statement_visitor(&visitor);

    union_set(result, visitor.return_result());

    return result;
}

template <>
StatementSet NextSolver::solve_previous<WhileAst>(WhileAst *ast) {
    StatementSet result;

    StatementAst *body = ast->get_body();
    while(body->next() != NULL) {
        body = body->next();
    }
    union_set(result, solve_last_previous<StatementAst>(body));

    return result;
}

template <>
StatementSet NextSolver::solve_last_previous<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<NextSolver, SolveLastPreviousVisitorTraits>
    visitor(this);
    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
StatementSet NextSolver::solve_last_previous<ConditionalAst>(ConditionalAst *condition) {

    StatementAst *then_branch = condition->get_then_branch();
    StatementAst *else_branch = condition->get_else_branch();

    while(then_branch->next() != NULL) {
        then_branch = then_branch->next();
    }

    while(else_branch->next() != NULL) {
        else_branch = else_branch->next();
    }

    StatementSet result = solve_last_previous<StatementAst>(then_branch);
    union_set(result, solve_last_previous<StatementAst>(else_branch));

    return result;
};

template <>
StatementSet NextSolver::solve_last_previous<WhileAst>(WhileAst *loop) {
    StatementSet result;
    result.insert(loop);
    return result;
}

template <>
StatementSet NextSolver::solve_last_previous<AssignmentAst>(AssignmentAst *assign) {
    StatementSet result;
    result.insert(assign);
    return result;
}


template <>
StatementSet NextSolver::solve_last_previous<CallAst>(CallAst *call) {
    StatementSet result;
    result.insert(call);
    return result;
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
bool NextSolver::validate_next<ConditionalAst>(ConditionalAst *condition, StatementAst *statement)
{
    return (condition->get_then_branch() == statement ||
            condition->get_else_branch() == statement);
}

template <>
bool NextSolver::validate_next<WhileAst>(WhileAst *loop, StatementAst *statement)
{
    if(loop->get_body() == statement || loop->next() == statement) {
        return true;
    } else if(!loop->next() && loop->get_parent()) {
        return validate_container_next<ContainerAst>(loop->get_parent(), statement);
    } else {
        return false;
    }
}

template <>
bool NextSolver::validate_next<AssignmentAst>(AssignmentAst *assign, StatementAst *statement)
{
    if(assign->next()) {
        return assign->next() == statement;
    } else if(assign->get_parent()) {
        return validate_container_next<ContainerAst>(assign->get_parent(), statement);
    } else {
        return false;
	}
}

template <>
bool NextSolver::validate_next<CallAst>(CallAst *call, StatementAst *statement)
{
    if(call->next()) {
        return call->next() == statement;
    } else if(call->get_parent()) {
        return validate_container_next<ContainerAst>(call->get_parent(), statement);
    } else {
        return false;
	}
}

class ValidateNextContainerVisitor : public ContainerVisitor {
  public:
    ValidateNextContainerVisitor(NextSolver *solver, StatementAst *statement) : 
        _solver(solver), _statement(statement), _result(false) 
    { }

    void visit_conditional(ConditionalAst *container) {
        _result = _solver->validate_container_next<ConditionalAst>(container, _statement);
    }

    void visit_while(WhileAst *container) {
        _result = _solver->validate_container_next<WhileAst>(container, _statement);
    }

    bool return_result() {
        return _result;
    }
  private:
    NextSolver *_solver;
    StatementAst *_statement;
    bool _result;
};

template <>
bool NextSolver::validate_container_next<ContainerAst>(ContainerAst *container, StatementAst *statement) {
    ValidateNextContainerVisitor visitor(this, statement);
    container->accept_container_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool NextSolver::validate_container_next<ConditionalAst>(ConditionalAst *container, StatementAst *statement) {
    if(container->next()) {
        return container->next() == statement;
    } else if(container->get_parent()) {
        // it is extremely tricky when we are in the last statement of
        // deeply nested containers.
        return validate_container_next<ContainerAst>(container->get_parent(), statement);
    } else {
        return false;
    }
}

template <>
bool NextSolver::validate_container_next<WhileAst>(WhileAst *container, StatementAst *statement) {
    // The next statement after the last statement in a loop
    // is the loop statement itself.
    return static_cast<StatementAst*>(container) == statement;
}






void ValidateNextStatementVisitor::visit_conditional(ConditionalAst *ast) {
    _result = _solver->validate_next<ConditionalAst>(ast, _statement);
}

void ValidateNextStatementVisitor::visit_while(WhileAst *ast) {
    _result = _solver->validate_next<WhileAst>(ast, _statement);
}

void ValidateNextStatementVisitor::visit_assignment(AssignmentAst *ast) {
    _result = _solver->validate_next<AssignmentAst>(ast, _statement);
}

void ValidateNextStatementVisitor::visit_call(CallAst *ast) {
    _result = _solver->validate_next<CallAst>(ast, _statement);
}

} // namespace impl
} // namespace simple
