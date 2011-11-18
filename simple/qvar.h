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

#include <memory>
#include <string>
#include "simple/predicate.h"

namespace simple {

class QueryVariable {
  public:
    /*
     * Indicates whether the variable has already bounded to certain 
     * sets of conditions. Whether a QueryVariable is bounded affects 
     * the matching algorithm used to solve a query.
     */
    virtual bool is_bounded() const = 0;

    /*
     * Get the conditions set that are bounded to this variable.
     * Calling this on an unbounded variable will make it bounded and
     * returns the global set defined by the variable's predicate.
     */
    virtual const ConditionSet& get_conditions() = 0;

    /*
     * Bound the variable to a new sets of conditions. If the variable
     * is unbounded, it becomes bounded now.
     *
     * The new ConditionSet must be a subset of the currently bounded 
     * ConditionSet, if bounded, or a subset of the global set defined 
     * by the predicate if is unbounded. Violation of this will cause 
     * an exception to be thrown, as the logic of this program does not 
     * allow this to happen and thus it is an indication of bug.
     */
    virtual void set_conditions(const ConditionSet& conditions) = 0;
    virtual void set_conditions(ConditionSet&& conditions) = 0;

    virtual SimplePredicate* get_predicate() = 0;

    virtual std::string get_name() = 0;

    virtual bool is_wildcard() = 0;

    virtual ~QueryVariable() { }
};

typedef std::shared_ptr<QueryVariable> QVarPtr;

class QVarTable {
  public:
    virtual QVarPtr get_qvar(const std::string& name) const = 0;
    virtual void insert(const std::string& name, QVarPtr qvar) = 0;

    virtual ~QVarTable() { }
};



} // namespace simple
