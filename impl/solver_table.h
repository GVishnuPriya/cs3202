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

#include <map>
#include "simple/solver.h"

namespace simple {
namespace impl {

class SimpleSolverTable : public SolverTable {
  public:
    SimpleSolverTable() { }

    void insert(const std::string& solver_name, QuerySolver *solver) {
        _table[solver_name] = std::unique_ptr<QuerySolver>(solver);
    }

    QuerySolver* get_solver(const std::string& solver_name) {
        if(has_solver(solver_name)) {
            return _table[solver_name].get();
        } else {
            return NULL;
        }
    }

    bool has_solver(const std::string& solver_name) {
        return _table.count(solver_name) > 0;
    }

    virtual ~SimpleSolverTable() { }

  private:
    std::map<std::string, std::unique_ptr<QuerySolver> > _table;
};


}
}
