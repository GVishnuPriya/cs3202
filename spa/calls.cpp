
#include "spa/calls.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

Calls::Calls(SolverPtr calls_solver, AST *ast) :
  _calls_solver(calls_solver), _ast(ast)
{ }

bool Calls::validate_calls(Proc p1, Proc p2) {
    SimpleProcCondition c1(_ast->get_proc(p1));
    SimpleProcCondition c2(_ast->get_proc(p2));
  
    return _calls_solver->validate(&c1, &c2);
}

ProcResults Calls::get_called(Proc p1) {
    SimpleProcCondition c1(_ast->get_proc(p1));
    
    ConditionSet result = _calls_solver->solve_right(&c1);
    return condition_to_proc_results(result);
}

ProcResults Calls::get_caller(Proc p2) {
    SimpleProcCondition c2(_ast->get_proc(p2));
    
    ConditionSet result = _calls_solver->solve_left(&c2);
    return condition_to_proc_results(result);
}

}