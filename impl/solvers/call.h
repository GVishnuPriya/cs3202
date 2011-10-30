
#pragma once

#include <map>
#include <set>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::impl;

class CallSolver {
  public:
    CallSolver(SimpleRoot ast, SolverTable *table);

    /*
     * SOLVE RIGHT PART
     */
    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2) {
        return false;
    }

    template <typename Condition>
    void index_calls(Condition *condition) {
        // no-op
    }


  private:
    typedef std::map<std::string, std::set<ProcAst*> > ProcIndex;
    SimpleRoot _ast;
    ProcIndex _calls_table;
    ProcIndex _called_table;
};

template <>
ConditionSet CallSolver::solve_right<ProcAst>(ProcAst *proc);

template <>
ConditionSet CallSolver::solve_left<ProcAst>(ProcAst *proc);

template <>
bool CallSolver::validate<ProcAst, ProcAst>(ProcAst *proc1, ProcAst *proc2);

template <>
void CallSolver::index_calls<StatementAst>(StatementAst *statement);

template <>
void CallSolver::index_calls<ProcAst>(ProcAst *proc);

template <>
void CallSolver::index_calls<CallAst>(CallAst *call);

template <>
void CallSolver::index_calls<WhileAst>(WhileAst *loop);

template <>
void CallSolver::index_calls<ConditionalAst>(ConditionalAst *condition);





} // namespace impl
} // namespace simple
