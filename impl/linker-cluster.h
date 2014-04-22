
#pragma once

#include <map>
#include <string>
#include <utility>
#include <stdexcept>
#include "simple/linker.h"

namespace simple {
namespace impl {

using namespace simple;

enum QvarType {
    UnlinkedQvar,
    LinkedQvar
};

class LinkerError : public std::runtime_error {
  public:
    LinkerError(const std::string& message) : 
      std::runtime_error(message)
    { }
};

std::set<ConditionPair> reverse_pairs(const std::set<ConditionPair>& pairs);

class QvarCluster {
  public:
    QvarCluster();

    void add_qvar(const Qvar& qvar, const ConditionSet& values);

    void update_qvar(const Qvar& qvar, const ConditionSet& values);

    void add_pair(const Qvar& qvar1, const Qvar& qvar2, 
        const std::set<ConditionPair>& values);

    void add_new_pair(const Qvar& qvar1, const Qvar& qvar2, 
        const std::set<ConditionPair>& values);

    void add_pair_first_exist(const Qvar& qvar1, const Qvar& qvar2, 
        const std::set<ConditionPair>& values);

    void update_pair(const Qvar& qvar1, const Qvar& qvar2, 
        const std::set<ConditionPair>& values);

    ConditionSet get_conditions(const Qvar& qvar);

    TupleSet make_tuples(const std::vector<Qvar>& qvars);

    bool is_valid_state();

    ConditionPtr get_condition_from_tuple_index(
        const ConditionTuplePtr& tuple, int index);

    ConditionPtr get_condition_from_tuple(
        const ConditionTuplePtr& tuple,
        const Qvar& qvar);

    bool has_qvar(const Qvar& qvar);

    int get_qvar_index(const Qvar& qvar);

    int reserve_qvar(const Qvar& qvar);

  private:
    int _qvar_counter;
    std::map<Qvar, int> _qvar_index;
    TupleList _tuples;
};

class SimpleQueryLinker : public QueryLinker {
  public:
    SimpleQueryLinker();

    SimpleQueryLinker(
        std::map<Qvar, PredicatePtr> pred_table, 
        ConditionSet global_set);

    void update_links(
        const std::string& qvar1, 
        const std::string& qvar2, 
        const std::set<ConditionPair>& links);

    void update_results(
        const std::string& qvar,
        const ConditionSet& conditions);

    void init_qvar(
        const std::string& qvar,
        const ConditionSet& conditions);

    ConditionSet get_conditions(const Qvar& qvar);

    TupleSet make_tuples(const std::vector<Qvar>& qvars);

    bool is_valid_state();

    void invalidate_state();

    bool is_initialized(const Qvar& qvar);

    void ensure_initialized(const Qvar& qvar);

    QvarType get_qvar_type(const Qvar& qvar);

  private:
    bool _valid_state;
    std::map<Qvar, PredicatePtr> _pred_table;
    ConditionSet _global_set;

    std::map<Qvar, ConditionSet> _single_qvar_values;
    std::map<Qvar, QvarType> _qvar_type_table;
    QvarCluster _cluster;
};

}
}