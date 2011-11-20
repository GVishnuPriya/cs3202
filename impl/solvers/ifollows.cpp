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

#include "impl/solvers/ifollows.h"

namespace simple {
namespace impl {

using namespace simple;

template <>
ConditionSet IFollowSolver::solve_right<StatementAst>(StatementAst *statement) {
    ConditionSet result;

    if(statement->next()) {
        while(statement->next() != NULL) {
            result.insert(new SimpleStatementCondition(statement->next()));
            statement = statement->next();
        }
    }
    return result;
}
    
template <>
ConditionSet IFollowSolver::solve_left<StatementAst>(StatementAst *statement) {
    ConditionSet result;

    if(statement->prev()) {
        while(statement->prev() != NULL) {
            result.insert(new SimpleStatementCondition(statement->prev()));
            statement = statement->prev();
        }
    }
    return result;
}

template <>
bool IFollowSolver::validate<StatementAst, StatementAst>(
        StatementAst *left, StatementAst *right)
{
    while(left->next() != NULL) {
        if(left->next() == right) {
            return true;
        }
        left = left->next();
    }
    return false;
}




}
}
