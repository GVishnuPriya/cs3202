#include "gtest/gtest.h"
#include "simple/util/ast_utils.h"
#include "impl/solvers/uses.h"
#include "impl/ast.h"
#include "impl/condition.h"
#include "simple/util/solver_generator.h"
#include "test/fixtures/uses_fixtures.h"

namespace spa  {
namespace test{

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

//void 
//	TestUses::setUp(){
//}
//
//void 
//	TestUses::tearDown(){
//}
//
//// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION( TestUses ); // Note 4 

TEST (UsesTest, Single_Varible) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * x = 1;
	 * x = x + 1;}
	 */
	AST *ast = uses_fixture_1();    

	SolverPtr uses_solver(new SimpleSolverGenerator<UsesSolver>(
		new UsesSolver(ast->get_root())));
 	Uses uses(uses_solver, ast);

	EXPECT_TRUE(uses.validate_uses("First", "x"));
	EXPECT_TRUE(uses.validate_uses(2,"x"));

	VarResults variable_set = uses.get_used_vars(2);
	VarResults::iterator iter = variable_set.begin();
	EXPECT_EQ("x",*iter);

	variable_set = uses.get_used_vars("First");
	iter = variable_set.begin();
	EXPECT_EQ("x",*iter);

	StatementResults statement_set = uses.get_using_statements("x");
	StatementResults::iterator int_iter = statement_set.begin();
	EXPECT_EQ(2,*int_iter);

	//TODO: Uncomment this when Pauline implement this
	//std::set<Proc> procedure_set = uses.get_using_procs("x");
	//std::set<Proc>::iterator proc_iter = procedure_set.begin();
	//EXPECT_EQ("First", *proc_iter);
}

#if 0
TEST (UsesTest, Multiple_Variables_Use_1) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * x = 1;
	 * y = x + 1;}
	 */

	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varX("x");
	SimpleVariable varY("y");

	//1st statement
	SimpleAssignmentAst *stmt_1 = new SimpleAssignmentAst();
	stmt_1->set_variable(varX);
	stmt_1->set_expr(new SimpleConstAst(1));
	stmt_1->set_line(1);
	set_proc(stmt_1, procedure);
	procedure->set_first_statement(stmt_1);

	//2nd statement
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst();
	stmt_2->set_variable(varY);
	SimpleBinaryOpAst *expression = new SimpleBinaryOpAst('+', varX, new SimpleConstAst(1));
	stmt_2->set_expr(expression);
	stmt_2->set_line(2);
	stmt_2->set_prev(stmt_1);
	stmt_2->set_proc(procedure);

	stmt_1->set_next(stmt_2);
	SimpleRoot root(procedure);	//Set this procedure as the first procedure

	//FIXME: Check with Soares what to put inside
	UsesSolver solver(); 
	AST *ast = new AST(root, NULL);

	//Uses *test_uses = new Uses(solver,ast);
	spa::Uses test_uses(solver,ast);

	EXPECT_TRUE(test_uses->validate_uses(procedure, varX));
	EXPECT_TRUE(test_uses->validate_uses(2,varX));

	set<Var> variable_set = test_uses->get_used_vars(2);
	set<Var> iter = variable_set.begin();
	EXPECT_EQ(var,*iter);

	set<Var> variable_set = test_uses->get_used_vars(procedure);
	set<Var> iter = variable_set.begin();
	EXPECT_EQ(var,*iter);

	std::set<int> statement_set = test_uses.get_using_statements(varX);
	std::set<int>::iterator iter = statement_set.begin();
	EXPECT_EQ(2,*iter);

	std::set<Proc> procedure_set = test_uses.get_using_procs(varX);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ(procedure, *iter);
}

TEST (UsesTest, MultipleVariables_MultipleUsesInTermOfVariablesAndStatement) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * x = 1;
	 * y = x + 1;
	 * x = y + x;}
	 */

	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varX("x");
	SimpleVariable varY("y");

	//1st statement
	SimpleAssignmentAst *stmt_1 = new SimpleAssignmentAst();
	stmt_1->set_variable(varX);
	stmt_1->set_expr(new SimpleConstAst(1));
	stmt_1->set_line(1);
	stmt_1->set_proc(procedure);
	procedure->set_first_statement(stmt_1);

	//2nd statement
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst();
	stmt_2->set_variable(varY);
	SimpleBinaryOpAst *expression_2 = new SimpleBinaryOpAst('+', varX, new SimpleConstAst(1));
	stmt_2->set_expr(expression_2);
	stmt_2->set_line(2);
	stmt_2->set_prev(stmt_1);
	stmt_2->set_proc(procedure);

	stmt_1->set_next(stmt_2);
	//3rd statement
	SimpleAssignmentAst *stmt_3 = new SimpleAssignmentAst();
	stmt_3->set_variable(varX);
	SimpleBinaryOpAst *expression_3 = new SimpleBinaryOpAst('+', varX, varY);
	stmt_3->set_expr(expression_3);
	stmt_3->set_line(3);
	stmt_3->set_prev(stmt_2);
	stmt_3->set_proc(procedure);

	stmt_2->set_next(stmt_3);

	//FIXME: Check with Soares what to put inside
	UsesSolver solver(); 
	AST *ast = new AST(root, NULL);

	//Uses *test_uses = new Uses(solver,ast);
	spa::Uses test_uses(solver,ast);

	//Validate statement 2
	EXPECT_TRUE(test_uses->validate_uses(procedure, varX));
	EXPECT_TRUE(test_uses->validate_uses(2,varX));

	std::set<Var> variable_set = test_uses->get_used_vars(2);
	std::set<Var>::iterator iter = variable_set.begin();
	EXPECT_EQ(varX,*iter);

	//Validate statement 3
	EXPECT_TRUE(test_uses->validate_uses(procedure, varY));
	EXPECT_TRUE(test_uses->validate_uses(3,varX));
	EXPECT_TRUE(test_uses->validate_uses(3,varY));

	std::set<Var> variable_set = test_uses->get_used_vars(3);
	std::set<Var>::iterator iter = variable_set.begin();
	EXPECT_EQ(varX,*iter);
	++iter;
	EXPECT_EQ(varY,*iter);

	//Program wide
	std::set<Var> variable_set = test_uses->get_used_vars(procedure);
	std::set<Var>::iterator iter = variable_set.begin();
	EXPECT_EQ(varX,*iter);
	++iter;
	EXPECT_EQ(varY,*iter);

	std::set<int> statement_set = test_uses.get_using_statements(varX);
	std::set<int>::iterator iter = statement_set.begin();
	EXPECT_EQ(2,*iter);
	++iter;
	EXPECT_EQ(3, *iter);

	std::set<int> statement_set = test_uses.get_using_statements(varY);
	std::set<int>::iterator iter = statement_set.begin();
	EXPECT_EQ(3,*iter);

	std::set<Proc> procedure_set = test_uses.get_using_procs(varX);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ(procedure, *iter);

	std::set<Proc> procedure_set = test_uses.get_using_procs(varY);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ(procedure, *iter);
}

