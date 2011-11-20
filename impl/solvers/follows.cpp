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

#include "impl/solvers/follows.h"


namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;


template <>
ConditionSet FollowSolver::solve_right<StatementAst>(StatementAst *ast) {
    ConditionSet result;

    if(ast->next()) {
        result.insert(new SimpleStatementCondition(ast->next()));
    }
    return result;
}
    
template <>
ConditionSet FollowSolver::solve_left<StatementAst>(StatementAst *ast) {
    ConditionSet result;

    if(ast->prev()) {
        result.insert(new SimpleStatementCondition(ast->prev()));
    }
    return result;
}

template <>
bool FollowSolver::validate<StatementAst, StatementAst>(
        StatementAst *left, StatementAst *right)
{
    return left->next() == right;
}



}
}
