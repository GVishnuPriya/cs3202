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


TEST (UsesTest, Multiple_Variables_Use_1) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * x = 1;
	 * y = x + 1;}
	 */
	AST *ast = uses_fixture_2();    

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
	StatementResults::iterator stmt_iter = statement_set.begin();
	EXPECT_EQ(2,*stmt_iter);

	/* Wait for Pauline to implement
	ProcResults procedure_set = uses.get_using_procs("x");
	ProcResults::iterator proc_iter = procedure_set.begin();
	EXPECT_EQ("First", *proc_iter);
	*/
}

TEST (UsesTest, MultipleVariables_MultipleUsesInTermOfVariablesAndStatement) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * x = 1;
	 * y = x + 1;
	 * x = y + x;}
	 */
	AST *ast = uses_fixture_3();

	SolverPtr uses_solver(new SimpleSolverGenerator<UsesSolver>(
		new UsesSolver(ast->get_root())));
 	Uses uses(uses_solver, ast);

	//Validate statement 2
	EXPECT_TRUE(uses.validate_uses("First", "x"));
	EXPECT_TRUE(uses.validate_uses(2,"x"));

	VarResults variable_set = uses.get_used_vars(2);
	VarResults::iterator var_iter = variable_set.begin();
	EXPECT_EQ("x",*var_iter);

	//Validate statement 3
	EXPECT_TRUE(uses.validate_uses("First", "y"));
	EXPECT_TRUE(uses.validate_uses(3,"x"));
	EXPECT_TRUE(uses.validate_uses(3,"y"));

	variable_set = uses.get_used_vars(3);
	var_iter = variable_set.begin();
	EXPECT_EQ("x",*var_iter);
	++var_iter;
	EXPECT_EQ("y",*var_iter);

	//Program wide
	variable_set = uses.get_used_vars("First");
	var_iter = variable_set.begin();
	EXPECT_EQ("x",*var_iter);
	++var_iter;
	EXPECT_EQ("y",*var_iter);

	StatementResults statement_set = uses.get_using_statements("x");
	StatementResults::iterator stmt_iter = statement_set.begin();
	EXPECT_EQ(2,*stmt_iter);
	++stmt_iter;
	EXPECT_EQ(3, *stmt_iter);

	statement_set = uses.get_using_statements("y");
	stmt_iter = statement_set.begin();
	EXPECT_EQ(3,*stmt_iter);

	/* Wait for Pauline to implement
	std::set<Proc> procedure_set = uses.get_using_procs("x");
	std::set<Proc>::iterator procedure_iter = procedure_set.begin();
	EXPECT_EQ("First", *procedure_iter);

	std::set<Proc> procedure_set = uses.get_using_procs(varY);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ("First", *iter);
	*/
}

