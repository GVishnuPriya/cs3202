
#pragma once

#include <map>
#include <set>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"

namespace simple {
namespace impl {

using namespace simple;

class ICallSolver {
  public:
    ICallSolver(SimpleRoot ast, SolverTable *table);

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
    typedef std::map<ProcAst*, std::set<ProcAst*> > ProcIndex;
    SimpleRoot _ast;
    ProcIndex _calls_table;
    ProcIndex _called_table;
};

template <>
ConditionSet ICallSolver::solve_right<ProcAst>(ProcAst *proc);

template <>
ConditionSet ICallSolver::solve_left<ProcAst>(ProcAst *proc);

template <>
bool ICallSolver::validate<ProcAst, ProcAst>(ProcAst *proc1, ProcAst *proc2);

template <>
void ICallSolver::index_calls<StatementAst>(StatementAst *statement);

template <>
void ICallSolver::index_calls<ProcAst>(ProcAst *proc);

template <>
void ICallSolver::index_calls<CallAst>(CallAst *call);

template <>
void ICallSolver::index_calls<WhileAst>(WhileAst *loop);

template <>
void ICallSolver::index_calls<ConditionalAst>(ConditionalAst *condition);





} // namespace impl
} // namespace simple
