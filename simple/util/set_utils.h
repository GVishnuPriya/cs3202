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

#include <set>
#include "simple/ast.h"

namespace simple {
namespace util {

template <typename T>
void union_set(std::set<T>& set1, const std::set<T>& set2) {
    if(set2.size() == 0) {
        return;
    }
    
    for(typename std::set<T>::const_iterator it = set2.begin();
            it != set2.end(); ++it)
    {
        set1.insert(*it);
    }
}

template <typename T>
std::set<T> difference_set(const std::set<T>& set1, const std::set<T>& set2) {
    std::set<T> result;

    for(typename std::set<T>::const_iterator it = set1.begin();
        it != set1.end(); ++it)
    {
        if(set2.count(*it) == 0) {
            result.insert(*it);
        }
    }

    return result;
}

} // namespace util
} // namespace simple