TEST (UsesTest, MultipleVariables_WhileStatement) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1. while x {
	 * 2. 	x = x + y; }}
	 */

	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varX("x");
	SimpleVariable varY("y");

	SimpleWhileAst *while_stmt = new SimpleWhileAst();
	while_stmt->set_variable(varX);
	while_stmt->set_line(1);
	set_proc(while_stmt, procedure);
	
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst(2);
	SimpleBinaryOpAst *expression = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("x"), new SimpleVariableAst("y"));
	stmt_2->set_variable(varX);
	stmt_2->set_expr(expression);
	set_while_body(stmt_2, while_stmt);

	SimpleRoot root(procedure);	//Set this procedure as the first procedure

	LineTable line_table;
    line_table[1] = while_stmt;
    line_table[2] = stmt_2;

    StatementTable *statement_table = new StatementTable(line_table);
	AST *ast = new AST(root, statement_table);

	SolverPtr uses_solver(new SimpleSolverGenerator<UsesSolver>(
		new UsesSolver(ast->get_root())));
 	Uses uses(uses_solver, ast);

	//Validate while
	EXPECT_TRUE(uses.validate_uses(1, "x"));

	//Validate statement 2
	EXPECT_TRUE(uses.validate_uses(2, "x"));
	EXPECT_TRUE(uses.validate_uses(2, "y"));

	VarResults variable_set = uses.get_used_vars(2);
	VarResults::iterator variable_iter = variable_set.begin();
	EXPECT_EQ("x",*variable_iter);
	++variable_iter;
	EXPECT_EQ("y",*variable_iter);
	
	//Program wide validation
	EXPECT_TRUE(uses.validate_uses("First", "x"));
	EXPECT_TRUE(uses.validate_uses("First", "y"));

	StatementResults expected_statement_x;
	expected_statement_x.insert(1);
	expected_statement_x.insert(2);

	StatementResults statement_set_x = uses.get_using_statements("x");
	EXPECT_EQ(expected_statement_x, statement_set_x);

	StatementResults expected_statement_y;
	expected_statement_y.insert(1);
	expected_statement_y.insert(2);

	StatementResults statement_set_y = uses.get_using_statements("y");
	EXPECT_EQ(expected_statement_y, statement_set_y);
	
	/* Wait for Pauline to implement
	std::set<Proc> procedure_set = test_uses.get_using_procs(varX);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ(procedure, *iter);

	std::set<Proc> procedure_set = test_uses.get_using_procs(varY);
	std::set<Proc>::iterator iter = procedure_set.begin();
	EXPECT_EQ(procedure, *iter);
	*/
}

TEST (UsesTest, MultipleVariables_IfStatement) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1. if x then{
	 * 2.	x = x + y; }
	 * 	  else{
	 * 3.	x = x + 1;}}
	 */

	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varX("x");
	SimpleVariable varY("y");

	SimpleIfAst *if_stmt = new SimpleIfAst();
	if_stmt->set_variable(varX);
	if_stmt->set_line(1);
	set_proc(if_stmt, procedure);

	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst(2);
	SimpleBinaryOpAst *expression_2 = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("x"), new SimpleVariableAst("y"));
	stmt_2->set_variable(varX);
	stmt_2->set_expr(expression_2);
	set_then_branch(stmt_2, if_stmt);

	SimpleAssignmentAst *stmt_3 = new SimpleAssignmentAst(3);
	SimpleBinaryOpAst *expression_3 = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("x"), new SimpleConstAst(1));
	stmt_3->set_variable(varX);
	stmt_3->set_expr(expression_3);
	set_else_branch(stmt_3, if_stmt);

	SimpleRoot root(procedure);	//Set this procedure as the first procedure

	LineTable line_table;
    line_table[1] = if_stmt;
    line_table[2] = stmt_2;
    line_table[3] = stmt_3;

    StatementTable *statement_table = new StatementTable(line_table);
	AST *ast = new AST(root, statement_table);

	SolverPtr uses_solver(new SimpleSolverGenerator<UsesSolver>(
		new UsesSolver(ast->get_root())));
 	Uses uses(uses_solver, ast);

 	//Validate if
	EXPECT_TRUE(uses.validate_uses(1, "x"));

	//Validate statement 2
	EXPECT_TRUE(uses.validate_uses(1, "x"));
	EXPECT_TRUE(uses.validate_uses(1, "y"));

	//Program wide validation
	EXPECT_TRUE(uses.validate_uses("First", "x"));
	EXPECT_TRUE(uses.validate_uses("First", "y"));

	StatementResults expected_statement_x;
	expected_statement_x.insert(1);
	expected_statement_x.insert(2);
	expected_statement_x.insert(3);

	StatementResults statement_set_x = uses.get_using_statements("x");
	EXPECT_EQ(expected_statement_x, statement_set_x);

	StatementResults expected_statement_y;
	expected_statement_y.insert(1);
	expected_statement_y.insert(2);
	
	StatementResults statement_set_y = uses.get_using_statements("y");
	EXPECT_EQ(expected_statement_y, statement_set_y);

	/* Wait for Pauline to implement
	ProcResults procedure_set = uses.get_using_procs("x");
	ProcResults::iterator procedure_iter = procedure_set.begin();
	EXPECT_EQ("First", *procedure_iter);

	procedure_set = test_uses.get_using_procs("y");
	procedure_iter = procedure_set.begin();
	EXPECT_EQ("First", *procedure_iter);
	*/
}

