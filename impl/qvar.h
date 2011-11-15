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
