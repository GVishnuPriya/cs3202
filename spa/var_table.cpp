
#include <iterator>
#include "spa/var_table.h"

namespace spa {

VarTable::VarTable(VariableSet variables) :
    _variables(variables), 
    _begin(_variables.begin()),
    _end(_variables.end())
{ }

int VarTable::getVarIndex(Var var) {
    VarIterator it = _variables.find(var);
    if(it == _end) return -1;
    return std::distance(it, _begin);
}

Var VarTable::getVarFromIndex(int index) {
    if(index < 0) throw std::out_of_range("Invalid var index");

    VarIterator it = _variables.begin();
    std::advance(it, index);
    if(it == _end) throw std::out_of_range("Invalid var index");

    return *it;
}

}