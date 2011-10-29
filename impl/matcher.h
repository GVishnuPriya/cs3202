
#include <exception>
#include <memory>
#include <string>
#include "simple/solver.h"
#include "simple/matcher.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::impl;

class SimpleQueryVariable : public QueryVariable {
  public:
    SimpleQueryVariable(const std::string& name) :
        _name(name), _is_bounded(false), _set()
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

    virtual ~SimpleQueryVariable() { }

  private:
    std::string _name;
    bool _is_bounded;
    ConditionSet _set;
};

class SimpleQueryMatcher : public QueryMatcher {
  public:
    SimpleQueryMatcher(QuerySolver *solver) : _solver(solver) { } 

    void solve_both(QueryVariable *left, QueryVariable *right) {
        if(left->is_bounded() && right->is_bounded()) {
            ConditionSet new_left;
            ConditionSet new_right;

            for(ConditionSet::iterator left_it = left->get_conditions().begin();
                    left_it != left->get_conditions().end(); ++left_it)
            {
              for(ConditionSet::iterator right_it = right->get_conditions().begin();
                      right_it != right->get_conditions().end(); ++right_it)
              {
                if(_solver->validate(left_it->get(), right_it->get())) {
                  new_left.insert(*left_it);
                  new_right.insert(*right_it);
                }
              }
            }
            
            left->set_conditions(std::move(new_left));
            right->set_conditions(std::move(new_right));
        } else if(left->is_bounded() && !right->is_bounded()) {
            for(ConditionSet::iterator it = left->get_conditions().begin();
                    it != left->get_conditions().end(); ++it)
            {
                right->get_conditions().union_with(_solver->solve_right(it->get()));
            }
        } else if(!left->is_bounded() && right->is_bounded()) {
            for(ConditionSet::iterator it = right->get_conditions().begin();
                    it != right->get_conditions().end(); ++it)
            {
                left->get_conditions().union_with(_solver->solve_left(it->get()));
            }
        } else {
            throw std::exception(); // not implemented at the moment
        }
    }

    void solve_right(SimpleCondition *left, QueryVariable *right) {
        if(right->is_bounded()) {
            right->get_conditions().union_with(_solver->solve_right(left));
        } else {
            right->set_conditions(_solver->solve_right(left));
        }
    }

    void solve_left(QueryVariable *left, SimpleCondition *right) {
        if(left->is_bounded()) {
            left->get_conditions().union_with(_solver->solve_left(right));
        } else {
            left->set_conditions(_solver->solve_left(right));
        }
    }

    virtual ~SimpleQueryMatcher() { }
  private:
    std::unique_ptr<QuerySolver> _solver;
};


}
}
