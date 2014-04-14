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
    SimpleVariable *assign_variable = assign_ast->get_variable();
	SimpleVariableAst assign_variable_ast(assign_variable->get_name());
    ExprAst *expr_ast = assign_ast->get_expr();

	switch(get_expr_type(expr_ast))
	{
	case VariableET:
		{
			VariableAst *variable = expr_cast<VariableAst>(expr_ast);

			_sibling_expression_index[&assign_variable_ast].insert(variable);
			_sibling_expression_index[variable].insert(&assign_variable_ast);
			break;
		}
	case ConstantET:
		{
			ConstAst *constant = expr_cast<ConstAst>(expr_ast);

			_sibling_expression_index[&assign_variable_ast].insert(constant);
			_sibling_expression_index[constant].insert(&assign_variable_ast);
			break;
		}
	case BinaryOpET:
		{
			BinaryOpAst *binary = expr_cast<BinaryOpAst>(expr_ast);
			SimpleBinaryOpAst *simple_binary = new SimpleBinaryOpAst();
			simple_binary->set_op(binary->get_op());

			_sibling_expression_index[&assign_variable_ast].insert(simple_binary);
			_sibling_expression_index[simple_binary].insert(&assign_variable_ast);

			index_expr(binary);
			break;
		}
	default:
		break;
	}

	string expr_string = top_node_from_expression(expr_ast);

	_sibling_index[assign_variable->get_name()].insert(expr_string);
	_sibling_index[expr_string].insert(assign_variable->get_name());
}

string SiblingSolver::top_node_from_expression(ExprAst *expr_ast)
{
	switch(get_expr_type(expr_ast))
	{
	case VariableET:
		{
			VariableAst *variable = expr_cast<VariableAst>(expr_ast);
			return variable->get_variable()->get_name();
		}
	case ConstantET:
		{
			ConstAst *constant = expr_cast<ConstAst>(expr_ast);
			return to_string(static_cast<long long>(constant->get_constant()->get_int()));
		}
	case BinaryOpET:
		{
			BinaryOpAst *binary = expr_cast<BinaryOpAst>(expr_ast);
			return string (1, binary->get_op());
		}
	default:
		return "";
	}
}

void SiblingSolver::index_expr(ExprAst* expr_ast) 
{
	//Need to break down each of the expression into individual expression
	//AST, or else there is no way to store them effectively to do solve 
	//left and right

    if(get_expr_type(expr_ast) == BinaryOpET){
        BinaryOpAst *binary_ops_ast = expr_cast<BinaryOpAst>(
            expr_ast);

        ExprAst *left_expr = binary_ops_ast->get_lhs();
        ExprAst *right_expr = binary_ops_ast->get_rhs();

		//9 different variation
		// Variable, Operator, Constant all can mix and match

		switch(get_expr_type(left_expr))
		{
		case VariableET:
			{
				VariableAst *variable = expr_cast<VariableAst>(expr_ast);
				index_variable(variable, right_expr);
				break;
			}
		case ConstantET:
			{
				ConstAst *constant = expr_cast<ConstAst>(expr_ast);
				index_constant(constant, right_expr);
				break;
			}
		case BinaryOpET:
			{
				BinaryOpAst *binary = expr_cast<BinaryOpAst>(expr_ast);
				SimpleBinaryOpAst *simple_binary = new SimpleBinaryOpAst();
				simple_binary->set_op(binary->get_op());

				index_binary_op(simple_binary, right_expr);
				break;
			}
		default:
			break;
		}

        _sibling_index[top_node_from_expression(left_expr)].insert(
            top_node_from_expression(right_expr));
        _sibling_index[top_node_from_expression(right_expr)].insert(
            top_node_from_expression(left_expr));

        index_expr(left_expr);
        index_expr(right_expr);
    }
}

void SiblingSolver::index_variable(VariableAst *variable_ast, ExprAst *expr_ast)
{
	switch(get_expr_type(expr_ast))
	{
	case VariableET:
		{
			VariableAst *variable = expr_cast<VariableAst>(expr_ast);

			_sibling_expression_index[variable_ast].insert(variable);
			_sibling_expression_index[variable].insert(variable_ast);
			break;
		}
	case ConstantET:
		{
			ConstAst *constant = expr_cast<ConstAst>(expr_ast);

			_sibling_expression_index[variable_ast].insert(constant);
			_sibling_expression_index[constant].insert(variable_ast);
			break;
		}
	case BinaryOpET:
		{
			BinaryOpAst *binary = expr_cast<BinaryOpAst>(expr_ast);
			SimpleBinaryOpAst *simple_binary = new SimpleBinaryOpAst();
			simple_binary->set_op(binary->get_op());

			_sibling_expression_index[variable_ast].insert(simple_binary);
			_sibling_expression_index[simple_binary].insert(variable_ast);
			break;
		}
	default:
		break;
	}
}

