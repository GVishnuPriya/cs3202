
#include "spa/calls_star.h"

namespace spa {

using namespace simple;
using namespace simple::impl;

CallsStar::CallsStar(SolverPtr calls_star_solver, AST *ast) :
  _calls_star_solver(calls_star_solver), _ast(ast)
{ }

bool CallsStar::validate_calls_star(Proc p1, Proc p2) {
    SimpleProcCondition c1(_ast->get_proc(p1));
    SimpleProcCondition c2(_ast->get_proc(p2));
  
    return _calls_star_solver->validate(&c1, &c2);
}

ProcResults CallsStar::get_called_star(Proc p1) {
    SimpleProcCondition c1(_ast->get_proc(p1));
    
    ConditionSet result = _calls_star_solver->solve_right(&c1);
    return condition_to_proc_results(result);
}

ProcResults CallsStar::get_caller_star(Proc p2) {
    SimpleProcCondition c2(_ast->get_proc(p2));
    
    ConditionSet result = _calls_star_solver->solve_left(&c2);
    return condition_to_proc_results(result);
}

}