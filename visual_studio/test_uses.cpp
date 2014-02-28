#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/ast.h"
#include "spa/statement.h"
#include "spa/uses.h"
#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "simple/util/ast_utils.h"
#include "impl/solvers/uses.h"
#include "impl/ast.h"
#include "impl/condition.h"

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

    SimpleProcAst *procedure = new SimpleProcAst("First");	//Create a new ProcAst for procedure "First"
    SimpleVariable var("x");	//Create the variable node for "x"

	//1st statement
	SimpleAssignmentAst *stmt_1 = new SimpleAssignmentAst();	//Create a new AssignmentAst for the statement inside
    stmt_1->set_variable(var);
    stmt_1->set_expr(new SimpleConstAst(1));
    stmt_1->set_line(1);	//Set line number to for the assignment statement
    stmt_1->set_proc(procedure);	//Set the assignment statement to the procedure

    procedure->set_first_statement(stmt_1);

	//2nd statement
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst();
	SimpleBinaryOpAst *expression = new SimpleBinaryOpAst();
	stmt_2->set_expr(expression);
	stmt_2->set_line(2);
	stmt_2->set_proc(procedure);

    SimpleRoot root(procedure);	//Set this procedure as the first procedure
	UsesSolver solver(root);
	AST *ast = new AST(root, NULL);

	Uses *testUses = new Uses(solver, ast);
	
	EXPECT_TRUE(testUses->validate_uses(procedure, var));
	EXPECT_TRUE(testUses->validate_uses(1,var));

	std::set<int> result_set = testUses.get_using_statements;
	std::set<int>::iterator iter = result_set.begin();
	EXPECT_EQ("1",*iter) ;


}

}
}