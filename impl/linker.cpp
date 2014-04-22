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

#include "impl/linker.h"

namespace simple {
namespace impl {

using namespace simple;

SimpleQueryLinker::SimpleQueryLinker() :
    _valid_state(true)
{ }

SimpleQueryLinker::SimpleQueryLinker(ConditionSet global_set) : 
    _valid_state(true), _global_set(global_set)
{ }

bool SimpleQueryLinker::is_initialized(const std::string& qvar) {
    return _qvar_table.count(qvar) > 0;
}

bool SimpleQueryLinker::has_condition(
    const std::string& qvar, const ConditionPtr& condition) 
{
    return _qvar_table[qvar].has_element(condition);
}

bool SimpleQueryLinker::has_link(
    const std::string& qvar1, const std::string& qvar2)
{
    return _qvar_link_table[qvar1].count(qvar2) > 0;
}

/*
 * Add a single link between two conditions in qvar1 and qvar2.
 * The adding is only successful if both of the query variables
 * already has the respective conditions in their result set.
 */
bool SimpleQueryLinker::add_link(
    const std::string& qvar1, const std::string& qvar2, 
    const ConditionPtr& condition1, const ConditionPtr& condition2)
{
    if(has_condition(qvar1, condition1) && has_condition(qvar2, condition2)) {
        _condition_link_table[QVarPair(qvar1, qvar2)][condition1].insert(condition2);
        _condition_link_table[QVarPair(qvar2, qvar1)][condition2].insert(condition1);

        return true;

    } else {
        return false;
    }
}

void SimpleQueryLinker::update_results(
    const std::string& qvar, const ConditionSet& new_set)
{
    if(!is_initialized(qvar)) {
        _qvar_table[qvar] = new_set;
        return;
    }

    ConditionSet difference = _qvar_table[qvar].difference_with(new_set);

    if(difference.is_empty()) return;

    for(auto it = difference.begin(); it != difference.end(); ++it) {
        remove_condition(qvar, *it);
    }
}

void SimpleQueryLinker::update_links(
    const std::string& qvar1, const std::string& qvar2, 
    const std::set<ConditionPair>& links)
{
    if(!is_initialized(qvar1)) _qvar_table[qvar1] = _global_set;
    if(!is_initialized(qvar2)) _qvar_table[qvar2] = _global_set;

    if(has_link(qvar1, qvar2)) {
        std::set<ConditionPair> old_links = get_links_as_tuples(qvar1, qvar2);

        for(auto it=old_links.begin(); it!=old_links.end(); ++it) {
            ConditionPair old_link = *it;

            if(links.count(old_link) == 0) {
                break_both_links(qvar1, qvar2, old_link.first, old_link.second);
            }
        }

    } else {
        ConditionSet new_set1;
        ConditionSet new_set2;

        for(auto it = links.begin(); it != links.end(); ++it) {
            ConditionPtr condition1 = it->first;
            ConditionPtr condition2 = it->second;

            if(add_link(qvar1, qvar2, condition1, condition2)) {
                new_set1.insert(condition1);
                new_set2.insert(condition2);
            }
        }

        update_results(qvar1, new_set1);
        update_results(qvar2, new_set2);

        _qvar_link_table[qvar1].insert(qvar2);
        _qvar_link_table[qvar2].insert(qvar1);
    }
}

bool SimpleQueryLinker::validate_condition_pair(
    const Qvar& qvar1, 
    const Qvar& qvar2, 
    const ConditionPtr& condition1, 
    const ConditionPtr& condition2)
{
    QVarPair qvar_pair(qvar1, qvar2);
    ConditionPair condition_pair(condition1, condition2);

    std::pair<QVarPair, ConditionPair> key(qvar_pair, condition_pair);

    // if(_valid_pair_cache.count(key) > 0) return true;
    // if(_invalid_pair_cache.count(key) > 0) return false;

    ConditionSet links = cached_get_indirect_links(qvar1, qvar2, condition1);

    if(links.has_element(condition2)) {
        _valid_pair_cache.insert(key);

        return true;
    } else {
        _invalid_pair_cache.insert(key);

        return false;
    }
}

bool SimpleQueryLinker::validate_tuple(
    const Qvar& qvar1, 
    const ConditionPtr& condition1,
    const QvarList& rest_qvars,
    ConditionTuplePtr tuple) 
{
    for(auto q_it = rest_qvars.begin(); q_it != rest_qvars.end(); 
        ++q_it, tuple=tuple->next()) 
    {
        const Qvar& qvar2 = *q_it;

        ConditionPtr condition2 = tuple->get_condition();

        if(qvar1 == qvar2 && condition1 != condition2) return false;

        if(!cached_has_indirect_links(qvar1, qvar2)) continue;

        if(!validate_condition_pair(qvar1, qvar2, condition1, condition2)) {
            return false;
        }
    }

    return true;
}

TupleList SimpleQueryLinker::make_tuples(std::vector<std::string> qvars) {
    auto qit = qvars.begin();
    Qvar qvar = *qit++; // current qvar

    QvarList rest_qvars(qit, qvars.end());

    // current qvar conditions
    ConditionSet conditions = get_conditions(qvar);
    TupleList result;

    if(rest_qvars.empty()) {
        for(auto it = conditions.begin(); it != conditions.end(); ++it) {
            ConditionTuplePtr new_tuple(new SimpleConditionTuple(*it));

            result.insert(new_tuple);
        }

        return result;
    }

    TupleList rest_tuples = make_tuples(rest_qvars);

    for(auto cit=conditions.begin(); cit!=conditions.end(); ++cit) {
        const ConditionPtr& condition = *cit;

        for(auto rit=rest_tuples.begin(); rit!=rest_tuples.end(); ++rit) {
            ConditionTuplePtr tuple = *rit;

            if(!validate_tuple(qvar, condition, rest_qvars, tuple)) continue;

            ConditionTuplePtr new_tuple(new SimpleConditionTuple(condition, tuple));

            result.insert(new_tuple);
        }
    }

    return result;
}

void SimpleQueryLinker::remove_condition(
    const std::string& qvar, const ConditionPtr& condition)
{
    ConditionSet& qvar_values = _qvar_table[qvar];

    if(!qvar_values.has_element(condition)) return;

    qvar_values.remove(condition);

    const std::set<std::string>& qvar_links = _qvar_link_table[qvar];

    /*
     * If it is a removal of the last condition in a qvar and
     * makes it empty, the whole PQL is then fall into an invalid state.
     */
    if(qvar_values.is_empty()) {
        invalidate_state();
    }

    /*
     * Iterate through the query variables that have linkage with this query variable
     */
    for(auto qit = qvar_links.begin(); qit != qvar_links.end(); ++qit) {
        std::string current_link = *qit;

        // clear the current link table and move that to local variable
        ConditionSet linked_set = std::move(
            _condition_link_table[QVarPair(qvar, current_link)][condition]);

        if(linked_set.is_empty()) {
            continue;
        }

        /*
         * Iterate through the conditions in the iterated query variable that
         * are linked to this condition.
         */
        for(ConditionSet::iterator cit = linked_set.begin();
            cit != linked_set.end(); ++cit)
        {
            ConditionPtr current_condition = *cit;

            break_link(current_link, qvar, current_condition, condition);
        }
    }
}

/*
 * Simple breaking of the link of condition1 in qvar with condition2.
 * if condition1 does not have any other link in qvar, return true 
 * to indicate that the break is complete.
 */
void SimpleQueryLinker::break_link(
    const std::string& qvar1, const std::string& qvar2,
    const ConditionPtr& condition1, const ConditionPtr& condition2)
{
    ConditionSet& set1 = _condition_link_table[QVarPair(qvar1, qvar2)][condition1];
    set1.remove(condition2);

    if(set1.is_empty()) {
        /*
         * If there is no other condition link in condition1 to
         * other conditions in qvar2, then condition1 is 
         * invalidated and needs to be removed as well.
         */
        remove_condition(qvar1, condition1);
    }
}

void SimpleQueryLinker::break_both_links(
    const std::string& qvar1, const std::string& qvar2,
    const ConditionPtr& condition1, const ConditionPtr& condition2)
{
    break_link(qvar1, qvar2, condition1, condition2);
    break_link(qvar2, qvar1, condition2, condition1);
}

ConditionSet SimpleQueryLinker::get_linked_conditions(
    const std::string& qvar1, const std::string& qvar2,
    const ConditionPtr& condition1) 
{
    if(_condition_link_table[QVarPair(qvar1, qvar2)].count(condition1) > 0) {
        return _condition_link_table[QVarPair(qvar1, qvar2)][condition1];

    } else {
        return ConditionSet();
    }
}
bool SimpleQueryLinker::cached_has_indirect_links(
    const std::string& qvar1, const std::string& qvar2)
{
    return has_indirect_links(qvar1, qvar2);

    QVarPair key(qvar1, qvar2);
    if(_has_indirect_links_cache.count(key)) return _has_indirect_links_cache[key];

    bool result = has_indirect_links(qvar1, qvar2);
    _has_indirect_links_cache[key] = result;

    return result;
}

ConditionSet SimpleQueryLinker::cached_get_indirect_links(
    const std::string& qvar1, const std::string& qvar2,
    const ConditionPtr& condition1)
{
    return get_indirect_links(qvar1, qvar2, condition1);

    std::pair<QVarPair, ConditionPtr> key(QVarPair(qvar1, qvar2), condition1);

    if(_indirect_links_cache.count(key)) return _indirect_links_cache[key];
    
    ConditionSet result = get_indirect_links(qvar1, qvar2, condition1);
    _indirect_links_cache[key] = result;

    return result;
}

bool SimpleQueryLinker::has_indirect_links(
    const std::string& qvar1, const std::string& qvar2,
    std::set<std::string> visited_qvars)
{
    if(has_link(qvar1, qvar2)) return true;

    const std::set<std::string>& qvar1_links = _qvar_link_table[qvar1];

    for(auto qit = qvar1_links.begin(); qit != qvar1_links.end(); ++qit) {
        std::string mid_qvar = *qit;

        if(visited_qvars.count(mid_qvar) != 0) continue;

        visited_qvars.insert(mid_qvar);

        if(has_indirect_links(mid_qvar, qvar2, visited_qvars)) {
            return true;
        }
    }

    return false;
}

ConditionSet SimpleQueryLinker::get_indirect_links(
    const std::string& qvar1, const std::string& qvar2,
    const ConditionPtr& condition1, 
    std::set<std::string> visited_qvars)
{
    if(has_link(qvar1, qvar2)) {
        return get_linked_conditions(qvar1, qvar2, condition1);
    }

    ConditionSet result;

    const std::set<std::string>& qvar1_links = _qvar_link_table[qvar1];

    for(auto qit = qvar1_links.begin(); qit != qvar1_links.end(); ++qit) {
        std::string mid_qvar = *qit;

        if(visited_qvars.count(mid_qvar) != 0) continue;

        visited_qvars.insert(mid_qvar);

        ConditionSet direct_links = get_linked_conditions(
            qvar1, mid_qvar, condition1);

        for(auto cit = direct_links.begin();
            cit != direct_links.end(); ++cit)
        {
            result.union_with(get_indirect_links(mid_qvar, qvar2,
                *cit, visited_qvars));
        }
    }
    
    return result;
}

ConditionSet SimpleQueryLinker::get_conditions(const std::string& qvar) {
    if(is_initialized(qvar)) return _qvar_table[qvar];

    _qvar_table[qvar] = _global_set;

    return _global_set;
}

std::map<ConditionPtr, ConditionSet> SimpleQueryLinker::get_links(
    const std::string& qvar1, const std::string& qvar2)
{
    return _condition_link_table[QVarPair(qvar1, qvar2)];
}

std::set<ConditionPair> SimpleQueryLinker::get_links_as_tuples(
    const std::string& qvar1, const std::string& qvar2) 
{
    std::set<ConditionPair> result;

    const std::map<ConditionPtr, ConditionSet>& link_table = 
        _condition_link_table[QVarPair(qvar1, qvar2)];

    for(auto it=link_table.begin(); it!=link_table.end(); ++it) {
        ConditionPtr condition1 = it->first;
        const ConditionSet& condition2_set = it->second;

        for(auto it2=condition2_set.begin(); 
            it2!=condition2_set.end(); ++it2)
        {
            ConditionPtr condition2 = *it2;

            result.insert(ConditionPair(condition1, condition2));
        }
    }

    return result;
}

bool SimpleQueryLinker::is_valid_state() {
    return _valid_state;
}

void SimpleQueryLinker::invalidate_state() {
    _valid_state = false;
}

}
}
