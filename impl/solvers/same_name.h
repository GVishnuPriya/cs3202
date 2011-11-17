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

#include <string>
#include "simple/ast.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class SameNameSolver {
  public:
    SameNameSolver(SimpleRoot ast) : _ast(ast) { }

    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return solve_name<Condition>(condition);
    }

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return solve_name<Condition>(condition);
    }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2) {
        if(has_name<Condition1>(condition1) && has_name<Condition2>(condition2)) {
            return get_name<Condition1>(condition1) == get_name<Condition2>(condition2);
        } else {
            return false;
        }
    }

    template <typename Condition>
    ConditionSet solve_name(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition>
    void index_name(Condition *condition) {
        // no-op
    }

    void index_statement_list(StatementAst *statement);

    template <typename Condition>
    std::string get_name(Condition *condition) {
        return "";
    }

    template <typename Condition>
    bool has_name(Condition *condition) {
       return false;
    }

  private:
    std::map<std::string, ConditionSet> _name_table;
    SimpleRoot _ast;
};



template <>
ConditionSet SameNameSolver::solve_name<ProcAst>(ProcAst *proc);

template <>
ConditionSet SameNameSolver::solve_name<SimpleVariable>(SimpleVariable *var);

template <>
ConditionSet SameNameSolver::solve_name<SimpleCondition>(SimpleCondition *condition);

template <>
void SameNameSolver::index_name<ProcAst>(ProcAst *proc);

template <>
void SameNameSolver::index_name<SimpleVariable>(SimpleVariable *var);

template <>
void SameNameSolver::index_name<VariableAst>(VariableAst *var);

template <>
void SameNameSolver::index_name<AssignmentAst>(AssignmentAst *assign);
    
template <>
void SameNameSolver::index_name<ConditionalAst>(ConditionalAst *condition);

template <>
void SameNameSolver::index_name<WhileAst>(WhileAst *loop);

template <>
void SameNameSolver::index_name<StatementAst>(StatementAst *statement);

template <>
void SameNameSolver::index_name<ExprAst>(ExprAst *expr);

template <>
std::string SameNameSolver::get_name<ProcAst>(ProcAst *proc);

template <>
std::string SameNameSolver::get_name<SimpleVariable>(SimpleVariable *var);

template <>
std::string SameNameSolver::get_name<SimpleCondition>(SimpleCondition *condition);

template <>
bool SameNameSolver::has_name<ProcAst>(ProcAst *proc);

template <>
bool SameNameSolver::has_name<SimpleVariable>(SimpleVariable *var);

template <>
bool SameNameSolver::has_name<SimpleCondition>(SimpleCondition *condition);


}
}
