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

		class SiblingSolver {
		public:
			SiblingSolver::SiblingSolver(SimpleRoot ast);
			

			template <typename Condition>
			ConditionSet solve_right(Condition *condition) {
				return ConditionSet();
			}

			template <typename Condition>
			ConditionSet solve_left(Condition *condition) {
				return ConditionSet();
			}

			template <typename Condition1, typename Condition2>
			bool validate(Condition1 *condition1, Condition2 *condition2) {
				return false;
			}

			// helper methods to index for solve-left
			void index_proc(ProcAst *proc);
			void index_statement_list(StatementAst *statement);
			void index_statement(StatementAst *statement) ;
			void index_while(WhileAst *while_ast);
			void index_if(IfAst *if_ast);
			void index_assign(AssignmentAst *assign_ast);
			void index_expr(ExprAst *expr_ast);
			void index_variable(VariableAst *variable_ast);

		private:
			SimpleRoot _ast;

			std::map<std::string, std::set<std::string>> _sibling_index;

			std::map<ProcAst*, ProcSet> _sibling_proc_index;
			std::map<StatementAst*, StatementSet> _sibling_statement_index;
			std::map<ExprAst*, ExprSet> _sibling_expression_index;
			std::map<VariableAst*, StatementSet> _sibling_container_index;

			std::string extract_string_from_expr(ExprAst* expr_ast);

			template <typename Condition>
			ConditionSet solve(Condition *condition) {
				return ConditionSet();
			}
		};

		/*
		 * Solve left
		 */
		template<>
		ConditionSet SiblingSolver::solve_left<ProcAst>(ProcAst *proc);

		template <>
		ConditionSet SiblingSolver::solve_left<StatementAst>(
			StatementAst *statement);

		template<>
		ConditionSet SiblingSolver::solve_left<ContainerAst>(
			ContainerAst *container);

		template<>
		ConditionSet SiblingSolver::solve_left<VariableAst>(
			VariableAst *variable);

		template<>
		ConditionSet SiblingSolver::solve_left<ExprAst>(
			ExprAst *expr);

		/*
		 * Solve right
		 */
		template<>
		ConditionSet SiblingSolver::solve_right<ProcAst>(ProcAst *proc);

		template <>
		ConditionSet SiblingSolver::solve_right<StatementAst>(
			StatementAst *statement);

		template<>
		ConditionSet SiblingSolver::solve_right<ContainerAst>(
			ContainerAst *container);

		template<>
		ConditionSet SiblingSolver::solve_right<VariableAst>(
			VariableAst *variable);

		template<>
		ConditionSet SiblingSolver::solve_right<ExprAst>(ExprAst *expr);

		/*
		 * Solve right
		 */
		template <>
		bool SiblingSolver::validate<StatementAst, StatementAst>(
			StatementAst *left, StatementAst *right);

		template<>
		bool SiblingSolver::validate<ProcAst, ProcAst>
			(ProcAst *left, ProcAst *right);

		template<>
		bool SiblingSolver::validate<ExprAst, ExprAst>(ExprAst *left, 
			ExprAst *right);

		template<>
		bool SiblingSolver::validate<VariableAst, ExprAst>(VariableAst *left,
			ExprAst *right);

		template<>
		bool SiblingSolver::validate<ExprAst, VariableAst>(ExprAst *left, 
			VariableAst *right);
} // namespace impl
} // namespace simple
