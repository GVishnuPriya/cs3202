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

#include "gtest/gtest.h"
#include "simple/util/ast_utils.h"
#include "impl/solvers/sibling.h"
#include "impl/ast.h"
#include "impl/condition.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(SiblingTest, Test_Procedure) {
    /*
     * procedure first {
     *   x = y;
     * }
	 * procedure second {
	 * }
	 * procedure third{
	 * }
     */
    SimpleProcAst *proc_first = new SimpleProcAst("first");
	SimpleProcAst *proc_second = new SimpleProcAst("second");
	SimpleProcAst *proc_third = new SimpleProcAst("third");

    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");
    SimpleVariable var2("y"); // different name
    SimpleVariable var3("y"); // same name different pointer

    assign->set_variable(var);
    assign->set_expr(new SimpleVariableAst(var2));
    assign->set_line(1);
    assign->set_proc(proc_first);

    proc_first->set_first_statement(assign);

	std::vector<ProcAst*> procs;
    procs.push_back(proc_first);
    procs.push_back(proc_second);
	procs.push_back(proc_third);

    SimpleRoot root(procs.begin(), procs.end());

    SiblingSolver solver(root);

    /*
     * Positive testing
     */
    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc_first, proc_second)));	//Directly right sibling
	EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc_second, proc_first)));	//Directly left sibling
	EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc_third, proc_first)));	//Indirectly sibling


	/*
	 * Negative testing
	 */
	SimpleProcAst *proc_wrong = new SimpleProcAst("wrong");

	//Procedure in first parameter not inside AST
	EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc_wrong,
		proc_second)));	

	//Procedure in second parameter not inside AST
	EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc_second,
		proc_wrong)));

	//Cannot have same procedures in Sibling
	EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc_second, 
		proc_second)));	

	//Sibling with Procedure cannot mix with Assignment type
	EXPECT_FALSE((solver.validate<AssignmentAst, ProcAst>(assign,
		proc_second)));	

	/*
	 * Solve left and right
	 * Since solve left and solve right is symmetric, the test case and result is the same 
	 */
	ConditionSet result;

	//Left side
	result.clear();
	result.insert(new SimpleProcCondition(proc_second));
	result.insert(new SimpleProcCondition(proc_third));

	EXPECT_EQ(result, solver.solve_left<ProcAst>(proc_first));
	EXPECT_EQ(result, solver.solve_right<ProcAst>(proc_first));

	////Middle
	result.clear();
	result.insert(new SimpleProcCondition(proc_first));
	result.insert(new SimpleProcCondition(proc_third));

	EXPECT_EQ(result, solver.solve_left<ProcAst>(proc_second));
	EXPECT_EQ(result, solver.solve_right<ProcAst>(proc_second));

	//Right side
	result.clear();
	result.insert(new SimpleProcCondition(proc_first));
	result.insert(new SimpleProcCondition(proc_second));
	
	EXPECT_EQ(result, solver.solve_left<ProcAst>(proc_third));
	EXPECT_EQ(result, solver.solve_right<ProcAst>(proc_third));

	//External procedure
	result.clear();
	EXPECT_EQ(result, solver.solve_left<ProcAst>(proc_wrong));
	EXPECT_EQ(result, solver.solve_right<ProcAst>(proc_wrong));
}

TEST(SiblingTest, Test_Assignment) {
	/*
	 * procedure first {
	 *  x = 2;
	 *  z = 3;
	 * }
	 * procedure second {
	 *  x = 0;
	 *  z = 3;
	 *  i = 5;
	 * }
	 */
    SimpleProcAst *proc_first = new SimpleProcAst("first");
	SimpleProcAst *proc_second = new SimpleProcAst("second");
	
	// x = 2
    SimpleAssignmentAst *assign1 = new SimpleAssignmentAst();
    SimpleVariable varX("x");
	SimpleConstant const2(2);

    assign1->set_variable(varX);
	assign1->set_expr(new SimpleConstAst(2));
    assign1->set_line(1);
    assign1->set_proc(proc_first);

    proc_first->set_first_statement(assign1);

	// z = 3
	SimpleAssignmentAst *assign2 = new SimpleAssignmentAst();
    SimpleVariable varZ("z");
	SimpleConstant const3(3);

	assign2->set_variable(varZ);
	assign2->set_expr(new SimpleConstAst(3));
    assign2->set_line(2);
    assign2->set_proc(proc_first);

	set_next(assign1, assign2);

	// x = 0
	SimpleAssignmentAst *assign3 = new SimpleAssignmentAst();
	SimpleConstant const0(0);

	assign3->set_variable(varX);
	assign3->set_expr(new SimpleConstAst(0));
    assign3->set_line(3);
    assign3->set_proc(proc_second);

	proc_second->set_first_statement(assign3);

	//z = 3
	SimpleAssignmentAst *assign4 = new SimpleAssignmentAst();

	assign4->set_variable(varZ);
	assign4->set_expr(new SimpleConstAst(4));
    assign4->set_line(4);
    assign4->set_proc(proc_second);

	set_next(assign3, assign4);

	//i = 5
	SimpleAssignmentAst *assign5 = new SimpleAssignmentAst();
	SimpleVariable varI("i");
	SimpleConstant const5(5);

	assign5->set_variable(varI);
	assign5->set_expr(new SimpleConstAst(5));
    assign5->set_line(5);
    assign5->set_proc(proc_second);

	set_next(assign4, assign5);

	std::vector<ProcAst*> procs;
    procs.push_back(proc_first);
    procs.push_back(proc_second);

    SimpleRoot root(procs.begin(), procs.end());

    SiblingSolver solver(root);

	/*
	 * Positive testing
	 */
	EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(assign1, assign2)));	//Directly right sibling
	EXPECT_TRUE((solver.validate<StatementAst, StatementAst>(assign2, assign1)));	//Directly left sibling

	/*
	 * Negative testing
	 */
	EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(assign1, assign3)));	//Not inside the same procedure
	EXPECT_FALSE((solver.validate<StatementAst, StatementAst>(assign4, assign2)));	//Directly right sibling

	/*
	 * Solve left and right
	 * Since solve left and solve right is symmetric, the test case and result is the same 
	 */
	ConditionSet result;

	//Left
	result.clear();
	result.insert(new SimpleStatementCondition(assign2));
	EXPECT_EQ(result, solver.solve_left<StatementAst>(assign1));

	//Right
	result.clear();
	result.insert(new SimpleStatementCondition(assign1));
	EXPECT_EQ(result, solver.solve_left<StatementAst>(assign2));

	//Middle
	result.clear();
	result.insert(new SimpleStatementCondition(assign3));
	result.insert(new SimpleStatementCondition(assign5));
	EXPECT_EQ(result, solver.solve_left<StatementAst>(assign4));


	//Verify that it is not inter-procedure
	result.clear();
	result.insert(new SimpleStatementCondition(assign1));
	result.insert(new SimpleStatementCondition(assign2));
	result.insert(new SimpleStatementCondition(assign3));
	EXPECT_NE(result, solver.solve_left<StatementAst>(assign4));
}

