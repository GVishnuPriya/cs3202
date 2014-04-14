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

#include "simple/ast.h"
#include "simple/condition.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include <list>

namespace simple {
namespace impl {

using namespace simple;

class SiblingSolver : public QuerySolver {
public:
    SiblingSolver(SimpleRoot ast);

    bool validate(SimpleCondition *left_condition, SimpleCondition *right_condition);

    ConditionSet solve_right(SimpleCondition *condition);

    ConditionSet solve_left(SimpleCondition *condition);

    void index_siblings(ConditionPtr condition1, ConditionPtr condition2);

    void index_proc(ProcAst *proc);
    void index_statement_list(StatementAst *statement);

    void index_statement(StatementAst *statement) ;
    void index_while(WhileAst *while_ast);
    void index_if(IfAst *if_ast);
    void index_assign(AssignmentAst *assign_ast);

    ConditionPtr index_expr(ExprAst *expr);
    ConditionPtr index_binary_op_expr(BinaryOpAst *expr);

  private:
    SimpleRoot _ast;

    std::map<ConditionPtr, ConditionSet> _sibling_index;
};

} // namespace impl
} // namespace simple