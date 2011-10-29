
#include <iostream>
#include "simple/condition_set.h"
#include "simple/util/condition_utils.h"

namespace simple {

using simple::util::is_same_condition;
using simple::util::is_less_than_condition;
using simple::util::condition_to_string;

::std::ostream& operator<<(::std::ostream& os, const ConditionSet& set) {
    os << "(ConditionSet ";
    for(ConditionSet::iterator it = set.begin(); it != set.end(); ++it) {
        os << condition_to_string(it->get());
    }
    os << ")";
}

ConditionSet::ConditionSet() : 
    _set() 
{ }

ConditionSet::ConditionSet(const ConditionSet& other) : 
    _set(other._set) 
{ }
ConditionSet::ConditionSet(ConditionSet&& other) : 
    _set(std::move(other._set)) 
{ }

ConditionSet::ConditionSet(ConditionPtr condition) :
    _set()
{
    insert(condition);
}

void ConditionSet::insert(ConditionPtr condition) {
    _set.insert(condition);
}

void ConditionSet::insert(SimpleCondition *condition) {
    _set.insert(ConditionPtr(condition));
}

void ConditionSet::remove(ConditionPtr condition) {
    _set.erase(condition);
}

void ConditionSet::union_with(const ConditionSet& other) {
    if(other.is_empty()) { return; }

    for(std::set<ConditionPtr>::iterator it = other._set.begin();
            it != other._set.end(); ++it)
    {
        _set.insert(*it);
    }
}

void ConditionSet::intersect_with(const ConditionSet& other) {
    if(other.is_empty()) {
        // we are intersecting with empty set, and so
        // the result is also an empty set.
        _set.clear();
        return; 
    }

    std::set<ConditionPtr>::iterator it = _set.begin();
    while(it != _set.end()) {
        if(!other.has_element(*it)) {
            std::set<ConditionPtr>::iterator old_it = it++;
            _set.erase(old_it);
        } else {
            ++it;
        }
    }
}

bool ConditionSet::is_empty() const {
    return _set.empty();
}

bool ConditionSet::equals(const ConditionSet& other) const {
    return _set == other._set;
}

bool ConditionSet::equals(const std::set<ConditionPtr>& other) const {
    return _set == other;
}

bool ConditionSet::operator ==(const ConditionSet& other) const {
    return equals(other);
}

bool ConditionSet::operator !=(const ConditionSet& other) const {
    return !equals(other);
}

bool ConditionSet::has_element(const ConditionPtr& other) const {
    return _set.count(other) != 0;
}

size_t ConditionSet::get_size() const {
    return _set.size();
}

std::set<ConditionPtr>::const_iterator ConditionSet::begin() const {
    return _set.begin();
}

std::set<ConditionPtr>::const_iterator ConditionSet::end() const {
    return _set.end();
}

ConditionSet::~ConditionSet() { }

ConditionPtr::ConditionPtr(SimpleCondition *condition) : 
    _ptr(condition) 
{ }

ConditionPtr::ConditionPtr(const std::shared_ptr<SimpleCondition>& other) :
    _ptr(other)
{ }

ConditionPtr::ConditionPtr(std::shared_ptr<SimpleCondition>&& other) :
    _ptr(std::move(other))
{ }

ConditionPtr::ConditionPtr(const ConditionPtr& other) :
    _ptr(other._ptr)
{ }

ConditionPtr::ConditionPtr(ConditionPtr&& other) :
    _ptr(std::move(other._ptr))
{ }

SimpleCondition* ConditionPtr::get() const {
    return _ptr.get();
}

bool ConditionPtr::operator ==(const ConditionPtr& other) const {
    return equals(other);
}

bool ConditionPtr::operator !=(const ConditionPtr& other) const {
    return !equals(other);
}

bool ConditionPtr::operator <(const ConditionPtr& other) const {
    return less_than(other);
}

bool ConditionPtr::operator >(const ConditionPtr& other) const {
    return !less_than_eq(other);
}

bool ConditionPtr::operator <=(const ConditionPtr& other) const {
    return less_than_eq(other);
}

bool ConditionPtr::operator >=(const ConditionPtr& other) const {
    return !less_than(other);
}

bool ConditionPtr::equals(const ConditionPtr& other) const {
    return is_same_condition(get(), other.get());
}

bool ConditionPtr::less_than(const ConditionPtr& other) const {
    return is_less_than_condition(get(), other.get());
}

bool ConditionPtr::less_than_eq(const ConditionPtr& other) const {
    return equals(other) || less_than(other);
}

ConditionPtr::~ConditionPtr() { }


} // namespace simple
