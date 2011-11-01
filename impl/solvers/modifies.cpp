

#include "simple/util/statement_visitor_generator.h"
#include "impl/solvers/modifies.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

/*
 * Helper classes
 */
class ModifiesValidateStatementVisitor : public StatementVisitor {
  public:
    ModifiesValidateStatementVisitor(
        ModifiesSolver *solver, SimpleVariable *var) : 
        _solver(solver), _var(var) { }

    void visit_conditional(ConditionalAst *ast) {
        _result = _solver->validate<ConditionalAst, SimpleVariable>(ast, _var);
    }

    void visit_while(WhileAst *ast) {
        _result = _solver->validate<WhileAst, SimpleVariable>(ast, _var);
    }

    void visit_assignment(AssignmentAst *ast) {
        _result = _solver->validate<AssignmentAst, SimpleVariable>(ast, _var);
    }

    void visit_call(CallAst *ast) {
        _result = _solver->validate<CallAst, SimpleVariable>(ast, _var);
    }

    bool return_result() {
        return _result;
    }

  private:
    ModifiesSolver *_solver;
    SimpleVariable *_var;
    bool _result;
};

class ModifiesVariableVisitorAdapter {
  public:
    ModifiesVariableVisitorAdapter(ModifiesSolver *solver, SimpleVariable *var) :
        _solver(solver), _var(var)
    { }

    template <typename Ast>
    ConditionSet solve_variable(Ast *ast) {
        return _solver->solve_variable<Ast>(ast, _var);
    }
  private:
    ModifiesSolver *_solver;
    SimpleVariable *_var;
};


ModifiesSolver::ModifiesSolver(const SimpleRoot& ast, SolverTable *table) : 
    _ast(ast) 
{
   for(SimpleRoot::iterator it = _ast.begin(); it != _ast.end(); ++it) {
       index_variables<ProcAst>(*it);
   } 
}

/*
 * validate() definitions
 */
