
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
