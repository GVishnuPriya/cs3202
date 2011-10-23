
#pragma once

class QueryVariable {
  public:
    /*
     * Indicates whether the variable has already bounded to certain 
     * sets of conditions. Whether a QueryVariable is bounded affects 
     * the matching algorithm used to solve a query.
     */
    virtual bool is_bounded() = 0;

    /*
     * Get the conditions set that are bounded to this variable.
     * If the variable is unbounded this will return an empty set.
     */
    virtual ConditionSet& get_conditions() = 0;

    /*
     * Bound the variable to a new sets of conditions. If the variable
     * is unbounded, it becomes bounded now.
     */
    virtual void set_conditions(const ConditionSet& conditions) = 0;
};

class QueryMatcher {
  public:
    virtual void solve_both(QueryVariable *var, QueryVariable *var) = 0;
    virtual void solve_right(Condition *condition, QueryVariable *var) = 0;
    virtual void solve_left(QueryVariable *var, Condition *condition) = 0;
};
