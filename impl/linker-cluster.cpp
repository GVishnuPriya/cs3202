
#include "impl/linker.h"

namespace simple {
namespace impl {

using namespace simple;

std::set<ConditionPair> reverse_pairs(const std::set<ConditionPair>& pairs) {
    std::set<ConditionPair> reversed;

    for(auto it=pairs.begin(); it!=pairs.end(); ++it) {
        reversed.insert(ConditionPair(it->second, it->first));
    }

    return reversed;
}

QvarCluster::QvarCluster() : _qvar_counter(0) { 
    _tuples.push_back(ConditionTuplePtr(NULL));
}

void QvarCluster::add_qvar(const Qvar& qvar, const ConditionSet& values) {
    reserve_qvar(qvar);

    TupleList new_tuples;

    for(auto it1=_tuples.begin(); it1!=_tuples.end(); ++it1) {
        ConditionTuplePtr tuple = *it1;

        for(auto it2=values.begin(); it2!=values.end(); ++it2) {
            ConditionPtr value = *it2;

            ConditionTuplePtr new_tuple(new SimpleConditionTuple(value, tuple));
            new_tuples.push_back(new_tuple);
        }
    }

    _tuples = new_tuples;
}

void QvarCluster::update_qvar(const Qvar& qvar, const ConditionSet& values) {
    int index = get_qvar_index(qvar);

    TupleList new_tuples;
    
    for(auto it1=_tuples.begin(); it1!=_tuples.end(); ++it1) {
        ConditionTuplePtr tuple = *it1;

        ConditionPtr condition = get_condition_from_tuple_index(
            tuple, index);

        if(values.has_element(condition)) new_tuples.push_back(tuple);
    }

    _tuples = new_tuples;
}

void QvarCluster::add_pair(const Qvar& qvar1, const Qvar& qvar2, 
    const std::set<ConditionPair>& values)
{
    bool has_qvar1 = has_qvar(qvar1);
    bool has_qvar2 = has_qvar(qvar2);

    if(has_qvar1 && has_qvar2) {
        update_pair(qvar1, qvar2, values);

    } else if(!has_qvar1 && !has_qvar2) {
        add_new_pair(qvar1, qvar2, values);

    } else if(has_qvar1 && !has_qvar2) {
        add_pair_first_exist(qvar1, qvar2, values);

    } else if(!has_qvar1 && has_qvar2) {
        std::set<ConditionPair> reversed = reverse_pairs(values);

        add_pair_first_exist(qvar2, qvar1, reversed);
    }
}

void QvarCluster::add_new_pair(const Qvar& qvar1, const Qvar& qvar2, 
    const std::set<ConditionPair>& values)
{
    reserve_qvar(qvar1);
    reserve_qvar(qvar2);
    
    TupleList new_tuples;
    
    for(auto it1=_tuples.begin(); it1!=_tuples.end(); ++it1) {
        ConditionTuplePtr tuple1 = *it1;

        for(auto it2=values.begin(); it2!=values.end(); ++it2) {
            ConditionPtr condition1 = it2->first;
            ConditionPtr condition2 = it2->second;

            ConditionTuplePtr tuple2(
                new SimpleConditionTuple(condition1, tuple1));

            ConditionTuplePtr tuple3(
                new SimpleConditionTuple(condition2, tuple2));

            new_tuples.push_back(tuple3);
        }
    }

    _tuples = new_tuples;
}

void QvarCluster::add_pair_first_exist(const Qvar& qvar1, const Qvar& qvar2, 
    const std::set<ConditionPair>& values)
{
    int qvar1_index = get_qvar_index(qvar1);

    std::map<ConditionPtr, ConditionSet> value_map;

    for(auto it=values.begin(); it!=values.end(); ++it) {
        ConditionPtr condition1 = it->first;
        ConditionPtr condition2 = it->second;

        value_map[condition1].insert(condition2);
    }

    TupleList new_tuples;

    for(auto it1=_tuples.begin(); it1!=_tuples.end(); ++it1) {
        ConditionTuplePtr tuple = *it1;

        ConditionPtr condition1 = get_condition_from_tuple_index(
            tuple, qvar1_index);

        if(value_map.count(condition1) == 0) continue;

        const ConditionSet& conditions2 = value_map[condition1];

        for(auto it2=conditions2.begin(); it2!=conditions2.end(); ++it2) {
            ConditionPtr condition2 = *it2;

            ConditionTuplePtr new_tuple(
                new SimpleConditionTuple(condition2, tuple));

            new_tuples.push_back(new_tuple);
        }
    }

    reserve_qvar(qvar2);
    _tuples = new_tuples;
}

void QvarCluster::update_pair(const Qvar& qvar1, const Qvar& qvar2, 
    const std::set<ConditionPair>& values)
{
    int qvar1_index = get_qvar_index(qvar1);
    int qvar2_index = get_qvar_index(qvar2);
    
    TupleList new_tuples;
    
    for(auto it1=_tuples.begin(); it1!=_tuples.end(); ++it1) {
        ConditionTuplePtr tuple = *it1;

        ConditionPtr condition1 = get_condition_from_tuple_index(tuple, qvar1_index);
        ConditionPtr condition2 = get_condition_from_tuple_index(tuple, qvar2_index);

        ConditionPair pair(condition1, condition2);

        if(values.count(pair) > 0) {
            new_tuples.push_back(tuple);
        }
    }

    _tuples = new_tuples;
}

ConditionSet QvarCluster::get_conditions(const Qvar& qvar) {
    int index = get_qvar_index(qvar);

    ConditionSet result;

    for(auto it=_tuples.begin(); it!=_tuples.end(); ++it) {
        ConditionTuplePtr tuple = *it;

        ConditionPtr condition = get_condition_from_tuple_index(tuple, index);
        
        result.insert(condition);
    }

    return result;
}

TupleSet QvarCluster::make_tuples(const std::vector<Qvar>& qvars) {
    std::list<int> indexes;
    
    for(auto it=qvars.begin(); it!=qvars.end(); ++it) {
        indexes.push_front(get_qvar_index(*it));
    }

    TupleSet result;
    for(auto it1=_tuples.begin(); it1!=_tuples.end(); ++it1) {
        ConditionTuplePtr tuple = *it1;
        ConditionTuplePtr result_tuple;

        for(auto it2=indexes.begin(); it2!=indexes.end(); ++it2) {
            int index = *it2;

            ConditionPtr condition = get_condition_from_tuple_index(tuple, index);
            result_tuple = new SimpleConditionTuple(condition, result_tuple);
        }

        result.insert(result_tuple);
    }
    return result;
}

bool QvarCluster::is_valid_state() {
    return _qvar_counter == 0 || _tuples.size() > 0;
}

ConditionPtr QvarCluster::get_condition_from_tuple_index(
    const ConditionTuplePtr& tuple, int index)
{
    ConditionTuplePtr current_tuple(tuple);

    int inversed_index = _qvar_counter-index-1;

    for(int i=0; i!=inversed_index; ++i) {
        current_tuple = current_tuple->next();
    }

    return current_tuple->get_condition();
}

ConditionPtr QvarCluster::get_condition_from_tuple(
    const ConditionTuplePtr& tuple,
    const Qvar& qvar) 
{
    int index = get_qvar_index(qvar);

    return get_condition_from_tuple_index(tuple, index);
}

bool QvarCluster::has_qvar(const Qvar& qvar) {
    return _qvar_index.count(qvar) > 0;
}

int QvarCluster::get_qvar_index(const Qvar& qvar) {
    if(!has_qvar(qvar)) throw LinkerError(
        "qvar is not initialized in cluster");

    return _qvar_index[qvar];
}

int QvarCluster::reserve_qvar(const Qvar& qvar) {
    int index = _qvar_counter++;
    _qvar_index[qvar] = index;

    return index;
}

SimpleQueryLinker::SimpleQueryLinker() : _valid_state(true) { }

SimpleQueryLinker::SimpleQueryLinker(
    std::map<Qvar, PredicatePtr> pred_table, 
    ConditionSet global_set) :  
    _valid_state(true),
    _pred_table(pred_table),
    _global_set(global_set)
{ 
    for(auto it=pred_table.begin(); it!=pred_table.end(); ++it) {
        init_qvar(it->first, it->second->global_set());
    }
}

void SimpleQueryLinker::update_links(
    const std::string& qvar1, 
    const std::string& qvar2, 
    const std::set<ConditionPair>& links)
{
    ensure_initialized(qvar1);
    ensure_initialized(qvar2);

    QvarType qvar1_type = get_qvar_type(qvar1);
    QvarType qvar2_type = get_qvar_type(qvar2);

    if(qvar1_type == UnlinkedQvar && qvar2_type == UnlinkedQvar) {
        const ConditionSet& conditions1 = _single_qvar_values[qvar1];
        const ConditionSet& conditions2 = _single_qvar_values[qvar2];

        std::set<ConditionPair> filtered_links;

        for(auto it=links.begin(); it!=links.end(); ++it) {
            ConditionPair pair = *it;

            ConditionPtr condition1 = pair.first;
            ConditionPtr condition2 = pair.second;

            if(conditions1.has_element(condition1) &&
               conditions2.has_element(condition2))
            {
                filtered_links.insert(pair);
            }
        }

        _qvar_type_table[qvar1] = LinkedQvar;
        _qvar_type_table[qvar2] = LinkedQvar;

        _cluster.add_new_pair(qvar1, qvar2, filtered_links);

    } else if(qvar1_type == UnlinkedQvar && qvar2_type == LinkedQvar) {
        const ConditionSet& conditions1 = _single_qvar_values[qvar1];

        std::set<ConditionPair> filtered_links;

        for(auto it=links.begin(); it!=links.end(); ++it) {
            ConditionPair pair = *it;

            ConditionPtr condition1 = pair.first;
            ConditionPtr condition2 = pair.second;

            if(conditions1.has_element(condition1)) {
                filtered_links.insert(ConditionPair(condition2, condition1));
            }
        }

        _qvar_type_table[qvar1] = LinkedQvar;

        _cluster.add_pair_first_exist(qvar2, qvar1, filtered_links);

    } else if(qvar1_type == LinkedQvar && qvar2_type == UnlinkedQvar) {
        const ConditionSet& conditions2 = _single_qvar_values[qvar2];

        std::set<ConditionPair> filtered_links;

        for(auto it=links.begin(); it!=links.end(); ++it) {
            ConditionPair pair = *it;

            ConditionPtr condition2 = pair.second;

            if(conditions2.has_element(condition2)) {
                filtered_links.insert(pair);
            }
        }

        _qvar_type_table[qvar2] = LinkedQvar;

        _cluster.add_pair_first_exist(qvar1, qvar2, filtered_links);

    } else if(qvar1_type == LinkedQvar && qvar2_type == LinkedQvar) {
        _cluster.update_pair(qvar1, qvar2, links);
    }
}

void SimpleQueryLinker::init_qvar(
    const std::string& qvar,
    const ConditionSet& conditions)
{
    _qvar_type_table[qvar] = UnlinkedQvar;
    _single_qvar_values[qvar] = conditions;
}

void SimpleQueryLinker::update_results(
    const std::string& qvar,
    const ConditionSet& conditions)
{
    ensure_initialized(qvar);

    QvarType qvar_type = get_qvar_type(qvar);

    if(qvar_type == UnlinkedQvar) {
        _single_qvar_values[qvar].intersect_with(conditions);
    } else {
        _cluster.update_qvar(qvar, conditions);
    }
}

ConditionSet SimpleQueryLinker::get_conditions(const Qvar& qvar) {
    ensure_initialized(qvar);

    if(get_qvar_type(qvar) == UnlinkedQvar) {
        return _single_qvar_values[qvar];
    } else {
        return _cluster.get_conditions(qvar);
    }
}

TupleSet SimpleQueryLinker::make_tuples(const std::vector<Qvar>& qvars) {
    for(auto it=qvars.begin(); it!=qvars.end(); ++it) {
        Qvar qvar = *it;
        ensure_initialized(qvar);

        if(get_qvar_type(qvar) == UnlinkedQvar) {
            _cluster.add_qvar(qvar, get_conditions(qvar));
        }
    }

    return _cluster.make_tuples(qvars);
}

bool SimpleQueryLinker::is_valid_state() {
    return _valid_state && _cluster.is_valid_state();
}

void SimpleQueryLinker::invalidate_state() {
    _valid_state = false;
}

bool SimpleQueryLinker::is_initialized(const Qvar& qvar) {
    return _qvar_type_table.count(qvar) > 0;
}

void SimpleQueryLinker::ensure_initialized(const Qvar& qvar) {
    if(is_initialized(qvar)) return;

    _qvar_type_table[qvar] = UnlinkedQvar;

    if(_pred_table.count(qvar) > 0) {
        _single_qvar_values[qvar] = _pred_table[qvar]->global_set();
    } else {
        _single_qvar_values[qvar] = _global_set;
    }
}

QvarType SimpleQueryLinker::get_qvar_type(const Qvar& qvar) {
    return _qvar_type_table[qvar];
}


}
}