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

#include <set>
#include <memory>
#include <iostream>
#include "simple/condition.h"
#include "simple/util/set_utils.h"

namespace simple {

using namespace simple::util;

class ConditionPtr {
  public:
    ConditionPtr(SimpleCondition *condition);
    ConditionPtr(const std::shared_ptr<SimpleCondition>& other);
    ConditionPtr(std::shared_ptr<SimpleCondition>&& other);
    ConditionPtr(const ConditionPtr& other);
    ConditionPtr(ConditionPtr&& other);

    SimpleCondition* get() const;

    bool equals(const ConditionPtr& other) const ;
    bool less_than(const ConditionPtr& other) const ;
    bool less_than_eq(const ConditionPtr& other) const;

    operator SimpleCondition*() const;
    bool operator ==(const ConditionPtr& other) const;
    bool operator !=(const ConditionPtr& other) const;
    bool operator <(const ConditionPtr& other) const;
    bool operator <=(const ConditionPtr& other) const;
    bool operator >(const ConditionPtr& other) const;
    bool operator >=(const ConditionPtr& other) const;

    ~ConditionPtr();

  private:
    std::shared_ptr<SimpleCondition> _ptr;
};

class ConditionSet {
  public:
    typedef std::set<ConditionPtr>::const_iterator  iterator;

    ConditionSet();
    ConditionSet(const ConditionSet& other);
    ConditionSet(ConditionSet&& other);
    ConditionSet(ConditionPtr condition);
    ConditionSet(std::set<ConditionPtr>&& set);

    void insert(ConditionPtr condition);
    void insert(SimpleCondition *condition);
    void union_with(const ConditionSet& other);
    void intersect_with(const ConditionSet& other);
    ConditionSet difference_with(const ConditionSet& other);
    void remove(ConditionPtr condition);
    void clear();

    bool is_empty() const;
    bool equals(const ConditionSet& other) const;
    bool equals(const std::set<ConditionPtr>& other) const;
    bool has_element(const ConditionPtr& other) const;
    size_t get_size() const;

    bool operator ==(const ConditionSet& other) const;
    bool operator !=(const ConditionSet& other) const;

    ConditionSet& operator =(const ConditionSet& other);

    iterator begin() const;
    iterator end() const;

    ~ConditionSet();

  private:
    std::set<ConditionPtr> _set;
};

struct ConditionPair {
  public:
    ConditionPair(ConditionPtr first_, ConditionPtr second_) :
        first(first_), second(second_)
    { }

    ConditionPtr first;
    ConditionPtr second;
};

::std::ostream& operator<<(::std::ostream& os, const ConditionSet& set);


} // namespace simple
