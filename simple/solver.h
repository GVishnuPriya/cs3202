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

#include <list>
#include <exception>
#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

using namespace simple;

class SolverDependencyException : public std::exception { };

class QueryValidator {
  public:
    virtual bool validate(SimpleCondition *left_condition, 
            SimpleCondition *right_condition) = 0;

    virtual ~QueryValidator() { }
};

class QuerySolver : public QueryValidator {
  public:
    virtual ConditionSet solve_left(SimpleCondition *right_condition) = 0;
    virtual ConditionSet solve_right(SimpleCondition *left_condition) = 0;

   virtual ~QuerySolver() { }
};

} // namespace matcher