TEST (UsesTest, MultipleVariables_WhileNestedIfInside) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1.	while a {
	 * 2.		if b then{
	 * 3.			c = d + e; }
	 *			else{
	 * 4.			f = g + 1;}
	 *		}
	 */

	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varA("a");
	SimpleVariable varB("b");
	SimpleVariable varC("c");
	SimpleVariable varD("d");
	SimpleVariable varE("e");
	SimpleVariable varF("f");
	SimpleVariable varG("g");

	SimpleWhileAst *while_stmt = new SimpleWhileAst();
	while_stmt->set_line(1);
	while_stmt->set_variable(varA);
	set_proc(while_stmt, procedure);

	SimpleIfAst *if_stmt = new SimpleIfAst();
	if_stmt->set_line(2);
	if_stmt->set_variable(varB);
	set_while_body(if_stmt, while_stmt);

	SimpleAssignmentAst *stmt_3 = new SimpleAssignmentAst(3);
	SimpleBinaryOpAst *expression_3 = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("d"), new SimpleVariableAst("e"));
	stmt_3->set_variable(varC);
	stmt_3->set_expr(expression_3);
	set_then_branch(stmt_3, if_stmt);

	SimpleAssignmentAst *stmt_4 = new SimpleAssignmentAst(4);
	SimpleBinaryOpAst *expression_4 = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("g"), new SimpleConstAst(1));
	stmt_4->set_variable(varF);
	stmt_4->set_expr(expression_4);
	set_else_branch(stmt_4, if_stmt);

	SimpleRoot root(procedure);	//Set this procedure as the first procedure

	LineTable line_table;
    line_table[1] = while_stmt;
    line_table[2] = if_stmt;
    line_table[3] = stmt_3;
    line_table[4] = stmt_4;

    StatementTable *statement_table = new StatementTable(line_table);
	AST *ast = new AST(root, statement_table);

	SolverPtr uses_solver(new SimpleSolverGenerator<UsesSolver>(
		new UsesSolver(ast->get_root())));
 	Uses uses(uses_solver, ast);

 	//Validate while
 	EXPECT_TRUE(uses.validate_uses(1, "a"));
 	EXPECT_TRUE(uses.validate_uses(1, "b"));
 	EXPECT_TRUE(uses.validate_uses(1, "d"));
 	EXPECT_TRUE(uses.validate_uses(1, "e"));
 	EXPECT_TRUE(uses.validate_uses(1, "g"));

 	VarResults expected_variable_set_1;
 	expected_variable_set_1.insert("a");
 	expected_variable_set_1.insert("b");
 	expected_variable_set_1.insert("d");
 	expected_variable_set_1.insert("e");
 	expected_variable_set_1.insert("g");

 	VarResults variable_set_1 = uses.get_used_vars(1);
 	EXPECT_EQ(expected_variable_set_1, variable_set_1);

 	//Validate if
 	EXPECT_TRUE(uses.validate_uses(2, "b"));
 	EXPECT_TRUE(uses.validate_uses(2, "d"));
 	EXPECT_TRUE(uses.validate_uses(2, "e"));

	VarResults expected_variable_set_2;
	expected_variable_set_2.insert("b");
 	expected_variable_set_2.insert("d");
 	expected_variable_set_2.insert("e");
 	expected_variable_set_2.insert("g");

 	VarResults variable_set_2= uses.get_used_vars(2);
 	EXPECT_EQ(expected_variable_set_2, variable_set_2);

 	//Validate statement 3
 	EXPECT_TRUE(uses.validate_uses(3, "d"));
 	EXPECT_TRUE(uses.validate_uses(3, "e"));
 	
 	VarResults expected_variable_set_3;
 	expected_variable_set_3.insert("d");
 	expected_variable_set_3.insert("e");

	VarResults variable_set_3= uses.get_used_vars(3);
 	EXPECT_EQ(expected_variable_set_3, variable_set_3);

	//Validate statement 4
 	EXPECT_TRUE(uses.validate_uses(4, "g"));
 	
 	VarResults expected_variable_set_4;
 	expected_variable_set_4.insert("g");

	VarResults variable_set_4= uses.get_used_vars(4);
 	EXPECT_EQ(expected_variable_set_4, variable_set_4);

 	//Program wide
 	EXPECT_TRUE(uses.validate_uses("First", "a"));
 	EXPECT_TRUE(uses.validate_uses("First", "b"));
 	EXPECT_TRUE(uses.validate_uses("First", "d"));
 	EXPECT_TRUE(uses.validate_uses("First", "e"));
 	EXPECT_TRUE(uses.validate_uses("First", "g"));

 	VarResults expected_variable_set_proc;
 	expected_variable_set_proc.insert("a");
 	expected_variable_set_proc.insert("b");
 	expected_variable_set_proc.insert("d");
 	expected_variable_set_proc.insert("e");
 	expected_variable_set_proc.insert("g");

	VarResults variable_set_proc = uses.get_used_vars("First");
	EXPECT_EQ(expected_variable_set_proc, variable_set_proc);

	//Variable a
	StatementResults expected_statement_a;
	expected_statement_a.insert(1);

	StatementResults statement_set_a = uses.get_using_statements("a");
	EXPECT_EQ(expected_statement_a, statement_set_a);

	//Variable b
	StatementResults expected_statement_b;
	expected_statement_b.insert(1);
	expected_statement_b.insert(2);

	StatementResults statement_set_b = uses.get_using_statements("b");
	EXPECT_EQ(expected_statement_b, statement_set_b);

	//Variable c
	StatementResults expected_statement_c;
	StatementResults statement_set_c = uses.get_using_statements("c");
	EXPECT_EQ(expected_statement_c, statement_set_c);

	//Variable d
	StatementResults expected_statement_d;
	expected_statement_d.insert(1);
	expected_statement_d.insert(2);
	expected_statement_d.insert(3);

	StatementResults statement_set_d = uses.get_using_statements("d");
	EXPECT_EQ(expected_statement_d, statement_set_d);

	//Variable e
	StatementResults expected_statement_e;
	expected_statement_e.insert(1);
	expected_statement_e.insert(2);
	expected_statement_e.insert(3);

	StatementResults statement_set_e = uses.get_using_statements("e");
	EXPECT_EQ(expected_statement_e, statement_set_e);

	//Variable f
	StatementResults expected_statement_f;	
	StatementResults statement_set_f = uses.get_using_statements("f");
	EXPECT_EQ(expected_statement_f, statement_set_f);

	//Variable g
	StatementResults expected_statement_g;
	expected_statement_g.insert(1);
	expected_statement_g.insert(2);
	expected_statement_g.insert(4);

	StatementResults statement_set_g = uses.get_using_statements("g");
	EXPECT_EQ(expected_statement_g, statement_set_g);
}

