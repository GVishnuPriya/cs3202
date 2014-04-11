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

#include "impl/solvers/sibling.h"
#include "simple/util/statement_visitor_generator.h"
#include "simple/util/ast_utils.h"
#include "simple/util/set_convert.h"
#include "simple/util/condition_utils.h"

namespace simple {
	namespace impl {

		using namespace simple;
		using namespace std;

		template <>
		bool SiblingSolver::validate<StatementAst, StatementAst>(
			StatementAst *left, StatementAst *right)
		{
			StatementVisitorGenerator<SiblingSolver, 
				PartialValidateVisitorTraits<SiblingSolver> > visitor(this, right);
			left->accept_statement_visitor(&visitor);
			return visitor.return_result();

			//Alternative: Soares please choose which one to take
			if(left == right){
				return false;
			}

			else{
				StatementAst *statement_ast = left;

				//Go through in forward manner
				while(statement_ast->next() != NULL) {
					if(statement_ast->next() == right) {
						return true;
					}
					statement_ast = statement_ast->next();
				}

				statement_ast = left;
				while(statement_ast->prev()!= NULL){
					if(statement_ast->prev() == right){
						return true;
					}
					statement_ast = statement_ast->prev();
				}
				return false;
			}
		}

		template<>
		bool SiblingSolver::validate<ProcAst, ProcAst>
			(ProcAst *left, ProcAst *right){
				if(left == right){
					return false;
				}
				else{
					return true;
				}
		}

		template<>
		bool SiblingSolver::validate<ExprAst, ExprAst>(ExprAst *left, 
			ExprAst *right){
				if(left == right){
					return false;
				}

				ExprAst *expr_ast = left;
		}

		template<>
		bool SiblingSolver::validate<VariableAst, ExprAst>(VariableAst *left,
			ExprAst *right){
				return false;

		}

		template<>
		bool SiblingSolver::validate<ExprAst, VariableAst>(ExprAst *left, 
			VariableAst *right);

		void SiblingSolver::index_proc(ProcAst *proc) {
			index_statement_list(proc->get_statement());
		}

		void SiblingSolver::index_statement_list(StatementAst *statement) {
			while(statement!=NULL) {
				index_statement(statement);
				statement = statement->next();
			}
		}

		void SiblingSolver::index_statement(StatementAst *statement) {
			switch(get_statement_type(statement)) {
			case AssignST:
				index_assign(statement_cast<AssignmentAst>(statement));
				break;

			case WhileST:
				index_while(statement_cast<WhileAst>(statement));
				break;

			case IfST:
				index_if(statement_cast<IfAst>(statement));
				break;

			default:
				// noop
				break;
			}
		}

		void SiblingSolver::index_while(WhileAst *while_ast) {
			index_statement_list(while_ast->get_body());
		}

		void SiblingSolver::index_if(IfAst *if_ast) {
			index_statement_list(if_ast->get_then_branch());
			index_statement_list(if_ast->get_else_branch());
		}

		void SiblingSolver::index_assign(AssignmentAst *assign_ast) 
		{
			SimpleVariable* variable = assign_ast->get_variable();
			
			int statement_number = assign_ast->get_statement_line();

			string variable_name = variable->get_name();

			ExprAst *expr_ast = assign_ast->get_expr();
			string expr_string = extract_string_from_expr(expr_ast);

			
		}

		void SiblingSolver::index_expr(ExprAst* expr_ast, int statement_num) 
		{
			if(get_expr_type(expr_ast) == BinaryOpET){

			}
		}

		string SiblingSolver::extract_string_from_expr(ExprAst* expr_ast)
		{
			string expr_string;
			switch(get_expr_type(expr_ast)){
				//BinaryOpET,
				//	VariableET,
				//	ConstantET
			case ConstantET:
				{
					ConstAst *constant_ast = expr_cast<ConstAst>(expr_ast);
					int constant_value = constant_ast->get_value();
					
					expr_string = to_string(static_cast<long long>(
						constant_value));

					break;
				}

			case VariableET:
				{
					VariableAst *variable_ast = expr_cast<VariableAst>(
						expr_ast);

					SimpleVariable *variable = variable_ast->get_variable();
					expr_string = variable->get_name();
					break;
				}

			case BinaryOpET:
				{
					BinaryOpAst *binary_ops_ast = expr_cast<BinaryOpAst>(
						expr_ast);
					
					expr_string = binary_ops_ast->get_op();
					break;
				}
			}

			return expr_string;
		}

	}
}