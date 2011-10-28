
#pragma once

#include <set>
#include <memory>
#include <iostream>
#include "simple/condition.h"

namespace simple {

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

    void insert(ConditionPtr condition);
    void insert(SimpleCondition *condition);
    void union_with(const ConditionSet& other);
    void intersect_with(const ConditionSet& other);

    bool is_empty() const;
    bool equals(const ConditionSet& other) const;
    bool equals(const std::set<ConditionPtr>& other) const;
    bool has_element(const ConditionPtr& other) const;
    size_t get_size() const;

    bool operator ==(const ConditionSet& other) const;
    bool operator !=(const ConditionSet& other) const;

    iterator begin() const;
    iterator end() const;

    ~ConditionSet();

  private:
    std::set<ConditionPtr> _set;
};

::std::ostream& operator<<(::std::ostream& os, const ConditionSet& set);


} // namespace simple