TEST (UsesTest, MultipleVariables_IfNestedWhileInside) {
	/*
	 * Meant to build this SIMPLE program
	 * procedure First {
	 * 1.	if a then {
	 * 2.		while b {
	 * 3.			c = d - 1;}
	 *		else{
	 * 4.		while e {
	 * 5.			f = 1;}
	 *		}
	 */

	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varA("a");
	SimpleVariable varB("b");
	SimpleVariable varC("c");
	SimpleVariable varD("d");
	SimpleVariable varE("e");
	SimpleVariable varF("f");

	SimpleIfAst *if_stmt = new SimpleIfAst();
	if_stmt->set_line(1);
	if_stmt->set_variable(varA);
	set_proc(if_stmt, procedure);

	SimpleWhileAst *while_stmt_2 = new SimpleWhileAst();
	while_stmt_2->set_line(2);
	while_stmt_2->set_variable(varB);
	set_then_branch(while_stmt_2, if_stmt);

	SimpleAssignmentAst *stmt_3 = new SimpleAssignmentAst(3);

	SimpleBinaryOpAst *expression_3 = new SimpleBinaryOpAst('-', 
		new SimpleVariableAst("d"), new SimpleConstAst(1));
	stmt_3->set_variable(varC);
	stmt_3->set_expr(expression_3);
	
	set_while_body(stmt_3, while_stmt_2);

	SimpleWhileAst *while_stmt_4 = new SimpleWhileAst();
	while_stmt_4->set_line(4);
	while_stmt_4->set_variable(varE);
	set_else_branch(while_stmt_4, if_stmt);

	SimpleAssignmentAst *stmt_5 = new SimpleAssignmentAst(5);
	stmt_5->set_variable(varF);
	stmt_5->set_expr(new SimpleConstAst(1));
	set_while_body(stmt_5, while_stmt_4);

	SimpleRoot root(procedure);

	LineTable line_table;
	line_table[1] = if_stmt;
	line_table[2] = while_stmt_2;
	line_table[3] = stmt_3;
	line_table[4] = while_stmt_4;
	line_table[5] = stmt_5;

	StatementTable *statement_table = new StatementTable(line_table);
	AST *ast = new AST(root, statement_table);

	SolverPtr uses_solver(new SimpleSolverGenerator<UsesSolver>(
		new UsesSolver(ast->get_root())));
 	Uses uses(uses_solver, ast);

 	//Validate if
 	EXPECT_TRUE(uses.validate_uses(1, "a"));
 	EXPECT_TRUE(uses.validate_uses(1, "b"));
 	EXPECT_TRUE(uses.validate_uses(1, "d"));
 	EXPECT_TRUE(uses.validate_uses(1, "e"));
 	
 	VarResults expected_variable_set_1;
 	expected_variable_set_1.insert("a");
 	expected_variable_set_1.insert("b");
 	expected_variable_set_1.insert("d");
 	expected_variable_set_1.insert("e");
 	
 	VarResults variable_set_1 = uses.get_used_vars(1);
 	EXPECT_EQ(expected_variable_set_1, variable_set_1);

 	//Validate while at statement 2
	EXPECT_TRUE(uses.validate_uses(2, "b"));
 	EXPECT_TRUE(uses.validate_uses(2, "d"));

	VarResults expected_variable_set_2;
	expected_variable_set_2.insert("b");
 	expected_variable_set_2.insert("d");

 	VarResults variable_set_2= uses.get_used_vars(2);
 	EXPECT_EQ(expected_variable_set_2, variable_set_2);

 	//Validate statement 3
  	EXPECT_TRUE(uses.validate_uses(3, "d"));
 	
 	VarResults expected_variable_set_3;
 	expected_variable_set_3.insert("d");

	VarResults variable_set_3= uses.get_used_vars(3);
 	EXPECT_EQ(expected_variable_set_3, variable_set_3);

 	//Validate while at statement 4
  	EXPECT_TRUE(uses.validate_uses(4, "e"));
 	
 	VarResults expected_variable_set_4;
 	expected_variable_set_4.insert("e");

	VarResults variable_set_4= uses.get_used_vars(4);
 	EXPECT_EQ(expected_variable_set_4, variable_set_4);	

 	//Validate statement 5
 	VarResults expected_variable_set_5;

	VarResults variable_set_5= uses.get_used_vars(5);
 	EXPECT_EQ(expected_variable_set_5, variable_set_5);	

 	//Program wide
 	EXPECT_TRUE(uses.validate_uses("First", "a"));
 	EXPECT_TRUE(uses.validate_uses("First", "b"));
 	EXPECT_TRUE(uses.validate_uses("First", "d"));
 	EXPECT_TRUE(uses.validate_uses("First", "e"));
 	//TO BE COMPLETED
}
#if 0
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