void SiblingSolver::index_constant(ConstAst *constant_ast, ExprAst *expr_ast)
{
	switch(get_expr_type(expr_ast))
	{
	case VariableET:
		{
			VariableAst *variable = expr_cast<VariableAst>(expr_ast);

			_sibling_expression_index[constant_ast].insert(variable);
			_sibling_expression_index[variable].insert(constant_ast);
			break;
		}
	case ConstantET:
		{
			ConstAst *constant = expr_cast<ConstAst>(expr_ast);

			_sibling_expression_index[constant_ast].insert(constant);
			_sibling_expression_index[constant].insert(constant_ast);
			break;
		}
	case BinaryOpET:
		{
			BinaryOpAst *binary = expr_cast<BinaryOpAst>(expr_ast);
			SimpleBinaryOpAst *simple_binary = new SimpleBinaryOpAst();
			simple_binary->set_op(binary->get_op());

			_sibling_expression_index[constant_ast].insert(simple_binary);
			_sibling_expression_index[simple_binary].insert(constant_ast);
			break;
		}
	default:
		break;
	}
}

void SiblingSolver::index_binary_op(BinaryOpAst *binary_op_ast, ExprAst *expr_ast)
{
	switch(get_expr_type(expr_ast))
	{
	case VariableET:
		{
			VariableAst *variable = expr_cast<VariableAst>(expr_ast);

			_sibling_expression_index[binary_op_ast].insert(variable);
			_sibling_expression_index[variable].insert(binary_op_ast);
			break;
		}
	case ConstantET:
		{
			ConstAst *constant = expr_cast<ConstAst>(expr_ast);

			_sibling_expression_index[binary_op_ast].insert(constant);
			_sibling_expression_index[constant].insert(binary_op_ast);
			break;
		}
	case BinaryOpET:
		{
			BinaryOpAst *binary = expr_cast<BinaryOpAst>(expr_ast);
			SimpleBinaryOpAst *simple_binary = new SimpleBinaryOpAst();
			simple_binary->set_op(binary->get_op());

			_sibling_expression_index[binary_op_ast].insert(simple_binary);
			_sibling_expression_index[simple_binary].insert(binary_op_ast);
			break;
		}
	default:
		break;
	}
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
        else if(_sibling_index[top_node_from_expression(left)].count(
            top_node_from_expression(right)) > 0)
        {
            return true;
        }
        else 
        {
            return false;
        }
}

template<>
bool SiblingSolver::validate<OperatorCondition, OperatorCondition>(
	OperatorCondition *left, OperatorCondition *right)
{
	string left_operator(1, left->get_operator());
	string right_operator(1, right->get_operator());

	if(left_operator == right_operator)
	{
		return false;
	}
	else if(_sibling_index[left_operator].count(
		right_operator) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<>
bool SiblingSolver::validate<OperatorCondition, ExprAst>(
	OperatorCondition *left, ExprAst *right)
{
	string left_operator(1, left->get_operator());
	if(left_operator == top_node_from_expression(right))
	{
		return false;
	}
	else if(_sibling_index[left_operator].count(
		top_node_from_expression(right)) > 0)
	{
		return true;
	}
	else{
		return false;
	}
}

template<>
bool SiblingSolver::validate<ExprAst, OperatorCondition>(
	ExprAst *left, OperatorCondition *right)
{
	string right_operator(1, right->get_operator());
	string left_expr(top_node_from_expression(left));
	if(right_operator == left_expr)
	{
		return false;
	}
	else if(_sibling_index[right_operator].count(
		left_expr) > 0)
	{
		return true;
	}
	else{
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
			return ConditionSet();
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
			return ConditionSet();
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
	switch(get_expr_type(expr))
	{
	case VariableET:
		{
			VariableAst *variable = expr_cast<VariableAst>(expr);
			return expr_set_to_condition_set(_sibling_expression_index[variable]);
		}
	case ConstantET:
		{
			ConstAst *constant = expr_cast<ConstAst>(expr);
			return expr_set_to_condition_set(_sibling_expression_index[constant]);
		}
	case BinaryOpET:
		{
			BinaryOpAst *binary = expr_cast<BinaryOpAst>(expr);
			SimpleBinaryOpAst *simple_binary = new SimpleBinaryOpAst();
			simple_binary->set_op(binary->get_op());

			return expr_set_to_condition_set(_sibling_expression_index[simple_binary]);
		}
	default:
    return ConditionSet();
		break;
	}
}

template<>
ConditionSet SiblingSolver::solve_right<ExprAst>(
	ExprAst *expr)
{
	return expr_set_to_condition_set(_sibling_expression_index[expr]);
}
}
}