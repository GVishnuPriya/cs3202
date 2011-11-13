
#pragma once

#include <memory>

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
     * returns an empty set.
     */
    virtual ConditionSet& get_conditions() = 0;

    /*
     * Bound the variable to a new sets of conditions. If the variable
     * is unbounded, it becomes bounded now.
     */
    virtual void set_conditions(const ConditionSet& conditions) = 0;
    virtual void set_conditions(ConditionSet&& conditions) = 0;

    virtual SimplePredicate* get_predicate() = 0;

    virtual ~QueryVariable() { }
};

class QVarTable {
  public:
    virtual std::shared_ptr<QueryVariable> get_qvar(const std::string& name) const = 0;
    virtual void insert(const std::string& name, QueryVariable *qvar) = 0;

    virtual ~QVarTable() { }
}


}
