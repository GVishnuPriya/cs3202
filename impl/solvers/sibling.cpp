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

SiblingSolver::SiblingSolver(SimpleRoot ast) : _ast(ast) {
    ProcSet proc_set;

    //Construct a complete Set on all the procedure in the source code
    for (auto it = _ast.begin(); it!= _ast.end(); ++it) {
        proc_set.insert(*it);
        index_proc(*it);
    }

    //Fill up the Sibling table for procedures
    for(auto it = proc_set.begin(); it!=proc_set.end(); ++it){
        ProcSet proc_sibling_set = proc_set;
        proc_sibling_set.erase(*it);
        _sibling_proc_index[*it] = proc_sibling_set;
    }
}

void SiblingSolver::index_proc(ProcAst *proc) {
    index_statement_list(proc->get_statement());
}

void SiblingSolver::index_statement_list(StatementAst *statement) {
    StatementSet statement_set;

    //Construct the complete set on all the statements on the same
    //nesting level in the source code
    while(statement != NULL)
    {
        statement_set.insert(statement);
        index_statement(statement);
        statement = statement->next();
    }

    //Fill up the Sibling table for statements
    for(auto it = statement_set.begin(); it!= statement_set.end(); ++it)
    {
        StatementSet statement_sibling_set = statement_set;
        statement_sibling_set.erase(*it);
        _sibling_statement_index[*it] = statement_sibling_set;
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
    //Fill up the Sibling Table for container
    SimpleVariable while_variable(while_ast->get_variable()->get_name());
    SimpleVariableAst variable_ast(while_variable);
    _sibling_container_index[&variable_ast].insert(while_ast->get_body());

	//Fill up the Sibling Table for container (inverse)
	_sibling_container_inverse_index[while_ast->get_body()].insert(&variable_ast);

    index_statement_list(while_ast->get_body());
}

void SiblingSolver::index_if(IfAst *if_ast) {
    //Fill up the Sibling Table for container
    SimpleVariable if_variable(if_ast->get_variable()->get_name());
    SimpleVariableAst variable_ast(if_variable);
    _sibling_container_index[&variable_ast].insert(if_ast->get_then_branch());
    _sibling_container_index[&variable_ast].insert(if_ast->get_else_branch());

	//Fill up the Sibling Table for container (inverse)
	_sibling_container_inverse_index[if_ast->get_then_branch()].insert(&variable_ast);
	_sibling_container_inverse_index[if_ast->get_else_branch()].insert(&variable_ast);

	//Fill up the Sibling Table for statement
	_sibling_statement_index[if_ast->get_then_branch()].insert(if_ast->get_else_branch());
	_sibling_statement_index[if_ast->get_else_branch()].insert(if_ast->get_then_branch());

    index_statement_list(if_ast->get_then_branch());
    index_statement_list(if_ast->get_else_branch());
}

void SiblingSolver::index_assign(AssignmentAst *assign_ast) 
{
    SimpleVariable *variable = assign_ast->get_variable();
    SimpleVariableAst variable_ast(*variable);

    ExprAst *expr_ast = assign_ast->get_expr();
    string expr_string = extract_string_from_expr(expr_ast);

    //Filling the Sibling table for expressions
    _sibling_expression_index[&variable_ast].insert(expr_ast);
    _sibling_expression_index[expr_ast].insert(&variable_ast);

    _sibling_index[variable->get_name()].insert(expr_string);
    _sibling_index[expr_string].insert(variable->get_name());

    index_expr(expr_ast);
}

void SiblingSolver::index_expr(ExprAst* expr_ast) 
{
    if(get_expr_type(expr_ast) == BinaryOpET){
        BinaryOpAst *binary_ops_ast = expr_cast<BinaryOpAst>(
            expr_ast);

        ExprAst *left_expr = binary_ops_ast->get_lhs();
        ExprAst *right_expr = binary_ops_ast->get_rhs();

        //Filling the Sibling table for expressions
        _sibling_expression_index[binary_ops_ast->get_lhs()].insert(
            binary_ops_ast->get_rhs());
        _sibling_expression_index[binary_ops_ast->get_rhs()].insert(
            binary_ops_ast->get_lhs());

        _sibling_index[extract_string_from_expr(left_expr)].insert(
            extract_string_from_expr(right_expr));
        _sibling_index[extract_string_from_expr(right_expr)].insert(
            extract_string_from_expr(left_expr));

        index_expr(left_expr);
        index_expr(right_expr);
    }
}

string SiblingSolver::extract_string_from_expr(ExprAst* expr_ast)
{
    string expr_string;
    switch(get_expr_type(expr_ast)){
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

    default:
        break;
    }
    return expr_string;
}

template <>
bool SiblingSolver::validate<StatementAst, StatementAst>(
    StatementAst *left, StatementAst *right)
{
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
        else if( (_ast.get_proc(left->get_name()) != NULL)
            && (_ast.get_proc(right->get_name()) != NULL)
            ){    //Check that both procedure is inside AST

                return true;
        }
        else
        {
            return false;
        }
}

template<>
 bool SiblingSolver::validate<VariableAst, StatementAst>(VariableAst *left,
 	StatementAst *right){
		if(_sibling_container_index.count(left) == 0)
		{
			return false;
		}
		else if(_sibling_container_index[left].count(right) == 0)
		{
			return false;
		}
		else{
			return true;
		}
 }

template<>
bool SiblingSolver::validate<StatementAst, VariableAst>(StatementAst *left,
	VariableAst *right){
		if(_sibling_container_inverse_index.count(left) == 0)
		{
			return false;
		}
		else if(_sibling_container_inverse_index[left].count(right) == 0)
		{
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
        else if(_sibling_index[extract_string_from_expr(left)].count(
            extract_string_from_expr(right)) > 0)
        {
            return true;
        }
        else 
        {
            return false;
        }
}

//Procedure
template<>
ConditionSet SiblingSolver::solve_left<ProcAst>(ProcAst *proc)
{
    return proc_set_to_condition_set(_sibling_proc_index[proc]);
}

template<>
ConditionSet SiblingSolver::solve_right<ProcAst>(ProcAst *proc)
{
    return proc_set_to_condition_set(_sibling_proc_index[proc]);
}

//Statement
template <>
ConditionSet SiblingSolver::solve_left<StatementAst>(
    StatementAst *statement)
{
    return statement_set_to_condition_set(_sibling_statement_index[statement]);
}

template <>
ConditionSet SiblingSolver::solve_right<StatementAst>(
    StatementAst *statement)
{
    return statement_set_to_condition_set(_sibling_statement_index[statement]);
}

//Container and variable
template<>
ConditionSet SiblingSolver::solve_left<ContainerAst>(
	ContainerAst *container)
{
		switch(get_statement_type(container))
		{
		case IfST:
			return expr_set_to_condition_set(_sibling_container_inverse_index[statement_cast<IfAst>(container)->get_then_branch()]);
		
		case WhileST:
			return expr_set_to_condition_set(_sibling_container_inverse_index[statement_cast<WhileAst>(container)->get_body()]);

		default:
			return NULL;
		}
}

template<>
ConditionSet SiblingSolver::solve_right<ContainerAst>(
	ContainerAst *container)
{
		switch(get_statement_type(container))
		{
		case IfST:
			return expr_set_to_condition_set(_sibling_container_inverse_index[statement_cast<IfAst>(container)->get_then_branch()]);
		
		case WhileST:
			return expr_set_to_condition_set(_sibling_container_inverse_index[statement_cast<WhileAst>(container)->get_body()]);

		default:
			return NULL;
		}
}

template<>
ConditionSet SiblingSolver::solve_left<VariableAst>(
	VariableAst *variable){
		return statement_set_to_condition_set(_sibling_container_index[variable]);
}

template<>
ConditionSet SiblingSolver::solve_right<VariableAst>(
	VariableAst *variable){
		return statement_set_to_condition_set(_sibling_container_index[variable]);
}

//Expression
template<>
ConditionSet SiblingSolver::solve_left<ExprAst>(
	ExprAst *expr)
{
	return expr_set_to_condition_set(_sibling_expression_index[expr]);
}

template<>
ConditionSet SiblingSolver::solve_right<ExprAst>(
	ExprAst *expr)
{
	return expr_set_to_condition_set(_sibling_expression_index[expr]);
}
}
}