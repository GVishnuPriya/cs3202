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

#include "impl/solvers/modifies.h"

namespace simple {
namespace impl {

using namespace simple;

class ModifiesAssignSolver : public AssignStatementSolver {
  public:
    ModifiesAssignSolver() { }

    VariableSet solve_assignment(AssignmentAst *assign) {
        VariableSet result;
        result.insert(*assign->get_variable());
        
        return result;
    }
};

ModifiesSolver::ModifiesSolver(const SimpleRoot& ast) : 
    AssignmentSolver(ast, std::shared_ptr<AssignStatementSolver>(new ModifiesAssignSolver()))
{ }

VariableSet ModifiesSolver::get_vars_modified_by_statement(StatementAst *statement) {
    return get_right_vars_from_statement(statement);
}

VariableSet ModifiesSolver::get_vars_modified_by_proc(ProcAst *proc) {
    return get_right_vars_from_proc(proc);
}

} // namespace impl
} // namespace simple
