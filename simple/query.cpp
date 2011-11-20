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

#include "simple/query.h"
#include "simple/util/query_utils.h"

namespace simple {

using namespace simple::util;

bool ClausePtr::operator ==(const ClausePtr& other) const {
    return is_same_clause(get(), other.get());
}

bool ClausePtr::operator <(const ClausePtr& other) const {
    PqlTerm *term11 = get()->get_left_term();
    PqlTerm *term12 = get()->get_right_term();
    PqlTerm *term21 = other->get_left_term();
    PqlTerm *term22 = other->get_right_term();

    if(is_same_term(term11, term21)) {
        if (is_same_term(term12, term22)) {
            return get()->get_solver() < other->get_solver();
        } else {
            return is_less_than_term(term12, term22);
        }
    } else {
        return is_less_than_term(term11, term21);
    }
}


} // namespace simple
