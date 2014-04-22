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
#include <set>
#include <list>
#include "simple/condition.h"
#include "simple/condition_set.h"

namespace simple {

class ConditionTuplePtr;

class ConditionTuple {
  public:
    virtual ConditionPtr get_condition() = 0;

    virtual ConditionTuplePtr next() = 0;

    virtual ~ConditionTuple() { }
};

class ConditionTuplePtr : public std::shared_ptr<ConditionTuple> {
  public:
    typedef std::shared_ptr<ConditionTuple> Parent;

    ConditionTuplePtr() : Parent() { }

    ConditionTuplePtr(ConditionTuple *tuple) :
        Parent(tuple)
    { }

    bool operator ==(const ConditionTuplePtr& other) const;

    bool operator <(const ConditionTuplePtr& other) const;
};

class SimpleConditionTuple : public ConditionTuple {
  public:
    SimpleConditionTuple(ConditionPtr condition, 
        ConditionTuplePtr next) :
        _condition(condition), _next(next)
    { }

    SimpleConditionTuple(ConditionPtr condition) : 
        _condition(condition), _next()
    { }

    ConditionPtr get_condition() {
        return _condition;
    }

    ConditionTuplePtr next() {
        return _next;
    }

  private:
    ConditionPtr    _condition;
    ConditionTuplePtr   _next;
};

typedef std::list<ConditionTuplePtr> TupleList;
typedef std::set<ConditionTuplePtr> TupleSet;

std::string tuple_to_string(ConditionTuplePtr tuple);
::std::ostream& operator<<(::std::ostream& os, const ConditionTuplePtr& tuple);
::std::ostream& operator<<(::std::ostream& os, const TupleList& tuple);

bool less_than_tuple(ConditionTuple *tuple1, ConditionTuple *tuple2);

bool equal_tuple(ConditionTuple *tuple1, ConditionTuple *tuple2);


}
