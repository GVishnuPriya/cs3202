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

#include "impl/solvers/call.h"
#include "impl/condition.h"
#include "simple/util/statement_visitor_generator.h"
#include "simple/util/set_convert.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::impl;

CallSolver::CallSolver(SimpleRoot ast) : 
    _ast(ast) 
{ 
    for(SimpleRoot::iterator it = _ast.begin(); 
        it != _ast.end(); ++it)
    {
        index_calls<ProcAst>(*it);
    }
}

template <>
ConditionSet CallSolver::solve_right<ProcAst>(ProcAst *proc) {
    ProcSet result = solve_called_procs(proc);
    return proc_set_to_condition_set(result);
}

template <>
ConditionSet CallSolver::solve_left<ProcAst>(ProcAst *proc) {
    ProcSet result = solve_calling_procs(proc);
    return proc_set_to_condition_set(result);
}

template <>
bool CallSolver::validate<ProcAst, ProcAst>(ProcAst *proc1, ProcAst *proc2)
{
    return solve_called_procs(proc1).count(proc2);
}

ProcSet CallSolver::solve_called_procs(ProcAst *calling_proc) {
    return _called_table[calling_proc];
}

ProcSet CallSolver::solve_calling_procs(ProcAst *called_proc) {
    return _calling_table[called_proc];
}

CallSet CallSolver::solve_calling_statements(ProcAst *called_proc) {
    return _calling_statements[called_proc];
}

class IndexCallsVisitorTraits {
  public:
    typedef bool ResultType;
    typedef int   ContextType;

    template <typename Ast>
    static bool visit(CallSolver *solver, Ast *ast, int *context = NULL) 
    {
        solver->template index_calls<Ast>(ast);
        return true;
    }
  private:
    IndexCallsVisitorTraits();
};

template <>
void CallSolver::index_calls<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<CallSolver, IndexCallsVisitorTraits>
    visitor(this);

    statement->accept_statement_visitor(&visitor);
}

template <>
void CallSolver::index_calls<ProcAst>(ProcAst *proc) {
    StatementAst *statement = proc->get_statement();
    while(statement != NULL) {
        index_calls<StatementAst>(statement);
        statement = statement->next();
    }
}

template <>
void CallSolver::index_calls<CallAst>(CallAst *call) {
    ProcAst *caller = call->get_proc();
    ProcAst *callee = call->get_proc_called();

    _called_table[caller].insert(callee);
    _calling_table[callee].insert(caller);
    _calling_statements[callee].insert(call);
}

template <>
void CallSolver::index_calls<WhileAst>(WhileAst *loop) {
    StatementAst *statement = loop->get_body();
    while(statement != NULL) {
        index_calls<StatementAst>(statement);
        statement = statement->next();
    }
}

template <>
void CallSolver::index_calls<IfAst>(IfAst *condition) {
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
