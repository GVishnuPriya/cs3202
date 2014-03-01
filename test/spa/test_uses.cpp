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
#include "simple/util/solver_generator.h"

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
    set_proc(stmt_1, procedure);	//Set the assignment statement to the procedure

	//2nd statement
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst();
	SimpleBinaryOpAst *expression = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("x"), new SimpleConstAst(1));
	stmt_2->set_variable(var);
	stmt_2->set_expr(expression);
	stmt_2->set_line(2);
	set_next(stmt_1, stmt_2);

    SimpleRoot root(procedure);	//Set this procedure as the first procedure

    LineTable line_table;
    line_table[1] = stmt_1;
    line_table[2] = stmt_2;

    StatementTable statement_table(line_table);
	SolverPtr uses_solver(new SimpleSolverGenerator<UsesSolver>(new UsesSolver(root))); 
	AST ast(root, &statement_table);

	Uses uses(uses_solver, &ast);

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

}
}