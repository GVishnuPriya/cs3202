#pragma once

#include "simple/ast.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "spa/ast.h"
#include "spa/statement.h"
#include "spa/uses.h"
#include <iostream>
#include <string>

namespace spa  {
namespace test{

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

AST* uses_fixture_1() {
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

    StatementTable *statement_table = new StatementTable(line_table);
	AST *ast = new AST(root, statement_table);

	return ast;
}

AST* uses_fixture_2() {
	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varX("x");
	SimpleVariable varY("y");

	//1st statement
	SimpleAssignmentAst *stmt_1 = new SimpleAssignmentAst();
	stmt_1->set_variable(varX);
	stmt_1->set_expr(new SimpleConstAst(1));
	stmt_1->set_line(1);
	set_proc(stmt_1, procedure);

	//2nd statement
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst();
	stmt_2->set_variable(varY);
	SimpleBinaryOpAst *expression = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("x"), new SimpleConstAst(1));
	stmt_2->set_expr(expression);
	stmt_2->set_line(2);
	set_next(stmt_1, stmt_2);

	SimpleRoot root(procedure);	//Set this procedure as the first procedure

	LineTable line_table;
    line_table[1] = stmt_1;
    line_table[2] = stmt_2;

    StatementTable *statement_table = new StatementTable(line_table);
	AST *ast = new AST(root, statement_table);

	return ast;
}

AST* uses_fixture_3() {
	SimpleProcAst *procedure = new SimpleProcAst("First");
	SimpleVariable varX("x");
	SimpleVariable varY("y");

	//1st statement
	SimpleAssignmentAst *stmt_1 = new SimpleAssignmentAst(1);
	stmt_1->set_variable(varX);
	stmt_1->set_expr(new SimpleConstAst(1));
	set_proc(stmt_1, procedure);

	//2nd statement
	SimpleAssignmentAst *stmt_2 = new SimpleAssignmentAst(2);
	stmt_2->set_variable(varY);
	SimpleBinaryOpAst *expression_2 = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("x"), new SimpleConstAst(1));
	stmt_2->set_expr(expression_2);
	set_next(stmt_1, stmt_2);

	//3rd statement
	SimpleAssignmentAst *stmt_3 = new SimpleAssignmentAst(3);
	stmt_3->set_variable(varX);
	SimpleBinaryOpAst *expression_3 = new SimpleBinaryOpAst('+', 
		new SimpleVariableAst("x"), new SimpleVariableAst("y"));
	stmt_3->set_expr(expression_3);
	set_next(stmt_2, stmt_3);

	SimpleRoot root(procedure);	//Set this procedure as the first procedure

	LineTable line_table;
    line_table[1] = stmt_1;
    line_table[2] = stmt_2;
    line_table[3] = stmt_3;

    StatementTable *statement_table = new StatementTable(line_table);
	AST *ast = new AST(root, statement_table);

	return ast;
}
}
}