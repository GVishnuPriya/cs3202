
#include "simple/util/set_convert.h"
#include "impl/solvers/next_bip.h"

namespace simple {
namespace impl {

bool is_first_statement(StatementAst *statement) {
    StatementAst *prev = statement->prev();
    StatementAst *parent = statement->get_parent();

    if(!prev && !parent) return true;
    return false;
}

bool is_last_statement(StatementAst *statement);

bool is_last_container_statement(StatementAst *statement) {
    StatementAst *next = statement->next();
    StatementAst *parent = statement->get_parent();

    if(!next && !parent) return true;
    if(!next && parent) return is_last_container_statement(parent);

    return false;
}

bool is_last_statement(StatementAst *statement) {
    // If statement can never be the last statement
    IfAst *if_ast = statement_cast<IfAst>(statement);
    if(if_ast) return false;

    return is_last_container_statement(statement);
}

NextBipSolver::NextBipSolver(SimpleRoot ast, 
    std::shared_ptr<NextQuerySolver> next_solver, 
    std::shared_ptr<CallsQuerySolver> calls_solver) :
    _ast(ast), _next_solver(next_solver),
    _calls_solver(calls_solver)
{ 
    for(auto it=ast.begin(); it!=ast.end(); ++it) {
        index_last_proc_statement(*it);
    }
}

StackedStatementSet NextBipSolver::solve_next_bip(
    StatementAst *statement, CallStack callstack) 
{
    StatementSet next_result = _next_solver->solve_next_statement(statement);

    StackedStatementSet result;

    for(auto it = next_result.begin(); it != next_result.end(); ++it) {
        result.insert(StackedStatement(*it, callstack));
    }

    if(!is_last_statement(statement)) return result;

    if(callstack.empty()) {
        CallSet call_statements = _calls_solver->solve_calling_statements(
            statement->get_proc());

        for(auto it=call_statements.begin(); it!=call_statements.end(); ++it) {
            CallAst *call_statement = *it;

            union_set(result, solve_next_bip(call_statement, callstack));
        }
    } else {
        CallAst* prev_call = callstack.top();
        callstack.pop();

        union_set(result, solve_next_bip(prev_call, callstack));
    }

    return result;
}

StackedStatementSet NextBipSolver::solve_next_bip_statement(
    StatementAst *statement, CallStack callstack) 
{
    CallAst *calls = statement_cast<CallAst>(statement);

    if(!calls) return solve_next_bip(statement, callstack);

    callstack.push(calls);
    StatementAst *result_statement = calls->get_proc_called()->get_statement();

    StackedStatementSet result;
    result.insert(StackedStatement(result_statement, callstack));

    return result;
}

StackedStatementSet NextBipSolver::solve_prev_bip_statement(
    StatementAst *statement, CallStack callstack) 
{
    StatementSet next_result = _next_solver->solve_prev_statement(statement);

    StackedStatementSet result;
    
    for(auto it = next_result.begin(); it != next_result.end(); ++it) {
        StatementAst *next_statement = *it;
        CallAst *calls = statement_cast<CallAst>(next_statement);

        if(!calls) {
            result.insert(StackedStatement(*it, callstack));
        } else {
            CallStack current_stack(callstack);

            StatementSet last_statements = _last_statement_index[calls->get_proc_called()];
            for(auto it2 = last_statements.begin(); it2 != last_statements.end(); ++it2) {
                result.insert(StackedStatement(*it2, current_stack));
            }
        }
    }

    if(!is_first_statement(statement)) return result;

    if(callstack.empty()) {
        CallSet call_statements = _calls_solver->solve_calling_statements(
            statement->get_proc());

        for(auto it=call_statements.begin(); it!=call_statements.end(); ++it) {
            CallAst *call_statement = *it;
            result.insert(StackedStatement(call_statement, CallStack()));
        }
    } else {
        CallAst *call_statement = callstack.top();
        callstack.pop();

        result.insert(StackedStatement(call_statement, callstack));
    }

    return result;
}

bool NextBipSolver::is_bip() {
    return true;
}

StatementSet NextBipSolver::solve_next_statement(StatementAst *statement) {
    CallStack callstack;

    return to_statement_set(solve_next_bip_statement(statement, callstack));
}

StatementSet NextBipSolver::solve_prev_statement(StatementAst *statement) {
    CallStack callstack;

    return to_statement_set(solve_prev_bip_statement(statement, callstack));
}


void NextBipSolver::index_last_proc_statement(ProcAst *proc) {
    _last_statement_index[proc] = last_statements_in_list(proc->get_statement());
}

StatementSet NextBipSolver::last_statements_in_list(StatementAst *statement) {
    while(statement->next() != NULL) {
        statement = statement->next();
    }

    return last_statements_in_statement(statement);
}

StatementSet NextBipSolver::last_statements_in_statement(StatementAst *statement) {
    switch(get_statement_type(statement)) {
        case WhileST:
            return last_statements_in_while(statement_cast<WhileAst>(statement));
        break;
        case IfST:
            return last_statements_in_if(statement_cast<IfAst>(statement));
        break;
        case AssignST:
            return last_statements_in_assign(statement_cast<AssignmentAst>(statement));
        break;
        case CallST:
            return last_statements_in_call(statement_cast<CallAst>(statement));
        break;
        default:
            return StatementSet();
        break;
    }
}

StatementSet NextBipSolver::last_statements_in_while(WhileAst *ast) {
    StatementSet result = last_statements_in_statement(ast->get_body());
    result.insert(ast);

    return result;
}

StatementSet NextBipSolver::last_statements_in_if(IfAst *ast) {
    StatementSet then_result = last_statements_in_statement(ast->get_then_branch());
    StatementSet else_result = last_statements_in_statement(ast->get_else_branch());

    union_set(then_result, else_result);
    return then_result;
}

StatementSet NextBipSolver::last_statements_in_assign(AssignmentAst *assign) {
    StatementSet result;
    result.insert(assign);
    return result;
}

StatementSet NextBipSolver::last_statements_in_call(CallAst *call) {
    StatementSet result;
    result.insert(call);
    return result;
}

template <>
ConditionSet NextBipSolver::solve_right<StatementAst>(StatementAst *statement) {
    StatementSet result = solve_next_statement(statement);
    return statement_set_to_condition_set(result);
}

template <>
ConditionSet NextBipSolver::solve_left<StatementAst>(StatementAst *statement) {
    StatementSet result = solve_prev_statement(statement);
    return statement_set_to_condition_set(result);
}

template <>
bool NextBipSolver::validate<StatementAst, StatementAst>(
        StatementAst *statement1, StatementAst *statement2)
{
    return solve_next_statement(statement1).count(statement2) > 0;
}

}
}