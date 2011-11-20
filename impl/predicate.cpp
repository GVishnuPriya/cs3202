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

#include "impl/predicate.h"

namespace simple {
namespace impl {

template <typename Predicate>
PredicateGenerator<Predicate>::PredicateGenerator(SimpleRoot ast, Predicate *pred) : 
    _ast(ast), _global_set(), _pred(pred)
{
    create_global_set();
}

template <typename Predicate>
const ConditionSet& PredicateGenerator<Predicate>::global_set() {
    return _global_set;
}


template <typename Predicate>
void PredicateGenerator<Predicate>::filter(ConditionSet& conditions) {
    conditions.intersect_with(_global_set);
}

template <typename Predicate>
bool PredicateGenerator<Predicate>::validate(ConditionPtr condition) {
    return _global_set.has_element(condition);
}

template <typename Predicate>
std::string PredicateGenerator<Predicate>::get_name() {
    return Predicate::get_name();
}

template <typename Predicate>
std::string PredicateGenerator<Predicate>::get_predicate_name() {
    return get_name();
}


template <typename Predicate>
void PredicateGenerator<Predicate>::visit_assignment(AssignmentAst *assign) {
    if(_pred->template evaluate<AssignmentAst>(assign)) {
        _global_set.insert(new SimpleStatementCondition(assign));
    }

    if(_pred->template evaluate<SimpleVariable>(assign->get_variable())) {
        _global_set.insert(new SimpleVariableCondition(*assign->get_variable()));
    }

    if(_pred->template evaluate<ExprAst>(assign->get_expr())) {
        _global_set.insert(new SimplePatternCondition(assign->get_expr()->clone()));
    }

    assign->get_expr()->accept_expr_visitor(this);
}

template <typename Predicate>
void PredicateGenerator<Predicate>::visit_conditional(ConditionalAst *condition) {
    if(_pred->template evaluate<ConditionalAst>(condition)) {
        _global_set.insert(new SimpleStatementCondition(condition));
    }

    if(_pred->template evaluate<SimpleVariable>(condition->get_variable())) {
        _global_set.insert(new SimpleVariableCondition(*condition->get_variable()));
    }

    filter_statement_list(condition->get_then_branch());
    filter_statement_list(condition->get_else_branch());
}

template <typename Predicate>
void PredicateGenerator<Predicate>::visit_while(WhileAst *loop) {
    if(_pred->template evaluate<WhileAst>(loop)) {
        _global_set.insert(new SimpleStatementCondition(loop));
    }

    if(_pred->template evaluate<SimpleVariable>(loop->get_variable())) {
        _global_set.insert(new SimpleVariableCondition(*loop->get_variable()));
    }

    filter_statement_list(loop->get_body());
}

template <typename Predicate>
void PredicateGenerator<Predicate>::visit_call(CallAst *call) {
    if(_pred->template evaluate<CallAst>(call)) {
        _global_set.insert(new SimpleStatementCondition(call));
    }
}

template <typename Predicate>
void PredicateGenerator<Predicate>::visit_variable(VariableAst *var) {
    if(_pred->template evaluate<SimpleVariable>(var->get_variable())) {
        _global_set.insert(new SimpleVariableCondition(*var->get_variable()));
    }
}

template <typename Predicate>
void PredicateGenerator<Predicate>::visit_const(ConstAst *constant) {
    if(_pred->template evaluate<SimpleConstant>(constant->get_constant())) {
        _global_set.insert(new SimpleConstantCondition(*constant->get_constant()));
    }
}

template <typename Predicate>
void PredicateGenerator<Predicate>::visit_binary_op(BinaryOpAst *bin) {
    bin->get_lhs()->accept_expr_visitor(this);
    bin->get_rhs()->accept_expr_visitor(this);
}

template <typename Predicate>
void PredicateGenerator<Predicate>::create_global_set() {
    for(SimpleRoot::iterator it = _ast.begin();
        it != _ast.end(); ++it)
    {
        ProcAst *proc = *it;
        if(_pred->template evaluate<ProcAst>(proc)) {
            _global_set.insert(new SimpleProcCondition(proc));
        }

        filter_statement_list(proc->get_statement());
    }
}

template <typename Predicate>
void PredicateGenerator<Predicate>::filter_statement_list(StatementAst *statement) {
    while(statement != NULL) {
        statement->accept_statement_visitor(this);
        statement = statement->next();
    }
}

WildCardPredicate::WildCardPredicate() { }

template <typename Condition>
bool WildCardPredicate::evaluate(Condition *condition) {
    return true;
}

std::string WildCardPredicate::get_name() {
    return "wildcard";
}

ProcPredicate::ProcPredicate() { }

template <typename Condition>
bool ProcPredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool ProcPredicate::evaluate<ProcAst>(ProcAst *proc) {
    return true;
}

std::string ProcPredicate::get_name() {
    return "procedure";
}

StatementPredicate::StatementPredicate() { }

template <typename Condition>
bool StatementPredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool StatementPredicate::evaluate<AssignmentAst>(AssignmentAst *assign) {
    return true;
}

template <>
bool StatementPredicate::evaluate<WhileAst>(WhileAst *loop) {
    return true;
}

template <>
bool StatementPredicate::evaluate<ConditionalAst>(ConditionalAst *condition) {
    return true;
}

template <>
bool StatementPredicate::evaluate<CallAst>(CallAst *call) {
    return true;
}

std::string StatementPredicate::get_name() {
    return "statement";
}

AssignPredicate::AssignPredicate() { }

template <typename Condition>
bool AssignPredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool AssignPredicate::evaluate<AssignmentAst>(AssignmentAst *assign) {
    return true;
}

std::string AssignPredicate::get_name() {
    return "assign";
}

ConditionalPredicate::ConditionalPredicate() { }

template <typename Condition>
bool ConditionalPredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool ConditionalPredicate::evaluate<ConditionalAst>(ConditionalAst *condition) {
    return true;
}

std::string ConditionalPredicate::get_name() {
    return "if";
}

WhilePredicate::WhilePredicate() { }

template <typename Condition>
bool WhilePredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool WhilePredicate::evaluate<WhileAst>(WhileAst *loop) {
    return true;
}

std::string WhilePredicate::get_name() {
    return "while";
}

CallPredicate::CallPredicate() { }

template <typename Condition>
bool CallPredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool CallPredicate::evaluate<CallAst>(CallAst *call) {
    return true;
}

std::string CallPredicate::get_name() {
    return "call";
}

VariablePredicate::VariablePredicate() { }

template <typename Condition>
bool VariablePredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool VariablePredicate::evaluate<SimpleVariable>(SimpleVariable *variable) {
    return true;
}

std::string VariablePredicate::get_name() {
    return "variable";
}

ConstantPredicate::ConstantPredicate() { }

template <typename Condition>
bool ConstantPredicate::evaluate(Condition *condition) {
    return false;
}

template <>
bool ConstantPredicate::evaluate<SimpleConstant>(SimpleConstant *constant) {
    return true;
}

std::string ConstantPredicate::get_name() {
    return "constant";
}

template class PredicateGenerator<WildCardPredicate>;
template class PredicateGenerator<ProcPredicate>;
template class PredicateGenerator<StatementPredicate>;
template class PredicateGenerator<AssignPredicate>;
template class PredicateGenerator<WhilePredicate>;
template class PredicateGenerator<ConditionalPredicate>;
template class PredicateGenerator<CallPredicate>;
template class PredicateGenerator<VariablePredicate>;
template class PredicateGenerator<ConstantPredicate>;





} // namespace impl
} // namespace simple
