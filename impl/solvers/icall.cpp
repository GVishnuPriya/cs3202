
#include "impl/solvers/icall.h"
#include "impl/condition.h"
#include "simple/util/statement_visitor_generator.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::impl;

ICallSolver::ICallSolver(SimpleRoot ast) : 
    _ast(ast) 
{ 
    for(SimpleRoot::iterator it = _ast.begin(); 
        it != _ast.end(); ++it)
    {
        index_calls<ProcAst>(*it);
    }
}

template <>
ConditionSet ICallSolver::solve_right<ProcAst>(ProcAst *proc) {
    if(_calls_table.count(proc)) {
        ConditionSet result;
        for(std::set<ProcAst*>::iterator it = _calls_table[proc].begin();
                it != _calls_table[proc].end(); ++it)
        {
            result.insert(new SimpleProcCondition(*it));
        }
        return result;
    } else {
        return ConditionSet();
    }
}

template <>
ConditionSet ICallSolver::solve_left<ProcAst>(ProcAst *proc) {
    if(_called_table.count(proc)) {
        ConditionSet result;
        for(std::set<ProcAst*>::iterator it = _called_table[proc].begin();
                it != _called_table[proc].end(); ++it)
        {
            result.insert(new SimpleProcCondition(*it));
        }
        return result;
    } else {
        return ConditionSet();
    }
}

template <>
bool ICallSolver::validate<ProcAst, ProcAst>(ProcAst *proc1, ProcAst *proc2)
{
    if(_calls_table.count(proc1)) {
        return _calls_table[proc1].count(proc2) > 0;
    } else {
        return false;
    }
}

class IndexCallsVisitorTraits {
  public:
    typedef bool ResultType;
    typedef int   ContextType;

    template <typename Ast>
    static bool visit(ICallSolver *solver, Ast *ast, int *context = NULL) 
    {
        solver->template index_calls<Ast>(ast);
        return true;
    }
  private:
    IndexCallsVisitorTraits();
};

template <>
void ICallSolver::index_calls<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<ICallSolver, IndexCallsVisitorTraits>
    visitor(this);

    statement->accept_statement_visitor(&visitor);
}

template <>
void ICallSolver::index_calls<ProcAst>(ProcAst *proc) {
    // initialize the calls table to avoid infinite recursion
    _calls_table[proc]; 

    StatementAst *statement = proc->get_statement();
    while(statement != NULL) {
        index_calls<StatementAst>(statement);
        statement = statement->next();
    }
}

template <>
void ICallSolver::index_calls<CallAst>(CallAst *call) {
    ProcAst *caller = call->get_proc();
    ProcAst *callee = call->get_proc_called();

    _calls_table[caller].insert(callee);
    _called_table[callee].insert(caller);

    if(_calls_table.count(callee) == 0) {
        index_calls<ProcAst>(callee);
    }

    for(std::set<ProcAst*>::iterator it = _calls_table[callee].begin();
            it != _calls_table[callee].end(); ++it)
    {
        _calls_table[caller].insert(*it);
        _called_table[*it].insert(caller);
    }
}

template <>
void ICallSolver::index_calls<WhileAst>(WhileAst *loop) {
    StatementAst *statement = loop->get_body();
    while(statement != NULL) {
        index_calls<StatementAst>(statement);
        statement = statement->next();
    }
}

template <>
void ICallSolver::index_calls<ConditionalAst>(ConditionalAst *condition) {
    StatementAst *statement = condition->get_then_branch();
    while(statement != NULL) {
        index_calls<StatementAst>(statement);
        statement = statement->next();
    }

    statement = condition->get_else_branch();
    while(statement != NULL) {
        index_calls<StatementAst>(statement);
        statement = statement->next();
    }
}



} // namespace impl
} // namespace simple
