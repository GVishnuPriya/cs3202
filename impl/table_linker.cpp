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

#include "impl/table_linker.h"

namespace simple {
namespace impl {

using namespace simple;

TableQueryLinker::TableQueryLinker()
{
  _initialised = false;
}

void update_links(const std::string& qvar1, const std::string& qvar2, const std::vector<ConditionPair>& links)
{
  indexMap::iterator in1 = _indexes.find(qvar1);
  indexMap::iterator in2 = _indexes.find(qvar2);
  table _new_results;
  tableRow _new_row;
  
  if(!_initialised) {
    for(ConditionPair::iterator it = links.begin(); it != links.end(); ++it)
    {
      _new_row.push_back(it.first);
      _new_row.push_back(it.second);
      _new_results.push_back(_new_row);
      _new_row.clear();
    }
    indexes.insert(indexMap(qvar1, 0));
    indexes.insert(indexMap(qvar2, 1));
    _results = _new_results;
    _initialised = true;
  }
  else if(in1 == _index.end() && in2 == _index.end()) {
    for(indexMap::iterator it1 = _results.begin(); it1 != _results.end(); ++it1) //thru list of rows
    {
      for(tableRow::iterator it2 = *it1.begin(); it2 != *it1.end(); ++it2) //thru row
      {
        for(ConditionPair::iterator it3 = links.begin(); it3 != links.end(); ++it3)
        {
          _new_row = *it2;
          _new_row.push_back(it3.first);
          _new_row.push_back(it3.second);
          _new_results.push_back(_new_row);
          _new_row.clear();
        }
      }
    }
    _indexes.insert(indexMap(qvar1, index.size()));
    _indexes.insert(indexMap(qvar2, index.size()));
    _results = _new_results;
  }
  else if(in1 == _index.end()) {
    int i2 = in2.second;
    
    for(indexMap::iterator it1 = _results.begin(); it1 != _results.end(); ++it1) //thru list of rows
    {
      for(tableRow::iterator it2 = *it1.begin(); it2 != *it1.end(); ++it2) //thru row
      {
        for(ConditionPair::iterator it3 = links.begin(); it3 != links.end(); ++it3)
        {
          _new_row = *it2;
          if(_new_row[i2].equals(it3.second)) {
            _new_row.push_back(it3.first);
            _new_results.push_back(_new_row);
          }
          _new_row.clear();
        }
      }
    }
    _indexes.insert(indexMap(qvar1, index.size()));
    _results = _new_results;
  }
  else if(it2 == _index.end()) {
    int i1 = in1.second;
    
    for(indexMap::iterator it1 = _results.begin(); it1 != _results.end(); ++it1) //thru list of rows
    {
      for(tableRow::iterator it2 = *it1.begin(); it2 != *it1.end(); ++it2) //thru row
      {
        for(ConditionPair::iterator it3 = links.begin(); it3 != links.end(); ++it3)
        {
          _new_row = *it2;
          if(_new_row[i1].equals(it3.first)) {
            _new_row.push_back(it3.second);
            _new_results.push_back(_new_row);
          }
          _new_row.clear();
        }
      }
    }
    _indexes.insert(indexMap(qvar2, index.size()));
    _results = _new_results;
  }
  else {
    int i1 = in1.second;
    int i2 = in2.second;
    
    for(indexMap::iterator it1 = _results.begin(); it1 != _results.end(); ++it1) //thru list of rows
    {
      for(tableRow::iterator it2 = *it1.begin(); it2 != *it1.end(); ++it2) //thru row
      {
        for(ConditionPair::iterator it3 = links.begin(); it3 != links.end(); ++it3)
        {
          _new_row = *it2;
          if(_new_row[i1].equals(it3.first) && _new_row[i2].equals(it3.second)) {
            _new_results.push_back(_new_row);
          }
          _new_row.clear();
        }
      }
    }
    _results = _new_results;
  }
  
}
  
void update_results(const std::string& qvar, const ConditionSet& conditions)
{
  indexMap::iterator index = _indexes.find(qvar);
  table _new_results;
  tableRow _new_row;
  
  //empty table
  if(!_initialised) {
    for(ConditionSet::iterator it = conditions.begin(); it != conditions.end(); ++it)
    {
      _new_row.push_back(*it);
      _new_results.push_back(_new_row);
      _new_row.clear();
    }
    _indexes.insert(indexMap(qvar, 0));
    _results = _new_results;
    _initialised = true;
  }
  //new qvar
  else if(index == _index.end()) {
    for(indexMap::iterator it1 = _results.begin(); it1 != _results.end(); ++it1) //thru list of rows
    {
      for(tableRow::iterator it2 = *it1.begin(); it2 != *it1.end(); ++it2) //thru row
      {
        for(ConditionSet::iterator it3 = conditions.begin(); it3 != conditions.end(); ++it3)
        {
          _new_row = *it2;
          _new_row.push_back(*it3);
          _new_results.push_back(_new_row);
          _new_row.clear();
        }
      }
    }
    _indexes.insert(indexMap(qvar, index.size()));
    _results = _new_results;
  }
  //existing qvar
  else {
    int i = index.second;
    
    for(indexMap::iterator it1 = _results.begin(); it1 != _results.end(); ++it1) //thru list of rows
    {
      for(tableRow::iterator it2 = *it1.begin(); it2 != *it1.end(); ++it2) //thru row
      {
        for(ConditionSet::iterator it3 = conditions.begin(); it3 != conditions.end(); ++it3)
        {
          _new_row = *it2;
          if(_new_row[i].equals(*it3)){
            _new_results.push_back(_new_row);
          }
          _new_row.clear();
        }
      }
    }
    _results = _new_results;
  }
  
}

  
  
}
}