TEST (UsesTest, MultipleVariables_WhileStatement) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * while x {
	 *	x = x + y; }}
	 */

	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varX("x");
	SimpleVariable varY("y");

	SimpleWhileAst *while_stmt = new SimpleWhileAst();
	while_stmt->set_variable(varX);
	while_stmt->set_line(1);
	while_stmt->set_proc(procedure);

	//Check with Soares if while is consider a statement or not
	//FIXME: Currently, while is a valid statement number
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst(2);
	SimpleBinaryOpAst *expression = new SimpleBinaryOpAst('+', varX, varY);
	stmt_2->set_variable(varX);
	stmt_2->set_expr(expression);
	stmt_2->set_proc(procedure);
	
	procedure->set_first_statement(while_stmt);
	while_stmt->set_body(stmt_2);

	//FIXME: Check with Soares what to put inside
	UsesSolver solver(); 
	AST *ast = new AST(root, NULL);
	spa::Uses test_uses(solver,ast);

	//Validate while
	EXPECT_TRUE(test_uses->validate_uses(1, varX));
	

	//Validate statement 2
	EXPECT_TRUE(test_uses->validate_uses(2, varX));
	EXPECT_TRUE(test_uses->validate_uses(2, varY));

	std::set<Var> variable_set = test_uses->get_used_vars(2);
	std::set<Var>::iterator iter = variable_set.begin();
	EXPECT_EQ(varX,*iter);
	++iter;
	EXPECT_EQ(varY,*iter);
	
	//Program wide validation
	EXPECT_TRUE(test_uses->validate_uses((procedure, varX)));
	EXPECT_TRUE(test_uses->validate_uses((procedure, varY)));

	std::set<int> statement_set = test_uses.get_using_statements(varX);
	std::set<int>::iterator iter = statement_set.begin();
	EXPECT_EQ(1,*iter);
	++iter;
	EXPECT_EQ(2, *iter);

	std::set<int> statement_set = test_uses.get_using_statements(varY);
	std::set<int>::iterator iter = statement_set.begin();
	EXPECT_EQ(2,*iter);

	std::set<Proc> procedure_set = test_uses.get_using_procs(varX);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ(procedure, *iter);

	std::set<Proc> procedure_set = test_uses.get_using_procs(varY);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ(procedure, *iter);
}

TEST (UsesTest, MultipleVariables_IfStatement) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * if x then{
	 *	x = x + y; }
	 * else{
	 *  x = x + 1;}}
	 */
}

TEST (UsesTest, MultipleVariables_WhileNestedIfInside) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1.	while y {
	 * 2.		if x then{
	 * 3.			x = x + y; }
	 *			else{
	 * 4.			x = x + 1;}
	 *		}
	 */
}

TEST (UsesTest, MultipleVariables_IfNestedWhileInside) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1.	if x then {
	 * 2.		while x {
	 * 3.			x = x + 1;}
	 *		else{
	 * 4.		while y {
	 * 5.			x = x + y;}
	 *		}
	 */
}

TEST (UsesTest, MultipleVariables_MultipleProcedure_SingleVariable) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1. x = x + 1; }
	 *
	 * procedure Second {
	 * 1. x = x; }
	 */
}

TEST (UsesTest, MultipleVariables_MultipleProcedure_MultipleVariables) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1. x = x + 1; 
	 * 2. y = x + y;}
	 *
	 * procedure Second {
	 * 1. x = x; }
	 */
}

TEST (UsesTest, MultipleVariables_MultipleProcedure_Call) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1. x = x + 1; 
	 * 2. y = x + y;
	 * 3. Call Second;}
	 *
	 * procedure Second {
	 * 1. x = x; }
	 */
}

#endif
}
}