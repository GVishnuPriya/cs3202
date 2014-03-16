/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2014 Soares Chen
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

#include "impl/condition.h"
#include "simple/condition_set.h"

namespace simple{
namespace util {

using namespace impl;

inline ConditionSet statement_set_to_condition_set(const StatementSet& statement_set) {
    ConditionSet result;

    for(auto it = statement_set.begin(); 
            it!= statement_set.end(); ++it)
    {
        result.insert(new SimpleStatementCondition(*it));
    }

    return result;
}

inline ConditionSet proc_set_to_condition_set(const ProcSet& proc_set) {
    ConditionSet result;

    for(auto it = proc_set.begin(); 
            it!= proc_set.end(); ++it)
    {
        result.insert(new SimpleProcCondition(*it));
    }

    return result;
}

inline ConditionSet variable_set_to_condition_set(const VariableSet& variable_set) {
    ConditionSet result;

    for(auto it = variable_set.begin(); 
            it!= variable_set.end(); ++it)
    {
        result.insert(new SimpleVariableCondition(*it));
    }

    return result;
}

}
}