
#include "spa/statement.h"

namespace spa {

using namespace simple;
using namespace simple::util;

StatementTable::StatementTable(LineTable table) :
  _table(table)
{ }

bool StatementTable::is_valid_statement(StatementLine line) {
    return _table.count(line) != 0;
}

StatementAst* StatementTable::get_statement(StatementLine line) {
    if(!is_valid_statement(line)) {
        throw std::runtime_error("Invalid statement line");
    }

    return _table[line];
}

StatementResults condition_to_statement_results(const ConditionSet& conditions) {
    StatementResults result;

    for(ConditionPtr condition : conditions) {
        StatementCondition *statement_condition = 
            condition_cast<StatementCondition>(condition);

        if(statement_condition != NULL) {
            result.insert(statement_condition->get_statement_ast()
                ->get_statement_line());
        }
    }

    return result;
}

VarResults condition_to_variable_results(const ConditionSet& conditions) {
    VarResults result;

    for(ConditionPtr condition : conditions) {
        VariableCondition *var_condition = 
            condition_cast<VariableCondition>(condition);

        if(var_condition != NULL) {
            result.insert(*var_condition->get_variable());
        }
    }

    return result;
}

ProcResults condition_to_proc_results(const ConditionSet& conditions) {
    ProcResults result;

    for(ConditionPtr condition : conditions) {
        ProcCondition *proc_condition = 
            condition_cast<ProcCondition>(condition);

        if(proc_condition != NULL) {
            result.insert(proc_condition->get_proc_ast()
                ->get_name());
        }
    }

    return result;
}
  
}