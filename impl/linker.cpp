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

void SimpleQueryLinker::add_links(
        const std::string& qvar1, const std::string& qvar2, 
        const std::vector<ConditionPair>& links)
{
    if(!is_initialized(qvar1) || !is_initialized(qvar2)) {
        throw QueryLinkerError();
    }

    ConditionSet new_set1;
    ConditionSet new_set2;

    for(std::vector<ConditionPair>::const_iterator it = links.begin();
        it != links.end(); ++it) 
    {
        if(add_link(qvar1, qvar2, *it)) {
            new_set1.insert(it->first);
            new_set2.insert(it->second);
        }
    }

    update_results(qvar1, new_set1);
    update_results(qvar2, new_set2);

    _qvar_link_table[qvar1].insert(qvar2);
    _qvar_link_table[qvar2].insert(qvar1);
}

/*
 * Add a single link between two conditions in qvar1 and qvar2.
 * The adding is only successful if both of the query variables
 * already has the respective conditions in their result set.
 */
bool SimpleQueryLinker::add_link(const std::string& qvar1,
          const std::string& qvar2, const ConditionPair& pair)
{
    if(has_condition(qvar1, pair.first) && has_condition(qvar2, pair.second)) {
        _condition_link_table[QVarPair(qvar1, qvar2)][pair.first].insert(pair.second);
        _condition_link_table[QVarPair(qvar2, qvar1)][pair.second].insert(pair.first);
        return true;
    } else {
        return false;
    }
}

void SimpleQueryLinker::update_results(const std::string& qvar, 
        const ConditionSet& new_set)
{
    ConditionSet difference = _qvar_table[qvar].difference_with(new_set);
    if(!difference.is_empty()) {
        for(ConditionSet::iterator it = difference.begin(); 
                it != difference.end(); ++it )
        {
            remove_condition(qvar, *it);
        }
    }
}

TupleList SimpleQueryLinker::make_tuples(
        const std::vector<std::string>& variables) 
{
    TupleList result;

    std::string first_qvar = *variables.begin();
    
    if(!is_initialized(first_qvar)) {
        throw QueryLinkerError();
    }

    for(ConditionSet::iterator cit = _qvar_table[first_qvar].begin();
        cit != _qvar_table[first_qvar].end(); ++cit)
    {
        TupleList tuples = make_tuples(first_qvar, *cit, 
                ++variables.begin(), variables.end());

        merge_tuples(*cit, tuples, result);
    }

    return result;
}

TupleList SimpleQueryLinker::make_tuples(
        const std::string& current_qvar,
        const ConditionPtr& current_condition,
        std::vector<std::string>::const_iterator next_qit,
        std::vector<std::string>::const_iterator end)
{
    TupleList result;

    if(next_qit == end) {
        for(ConditionSet::iterator cit = _qvar_table[current_qvar].begin();
            cit != _qvar_table[current_qvar].end(); ++cit)
        {
            result.push_back(QueryTuplePtr(new SimpleQueryTuple(*cit)));
        }
    } else {
        std::string next_qvar = *next_qit++;
        if(!is_initialized(next_qvar)) {
            throw QueryLinkerError();
        }

        if(has_link(current_qvar, next_qvar)) {
            ConditionSet linked_conditions = get_linked_conditions(
                    current_qvar, next_qvar, current_condition);

            for(ConditionSet::iterator cit = linked_conditions.begin();
                    cit != linked_conditions.end(); ++cit)
            {
                std::vector<QueryTuplePtr> tuples = make_tuples(
                        next_qvar, *cit, next_qit, end);

                merge_tuples(current_condition, tuples, result);
            }

        } else {
            for(ConditionSet::iterator cit = _qvar_table[next_qvar].begin();
                cit != _qvar_table[next_qvar].end(); ++cit)
            {
                std::vector<QueryTuplePtr> tuples = make_tuples(
                        next_qvar, *cit, next_qit, end);

                merge_tuples(current_condition, tuples, result);
            }
        }
    }

    return result;
}

void SimpleQueryLinker::merge_tuples(const ConditionPtr& target_condition,
        const TupleList& tuples, TupleList& result)
{
    for(TupleList::const_iterator tit = tuples.begin();
        tit != tuples.end(); ++tit)
    {
        result.push_back(QueryTuplePtr(
                    new SimpleQueryTuple(target_condition, *tit)));
    }
}


void SimpleQueryLinker::remove_condition(
        const std::string& qvar, const ConditionPtr& condition)
{
    if(_qvar_table[qvar].has_element(condition)) {
        _qvar_table[qvar].remove(condition);

        /*
         * Iterate through the query variables that have linkage with this query variable
         */
        for(std::set<std::string>::iterator qit = _qvar_link_table[qvar].begin();
            qit != _qvar_link_table[qvar].end(); ++qit)
        {
            // clear the current link table and move that to local variable
            ConditionSet linked_set = std::move(
                    _condition_link_table[QVarPair(qvar, *qit)][condition]);

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
                break_link(*qit, qvar, *cit, condition);
            }
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
    ConditionSet& set = _condition_link_table[QVarPair(qvar1, qvar2)][condition1];
    set.remove(condition2);
    if(set.is_empty()) {
        /*
         * If there is no other condition link in condition1 to
         * other conditions in qvar2, then condition1 is 
         * invalidated and needs to be removed as well.
         */
        remove_condition(qvar1, condition1);
    }
}

bool SimpleQueryLinker::is_initialized(const std::string& qvar) {
    return _qvar_table.count(qvar) > 0;
}

bool SimpleQueryLinker::has_condition(const std::string& qvar, const ConditionPtr& condition) {
    return _qvar_table[qvar].has_element(condition);
}

bool SimpleQueryLinker::has_link(const std::string& qvar1,
              const std::string& qvar2)
{
    return _qvar_link_table[qvar1].count(qvar2) > 0;
}

ConditionSet SimpleQueryLinker::get_linked_conditions(
        const std::string& qvar1, const std::string& qvar2,
        const ConditionPtr& condition1) 
{
    return _condition_link_table[QVarPair(qvar1, qvar2)][condition1];
}

ConditionSet SimpleQueryLinker::get_conditions(const std::string& qvar) {
    return _qvar_table[qvar];
}

std::map<ConditionPtr, ConditionSet> SimpleQueryLinker::get_links(
        const std::string& qvar1, const std::string& qvar2)
{
    return _condition_link_table[QVarPair(qvar1, qvar2)];
}

}
}