template <>
bool ModifiesSolver::validate<StatementAst, SimpleVariable>(
        StatementAst *ast, SimpleVariable *var) 
{
    ModifiesValidateStatementVisitor visitor(this, var);
    ast->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool ModifiesSolver::validate<ProcAst, SimpleVariable>(
        ProcAst *ast, SimpleVariable *var)
{
    StatementAst *body = ast->get_statement();
    while(body != NULL) {
        if(validate<StatementAst, SimpleVariable>(body, var)) {
            return true;
        }
        body = body->next();
    }

    return false;
}

template <>
bool ModifiesSolver::validate<AssignmentAst, SimpleVariable>(
        AssignmentAst *ast, SimpleVariable *var) 
{
    SimpleVariable *v = ast->get_variable();
    return v->equals(*var);
}

template <>
bool ModifiesSolver::validate<ConditionalAst, SimpleVariable>(
        ConditionalAst *ast, SimpleVariable *var) 
{
    StatementAst *then_branch = ast->get_then_branch();
    while(then_branch != NULL) {
        if(validate<StatementAst, SimpleVariable>(then_branch, var)) {
            return true;
        }
        then_branch = then_branch->next();
    }

    StatementAst *else_branch = ast->get_else_branch();
    while(else_branch != NULL) {
        if(validate<StatementAst, SimpleVariable>(else_branch, var)) {
            return true;
        }
        else_branch = else_branch->next();
    }

    return false;
}

template <>
bool ModifiesSolver::validate<WhileAst, SimpleVariable>(
        WhileAst *ast, SimpleVariable *var)
{
    StatementAst *body = ast->get_body();
    while(body != NULL) {
        if(validate<StatementAst, SimpleVariable>(body, var)) {
            return true;
        }
        body = body->next();
    }

    return false;
}

template <>
bool ModifiesSolver::validate<CallAst, SimpleVariable>(
        CallAst *ast, SimpleVariable *var)
{
    return validate<ProcAst, SimpleVariable>(ast->get_proc_called(), var);
}


/*
 * solve_right() definitions
 */
template <>
ConditionSet ModifiesSolver::solve_right<StatementAst>(StatementAst *ast) {
    StatementVisitorGenerator<ModifiesSolver, 
        SolveRightVisitorTraits<ModifiesSolver> > visitor(this);

    ast->accept_statement_visitor(&visitor);

    return visitor.return_result();
}

template <>
ConditionSet ModifiesSolver::solve_right<ConditionalAst>(ConditionalAst *ast) {
    ConditionSet result;
    
    StatementAst *then = ast->get_then_branch();
    while(then != NULL) {
        result.union_with(solve_right<StatementAst>(then));
        then = then->next();
    }

    StatementAst *el = ast->get_else_branch();
    while(el != NULL) {
        result.union_with(solve_right<StatementAst>(el));
        el = el->next();
    }

    return result;
}

template <>
ConditionSet ModifiesSolver::solve_right<WhileAst>(WhileAst *ast) {
    ConditionSet result;

    StatementAst *body = ast->get_body();
    while(body != NULL) {
        result.union_with(solve_right<StatementAst>(body));
        body = body->next();
    }

    return result;
}

template <>
ConditionSet ModifiesSolver::solve_right<ProcAst>(ProcAst *ast) {
    ConditionSet result;
    
    StatementAst *body = ast->get_statement();
    while(body != NULL) {
        result.union_with(solve_right<StatementAst>(body));
        body = body->next();
    }

    return result;
}

template <>
ConditionSet ModifiesSolver::solve_right<AssignmentAst>(AssignmentAst *ast) {
    ConditionSet result;
    result.insert(new SimpleVariableCondition(
            *(ast->get_variable())));
    return result;
}

template <>
ConditionSet ModifiesSolver::solve_right<CallAst>(CallAst *ast) {
    return solve_right<ProcAst>(ast->get_proc_called());
}

template <>
ConditionSet ModifiesSolver::solve_variable<AssignmentAst>(
        AssignmentAst *ast, SimpleVariable *variable) 
{
    if(validate<AssignmentAst, SimpleVariable>(ast, variable)) {
        ConditionSet result;
        result.insert(new SimpleStatementCondition(ast));
        return result;
    } else {
        return ConditionSet(); // empty set
    }
}

/*
 * solve_left() definitions
 */
template <>
ConditionSet ModifiesSolver::solve_left<SimpleVariable>(SimpleVariable *variable) {
    if(_var_index.count(*variable)) {
        return _var_index[*variable];
    } else {
        return ConditionSet();
    }
}

/*
 * solve_variable() definitions
 */
template <>
ConditionSet ModifiesSolver::solve_variable<CallAst>(CallAst *ast, SimpleVariable *variable) {
    if(validate<CallAst, SimpleVariable>(ast, variable)) {
        ConditionSet result;
        result.insert(new SimpleStatementCondition(ast));
        return result;
    } else {
        return ConditionSet(); // empty set
    }
}

template <>
ConditionSet ModifiesSolver::solve_variable<ProcAst>(ProcAst *ast, SimpleVariable *variable) {
    ConditionSet result;
    StatementAst *statements = ast->get_statement();

    while(statements != NULL) {
        result.union_with(solve_variable<StatementAst>(statements, variable));
        statements = statements->next();
    }

    if(!result.is_empty()) {
        result.insert(new SimpleProcCondition(ast));
    }

    return result;
}

template <>
ConditionSet ModifiesSolver::solve_variable<StatementAst>(
        StatementAst *ast, SimpleVariable *variable) 
{
    ModifiesVariableVisitorAdapter adapter(this, variable);
    
    StatementVisitorGenerator<
        ModifiesVariableVisitorAdapter,
        SolveVariableVisitorTraits<
            ModifiesVariableVisitorAdapter> > 
    visitor(&adapter);
    
    ast->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

/*
 * index_variable()
 */
void ModifiesSolver::index_statement_list(
        StatementAst *statement, ConditionPtr condition, 
        std::set<SimpleVariable>& result) 
{
    while(statement != NULL) {
        std::set<SimpleVariable> current_result = index_variables<StatementAst>(statement);
        for(std::set<SimpleVariable>::iterator it = current_result.begin(); 
                it != current_result.end(); ++it) 
        {
            if(result.count(*it) == 0) {
                _var_index[*it].insert(condition);
                result.insert(*it);
            }
        }
        statement = statement->next();
    }
}

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<ProcAst>(ProcAst *proc) {
    std::set<SimpleVariable> result;

    index_statement_list(proc->get_statement(), new SimpleProcCondition(proc), result);

    return result;
}

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<AssignmentAst>(AssignmentAst *assign) {
    _var_index[*assign->get_variable()].insert(new SimpleStatementCondition(assign));
    std::set<SimpleVariable> result;
    result.insert(*assign->get_variable());
    return result;
}

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<WhileAst>(WhileAst *ast) {
    std::set<SimpleVariable> result;

    index_statement_list(ast->get_body(), new SimpleStatementCondition(ast), result);

    return result;
}

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<ConditionalAst>(ConditionalAst *ast) {
    std::set<SimpleVariable> result;
    ConditionPtr condition = new SimpleStatementCondition(ast);

    index_statement_list(ast->get_then_branch(), condition, result);
    index_statement_list(ast->get_else_branch(), condition, result);

    return result;
}

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<CallAst>(CallAst *ast) {
    std::set<SimpleVariable> result = index_variables<ProcAst>(ast->get_proc_called());
    ConditionPtr this_condition = new SimpleStatementCondition(ast);

    for(std::set<SimpleVariable>::iterator it = result.begin();
            it != result.end(); ++it)
    {
        _var_index[*it].insert(this_condition);
    }

    return result;
}

template <>
std::set<SimpleVariable> ModifiesSolver::index_variables<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<ModifiesSolver,
        IndexVariableVisitorTraits<ModifiesSolver> > 
    visitor(this);

    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}




} // namespace impl
} // namespace simple
