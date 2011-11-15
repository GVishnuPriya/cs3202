
#pragma once

#include <memory>
#include "simple/qvar.h"
#include "simple/condition_set.h"

namespace simple {
namespace impl {

using namespace simple;

class WildCardQueryVariable : public QueryVariable {
  public:
    WildCardQueryVariable(std::shared_ptr<SimplePredicate> pred) : 
        _pred(pred) 
    { }

    bool is_bounded() const {
        return false;
    }

    ConditionSet& get_conditions() {
        _conditions.clear();
        return _conditions;
    }

    void set_conditions(const ConditionSet& conditions) {
        // no-op
    }

    void set_conditions(ConditionSet&& conditions) {
        // no-op
    }

    SimplePredicate* get_predicate() {
        return _pred.get();
    }

    std::string get_name() {
        return "_";
    }

    virtual ~WildCardQueryVariable() { }

  private:
    ConditionSet                        _conditions;
    std::shared_ptr<SimplePredicate>    _pred;
};

class SimpleQueryVariable : public QueryVariable {
  public:
    SimpleQueryVariable(
        const std::string& name, std::shared_ptr<SimplePredicate> pred) :
        _name(name), _is_bounded(false), _set(), _pred(pred)
    { }

    bool is_bounded() const {
        return _is_bounded;
    }

    ConditionSet& get_conditions() {
        _is_bounded = true;
        return _set;
    }

    void set_conditions(const ConditionSet& conditions) {
        _is_bounded = true;
        _set = conditions;
    }

    void set_conditions(ConditionSet&& conditions) {
        _is_bounded = true;
        _set = std::move(conditions);
    }

    SimplePredicate* get_predicate() {
        return _pred.get();
    }

    std::string get_name() {
        return _name;
    }

    virtual ~SimpleQueryVariable() { }

  private:
    std::string     _name;
    bool            _is_bounded;
    ConditionSet    _set;
    std::shared_ptr<SimplePredicate> _pred;
};

}
}