TEST(SiblingTest, Test_Expression) {
	/*
	 * procedure first{
	 *  x = x + 7 * g + y * a * 2;
	 * }
	 */

	SimpleProcAst *proc_first = new SimpleProcAst("first");

    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable varX("x");
    SimpleVariable varY("y");
	SimpleVariable varG("g");
	SimpleVariable varA("a");

	SimpleConstant const7(7);
	SimpleConstant const2(2);

	SimpleBinaryOpAst *plus_first = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *plus_second = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *time_first = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *time_second = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *time_third = new SimpleBinaryOpAst();

	// 7 * g
	time_first->set_op('*');
	time_first->set_lhs(new SimpleConstAst(7));
	time_first->set_rhs(new SimpleVariableAst(varG));

	// y * a
	time_second->set_op('*');
	time_second->set_lhs(new SimpleVariableAst(varY));
	time_second->set_rhs(new SimpleVariableAst(varA));

	// x + 7 * g
	plus_first->set_op('+');
	plus_first->set_lhs(new SimpleVariableAst(varX));
	plus_first->set_rhs(time_first);

	// y * a * 2
	time_third->set_op('*');
	time_third->set_lhs(time_second);
	time_third->set_rhs(new SimpleConstAst(2));

	// x + 7 * g + y * a * 2
	plus_second->set_op('+');
	plus_second->set_lhs(plus_first);
	plus_second->set_rhs(time_third);

	// x = x + 7 * g + y * a * 2
    assign->set_variable(varX);
	assign->set_expr(plus_second);
    
    assign->set_line(1);
    assign->set_proc(proc_first);

    proc_first->set_first_statement(assign);

	SimpleRoot root(proc_first);

    SiblingSolver solver(root);

	/*
	 * Positive testing
	 */
	SimpleBinaryOpAst *plus = new SimpleBinaryOpAst();
	plus->set_op('+');

	SimpleBinaryOpAst *times = new SimpleBinaryOpAst();
	times->set_op('*');

	//Variable and top expression (and swap)
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varX), plus)));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(plus, new SimpleVariableAst(varX))));

	//2nd Level Operator (and swap)
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(plus, times)));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(times, plus)));

	//3rd Level (and swap)
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varX), times)));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(times, new SimpleVariableAst(varX))));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(times, new SimpleConstAst(2))));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(new SimpleConstAst(2), times)));

	//4th Level (and swap)
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varG), new SimpleConstAst(7))));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(new SimpleConstAst(7), new SimpleVariableAst(varG))));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varY), new SimpleVariableAst(varA))));
	EXPECT_TRUE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varA), new SimpleVariableAst(varY))));

	/*
	 * Negative testing
	 */
	//Operators at different nesting level
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(plus, plus)));	

	//Same constant/ variable
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varY), new SimpleVariableAst(varY))));
	
	//Constant and Operators at different nesting level (and swap)
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleConstAst(2), plus)));	
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(plus, new SimpleConstAst(2))));

	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleConstAst(7), plus)));	
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(times, new SimpleConstAst(7))));	

	//Variable and operators at differents nesting level
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varY), times)));
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(plus, new SimpleVariableAst(varY))));

	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varG), times)));
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(plus, new SimpleVariableAst(varG))));

	//Variables and constant at different branches
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varX), new SimpleConstAst(2))));
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleConstAst(2), new SimpleVariableAst(varX))));

	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleConstAst(7), new SimpleVariableAst(varY))));
	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleVariableAst(varA), new SimpleVariableAst(varG))));

	EXPECT_FALSE((solver.validate<ExprAst, ExprAst>(new SimpleConstAst(7), new SimpleConstAst(2))));
}
}
}
