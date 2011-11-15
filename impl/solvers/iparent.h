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

#pragma once

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::impl;

class IParentSolver {
  public:
    IParentSolver(SimpleRoot ast) : _ast(ast) { }

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

  private:
    SimpleRoot _ast;
};


template <>
ConditionSet IParentSolver::solve_right<StatementAst>(StatementAst *statement) {
    StatementVisitorGenerator<IParentSolver, 
        SolveRightVisitorTraits<IParentSolver> > visitor(this);
    statement->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
ConditionSet IParentSolver::solve_right<WhileAst>(WhileAst *loop) {
    ConditionSet result;
    StatementAst *body = loop->get_body();

    while(body != NULL) {
        result.insert(new SimpleStatementCondition(body));
        result.union_with(solve_right<StatementAst>(body));
        body = body->next();
    }

    return result;
}

template <>
ConditionSet IParentSolver::solve_right<ConditionalAst>(ConditionalAst *condition) {
    ConditionSet result;
    StatementAst *then_branch = condition->get_then_branch();
    StatementAst *else_branch = condition->get_else_branch();

    while(then_branch != NULL) {
        result.insert(new SimpleStatementCondition(then_branch));
        result.union_with(solve_right<StatementAst>(then_branch));
        then_branch = then_branch->next();
    }

    while(else_branch != NULL) {
        result.insert(new SimpleStatementCondition(else_branch));
        result.union_with(solve_right<StatementAst>(else_branch));
        else_branch = else_branch->next();
    }

    return result;
}
    
template <>
ConditionSet IParentSolver::solve_left<StatementAst>(StatementAst *statement) {
    ConditionSet result;

    while(statement->get_parent() != NULL) {
        result.insert(new SimpleStatementCondition(statement->get_parent()));
        statement = statement->get_parent();
    }
    return result;
}

template <>
bool IParentSolver::validate<StatementAst, StatementAst>(
        StatementAst *left, StatementAst *right)
{
    StatementVisitorGenerator<IParentSolver, 
        PartialValidateVisitorTraits<IParentSolver> > visitor(this, right);
    left->accept_statement_visitor(&visitor);
    return visitor.return_result();
}

template <>
bool IParentSolver::validate<ContainerAst, StatementAst>(
        ContainerAst *container, StatementAst *statement)
{
    ContainerAst *parent = statement->get_parent();
    while(parent != NULL) {
        if(parent == container) {
            return true;
        }
        parent = parent->get_parent();
    }
    return false;
}

template <>
bool IParentSolver::validate<ConditionalAst, StatementAst>(
        ConditionalAst *condition, StatementAst *statement)
{
    return validate<ContainerAst, StatementAst>(condition, statement);
}

template <>
bool IParentSolver::validate<WhileAst, StatementAst>(
        WhileAst *loop, StatementAst *statement)
{
    return validate<ContainerAst, StatementAst>(loop, statement);
}


} // namespace impl
} // namespace simple
