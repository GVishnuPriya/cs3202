

#include "simple/condition.h"
#include "simple/util/condition_utils.h"

namespace simple {

using simple::util::is_same_condition;
using simple::util::is_less_than_condition;

ConditionSet::ConditionSet() : 
    _set() 
{ }

ConditionSet::ConditionSet(const ConditionSet& other) : 
    _set(other._set) 
{ }
ConditionSet::ConditionSet(ConditionSet&& other) : 
    _set(std::move(other._set)) 
{ }

void ConditionSet::insert(std::shared_ptr<SimpleCondition> condition) {

}

void ConditionSet::insert(SimpleCondition *condition) {

}

void ConditionSet::union_with(const ConditionSet& other) {

}

void ConditionSet::intersect_with(const ConditionSet& other) {

}

bool ConditionSet::is_empty() const {
    return false; // stub
}

bool ConditionSet::equals(const ConditionSet& other) const {
    return false; // stub
}

ConditionSet::~ConditionSet() { }

ConditionPtr::ConditionPtr(SimpleCondition *condition) : 
    std::shared_ptr<SimpleCondition>(condition) 
{ }

ConditionPtr::ConditionPtr(const std::shared_ptr<SimpleCondition>& other) :
    std::shared_ptr<SimpleCondition>(other)
{ }

ConditionPtr::ConditionPtr(std::shared_ptr<SimpleCondition>&& other) :
    std::shared_ptr<SimpleCondition>(
        std::forward< std::shared_ptr<SimpleCondition> >(other))
{ }

ConditionPtr::ConditionPtr(const ConditionPtr& other) :
    std::shared_ptr<SimpleCondition>(other)
{ }

ConditionPtr::ConditionPtr(ConditionPtr&& other) :
    std::shared_ptr<SimpleCondition>(
        std::forward< std::shared_ptr<SimpleCondition> >(other))
{ }

bool ConditionPtr::operator ==(const ConditionPtr& other) {
    return equals(other);
}

bool ConditionPtr::operator <(const ConditionPtr& other) {
    return less_than(other);
}

bool ConditionPtr::operator <=(const ConditionPtr& other) {
    return less_than_eq(other);
}

bool ConditionPtr::equals(const ConditionPtr& other) {
    return is_same_condition(get(), other.get());
}

bool ConditionPtr::less_than(const ConditionPtr& other) {
    return is_less_than_condition(get(), other.get());
}

bool ConditionPtr::less_than_eq(const ConditionPtr& other) {
    return equals(other) || less_than(other);
}

ConditionPtr::~ConditionPtr() { }


} // namespace simple
