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

#include <sstream>
#include "simple/tuple.h"
#include "simple/util/condition_utils.h"

namespace simple {

using simple::util::condition_to_string;

std::string tuple_to_string(ConditionTuplePtr tuple) {
    std::stringstream ss;
    ss << condition_to_string(tuple->get_condition());
    ConditionTuplePtr next = tuple->next();
    
    if(next) {
        ss << " ";
        ss << tuple_to_string(next);
    }

    return ss.str();
}

::std::ostream& operator<<(::std::ostream& os, const ConditionTuplePtr& tuple) {
    os << tuple_to_string(tuple);
    
    return os;
}

::std::ostream& operator<<(::std::ostream& os, const TupleList& tuple) {
    os << "(TupleList [";
    
    for(auto it = tuple.begin(); it != tuple.end(); ++it) {
        os << tuple_to_string(*it);
        os << ",";
    }

    os << "])";

    return os;
}

bool less_than_tuple(ConditionTuple *tuple1, ConditionTuple *tuple2) {
    if(tuple1->get_condition() < tuple2->get_condition()) {
        return true;
    } else if(tuple1->get_condition() == tuple2->get_condition()) {
        if(tuple1->next() && tuple2->next()) {
            return less_than_tuple(tuple1->next().get(), tuple2->next().get());
        } else {
            return false;
        }
    } else {
        return false;
    }

}

bool equal_tuple(ConditionTuple *tuple1, ConditionTuple *tuple2) {
    if(tuple1->next() && tuple2->next()) {
        return tuple1->get_condition() == tuple2->get_condition() &&
            equal_tuple(tuple1->next().get(), tuple2->next().get());
    } else if(!tuple1->next() && !tuple2->next()) {
        return tuple1->get_condition() == tuple2->get_condition();
    } else {
        return false;
    }
}

bool ConditionTuplePtr::operator ==(const ConditionTuplePtr& other) const {
    return equal_tuple(get(), other.get());
}

bool ConditionTuplePtr::operator <(const ConditionTuplePtr& other) const {
    return less_than_tuple(get(), other.get());
}



}
