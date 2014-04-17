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

#include <map>
#include <string>
#include <utility>
#include <exception>
#include "simple/linker.h"

namespace simple {
namespace impl {

using namespace simple;

typedef std::string Qvar;
typedef std::vector<Qvar> QvarList;
typedef std::pair<Qvar, Qvar> QVarPair;

class QueryLinkerError : public std::exception { };

class SimpleQueryLinker : public QueryLinker {
  public:
    SimpleQueryLinker();

    void update_links(
        const std::string& qvar1, const std::string& qvar2, 
        const std::set<ConditionPair>& links);

    void update_results(const std::string& qvar, const ConditionSet& conditions);

    bool validate_condition_pair(
        const Qvar& qvar1, 
        const Qvar& qvar2, 
        const ConditionPtr& condition1, 
        const ConditionPtr& condition2);
    
    bool validate_tuple(const Qvar& qvar1, 
        const ConditionPtr& condition1,
        const QvarList& rest_qvars,
        ConditionTuplePtr tuple);

    TupleList make_tuples(std::vector<std::string> variables);

    bool add_link(const std::string& qvar1, const std::string& qvar2, 
        const ConditionPtr& condition1, const ConditionPtr& condition2);

    void remove_condition(const std::string& qvar, const ConditionPtr& condition);

    /*
     * Break the link of condition1 in qvar1 with condition2 in qvar2. 
     */
    void break_link(const std::string& qvar1, const std::string& qvar2,
        const ConditionPtr& condition1, const ConditionPtr& condition2);

    bool is_initialized(const std::string& qvar);

    bool has_condition(const std::string& qvar, 
        const ConditionPtr& condition);

    bool has_link(const std::string& qvar1,
        const std::string& qvar2);

    ConditionSet get_linked_conditions(
        const std::string& qvar1, const std::string& qvar2,
        const ConditionPtr& condition1);

    ConditionSet get_conditions(const std::string& qvar,
        SimplePredicate *pred);

    ConditionSet get_conditions(const std::string& qvar);

    std::map<ConditionPtr, ConditionSet> get_links(
        const std::string& qvar1, const std::string& qvar2);

    std::set<ConditionPair> get_links_as_tuples(
        const std::string& qvar1, const std::string& qvar2);

    bool has_indirect_links(
        const std::string& qvar1, const std::string& qvar2,
        std::set<std::string> visited_qvars = std::set<std::string>());

    ConditionSet get_indirect_links(
        const std::string& qvar1, const std::string& qvar2,
        const ConditionPtr& condition1, 
        std::set<std::string> visited_qvars = std::set<std::string>());

    bool cached_has_indirect_links(
        const std::string& qvar1, const std::string& qvar2);

    ConditionSet cached_get_indirect_links(
        const std::string& qvar1, const std::string& qvar2,
        const ConditionPtr& condition1);

    bool validate(const std::string& qvar1, const std::string& qvar2, 
        const ConditionPtr& condition1, const ConditionPtr& condition2);

    bool is_valid_state();
    void invalidate_state();
    
  private:
    std::set< std::pair<QVarPair, ConditionPair> >
    _valid_pair_cache;

    std::set< std::pair<QVarPair, ConditionPair> >
    _invalid_pair_cache;

    std::map< QVarPair, bool > 
    _has_indirect_links_cache;
    
    std::map< std::pair<QVarPair, ConditionPtr>,  ConditionSet> 
    _indirect_links_cache;

    std::map< QVarPair, std::map<ConditionPtr, ConditionSet> >
    _condition_link_table;

    std::map< std::string, std::set<std::string> >
    _qvar_link_table;
    
    std::map< std::string, ConditionSet > 
    _qvar_table;

    std::map< std::string, bool >
    _initialize_table;

    bool _valid_state;
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

}
}
