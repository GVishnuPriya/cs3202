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
#include "simple/solver.h"
#include "simple/util/query_utils.h"
#include "impl/ast.h"
#include "impl/parser/pql_parser.h"
#include "test/mock.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(PqlParserTest, ComponentTest) {
    /*
     * proc test {
     *   x = 1;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");

    assign->set_variable(var);
    assign->set_expr(new SimpleConstAst(1));
    assign->set_line(1);
    assign->set_proc(proc);

    proc->set_first_statement(assign);

    SimpleRoot ast(proc);

    LineTable line_table;
    line_table[1] = assign;

    std::shared_ptr<QuerySolver> modifies_solver(new MockSolver());
    std::shared_ptr<QuerySolver> imodifies_solver(new MockSolver());
    std::shared_ptr<QuerySolver> follows_solver(new MockSolver());
    std::shared_ptr<QuerySolver> ifollows_solver(new MockSolver());

    SolverTable solver_table;
    solver_table["modifies"] = modifies_solver;
    solver_table["imodifies"] = imodifies_solver;
    solver_table["follows"] = follows_solver;
    solver_table["ifollows"] = ifollows_solver;

    std::shared_ptr<SimplePredicate> statement_pred(new MockPredicate());
    std::shared_ptr<SimplePredicate> variable_pred(new MockPredicate());

    PredicateTable pred_table;
    pred_table["statement"] = statement_pred;
    pred_table["variable"] = variable_pred;

    std::shared_ptr<MockTokenizer> tokenizer1(new MockTokenizer());

    tokenizer1->insert(new IdentifierToken("stmt"));
    tokenizer1->insert(new IdentifierToken("x"));
    tokenizer1->insert(new SemiColonToken());
    tokenizer1->insert(new EOFToken());

    SimplePqlParser parser1(tokenizer1, ast, line_table, solver_table, pred_table);
    
    parser1.parse_predicate();

    EXPECT_EQ(parser1.get_query_set().predicates["x"], statement_pred);


    std::shared_ptr<MockTokenizer> tokenizer2(new MockTokenizer());

    tokenizer2->insert(new IdentifierToken("variable"));
    tokenizer2->insert(new IdentifierToken("x"));
    tokenizer2->insert(new CommaToken());
    tokenizer2->insert(new IdentifierToken("y"));
    tokenizer2->insert(new SemiColonToken());
    tokenizer2->insert(new EOFToken());

    SimplePqlParser parser2(tokenizer2, ast, line_table, solver_table, pred_table);

    parser2.parse_predicate();
    EXPECT_EQ(parser2.get_query_set().predicates["x"], variable_pred);
    EXPECT_EQ(parser2.get_query_set().predicates["y"], variable_pred);
    
    std::shared_ptr<MockTokenizer> tokenizer3(new MockTokenizer());

    /*
     * Modifies(s, "x")
     */
    tokenizer3->insert(new IdentifierToken("modifies"));
    tokenizer3->insert(new OpenBracketToken());
    tokenizer3->insert(new IdentifierToken("s"));
    tokenizer3->insert(new CommaToken());
    tokenizer3->insert(new LiteralToken("x"));
    tokenizer3->insert(new CloseBracketToken());
    tokenizer3->insert(new EOFToken());

    SimplePqlParser parser3(tokenizer3, ast, line_table, solver_table, pred_table);

    std::shared_ptr<PqlClause> clause1 = parser3.parse_clause();

    SimplePqlClause expected_clause1(modifies_solver, 
            new SimplePqlVariableTerm("s"), 
            new SimplePqlConditionTerm(new SimpleVariableCondition(
                    SimpleVariable("x"))));

    EXPECT_TRUE(is_same_clause(clause1.get(), &expected_clause1));

    std::shared_ptr<MockTokenizer> tokenizer4(new MockTokenizer());

    /*
     * Follows(1, s)
     */
    tokenizer4->insert(new IdentifierToken("Follows"));
    tokenizer4->insert(new OpenBracketToken());
    tokenizer4->insert(new IntegerToken(1));
    tokenizer4->insert(new CommaToken());
    tokenizer4->insert(new IdentifierToken("s"));
    tokenizer4->insert(new CloseBracketToken());
    tokenizer4->insert(new EOFToken());

    SimplePqlParser parser4(tokenizer4, ast, line_table, solver_table, pred_table);

    std::shared_ptr<PqlClause> clause2 = parser4.parse_clause();
    SimplePqlClause expected_clause2(follows_solver,
            new SimplePqlConditionTerm(
                new SimpleStatementCondition(assign)),
            new SimplePqlVariableTerm("s"));

    EXPECT_TRUE(is_same_clause(clause2.get(), &expected_clause2));

    std::shared_ptr<MockTokenizer> tokenizer5(new MockTokenizer());

    /*
     * Modifies*(_, "test")
     */
    tokenizer5->insert(new IdentifierToken("MODIFIES"));
    tokenizer5->insert(new OperatorToken('*'));
    tokenizer5->insert(new OpenBracketToken());
    tokenizer5->insert(new WildCardToken());
    tokenizer5->insert(new CommaToken());
    tokenizer5->insert(new LiteralToken("test"));
    tokenizer5->insert(new CloseBracketToken());
    tokenizer5->insert(new EOFToken());

    SimplePqlParser parser5(tokenizer5, ast, line_table, solver_table, pred_table);

    std::shared_ptr<PqlClause> clause3 = parser5.parse_clause();
    SimplePqlClause expected_clause3(imodifies_solver,
            new SimplePqlWildcardTerm(),
            new SimplePqlConditionTerm(
                new SimpleProcCondition(proc)));

    EXPECT_TRUE(is_same_clause(clause3.get(), &expected_clause3));

    /*
     * Follows*("y", _)
     */
    std::shared_ptr<MockTokenizer> tokenizer6(new MockTokenizer());

    tokenizer6->insert(new IdentifierToken("fOlLoWs"));
    tokenizer6->insert(new OperatorToken('*'));
    tokenizer6->insert(new OpenBracketToken());
    tokenizer6->insert(new LiteralToken("y"));
    tokenizer6->insert(new CommaToken());
    tokenizer6->insert(new WildCardToken());
    tokenizer6->insert(new CloseBracketToken());
    tokenizer6->insert(new EOFToken());

    SimplePqlParser parser6(tokenizer6, ast, line_table, solver_table, pred_table);

    std::shared_ptr<PqlClause> clause4 = parser6.parse_clause();
    SimplePqlClause expected_clause4(ifollows_solver, 
            new SimplePqlConditionTerm(new SimpleVariableCondition(
                    SimpleVariable("y"))),
            new SimplePqlWildcardTerm());
    EXPECT_TRUE(is_same_clause(clause4.get(), &expected_clause4));

    /*
     * Select my_qvar
     */
    std::shared_ptr<MockTokenizer> tokenizer7(new MockTokenizer());

    tokenizer7->insert(new IdentifierToken("Select"));
    tokenizer7->insert(new IdentifierToken("my_qvar"));
    tokenizer7->insert(new EOFToken());
    
    SimplePqlParser parser7(tokenizer7, ast, line_table, solver_table, pred_table);

    std::shared_ptr<PqlSelector> selector1 = parser7.parse_selector();
    EXPECT_TRUE(is_selector<PqlSingleVarSelector>(selector1.get()));

    std::string selector1_qvar = selector_cast<PqlSingleVarSelector>(
                selector1.get())->get_qvar_name();

    EXPECT_EQ(selector1_qvar, "my_qvar");

    /*
     * Select BOOLEAN
     */
    std::shared_ptr<MockTokenizer> tokenizer8(new MockTokenizer());

    tokenizer8->insert(new IdentifierToken("Select"));
    tokenizer8->insert(new IdentifierToken("BOOLEAN"));
    tokenizer8->insert(new EOFToken());
    
    SimplePqlParser parser8(tokenizer8, ast, line_table, solver_table, pred_table);

    std::shared_ptr<PqlSelector> selector2 = parser8.parse_selector();
    
    EXPECT_TRUE(is_selector<PqlBooleanSelector>(selector2.get()));
}

TEST(PqlParserTest, FullQueryTest) {
    /*
     * proc test {
     *   x = 1;
     * }
     */
    SimpleProcAst *proc = new SimpleProcAst("test");
    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");

    assign->set_variable(var);
    assign->set_expr(new SimpleConstAst(1));
    assign->set_line(1);
    assign->set_proc(proc);

    proc->set_first_statement(assign);

    SimpleRoot ast(proc);

    LineTable line_table;
    line_table[1] = assign;

    std::shared_ptr<QuerySolver> modifies_solver(new MockSolver());
    std::shared_ptr<QuerySolver> imodifies_solver(new MockSolver());
    std::shared_ptr<QuerySolver> follows_solver(new MockSolver());
    std::shared_ptr<QuerySolver> ifollows_solver(new MockSolver());

    SolverTable solver_table;
    solver_table["modifies"] = modifies_solver;
    solver_table["imodifies"] = imodifies_solver;
    solver_table["follows"] = follows_solver;
    solver_table["ifollows"] = ifollows_solver;

    std::shared_ptr<SimplePredicate> statement_pred(new MockPredicate());
    std::shared_ptr<SimplePredicate> variable_pred(new MockPredicate());

    PredicateTable pred_table;
    pred_table["statement"] = statement_pred;
    pred_table["variable"] = variable_pred;

    /*
     * stmt s;
     * Select s such that Follows(1, s);
     */
    std::shared_ptr<MockTokenizer> tokenizer1(new MockTokenizer());

    tokenizer1->insert(new IdentifierToken("stmt"));
    tokenizer1->insert(new IdentifierToken("s"));
    tokenizer1->insert(new SemiColonToken());
    tokenizer1->insert(new IdentifierToken("Select"));
    tokenizer1->insert(new IdentifierToken("s"));
    tokenizer1->insert(new IdentifierToken("such"));
    tokenizer1->insert(new IdentifierToken("that"));
    tokenizer1->insert(new IdentifierToken("Follows"));
    tokenizer1->insert(new OpenBracketToken());
    tokenizer1->insert(new IntegerToken(1));
    tokenizer1->insert(new CommaToken());
    tokenizer1->insert(new IdentifierToken("s"));
    tokenizer1->insert(new CloseBracketToken());
    tokenizer1->insert(new SemiColonToken());
    tokenizer1->insert(new EOFToken());

    SimplePqlParser parser1(tokenizer1, ast, line_table, solver_table, pred_table);

    PqlQuerySet result1 = parser1.parse_query();

    EXPECT_TRUE(is_selector<PqlSingleVarSelector>(result1.selector.get()));
    EXPECT_EQ(selector_cast<PqlSingleVarSelector>(
                result1.selector.get())->get_qvar_name(), "s");

    EXPECT_EQ(result1.predicates["s"], statement_pred);

    EXPECT_EQ(result1.clauses.size(), (size_t) 1);
    SimplePqlClause expected_clause1(follows_solver,
            new SimplePqlConditionTerm(
                new SimpleStatementCondition(assign)),
            new SimplePqlVariableTerm("s"));

    EXPECT_TRUE(is_same_clause(result1.clauses[0].get(), &expected_clause1));

    /*
     * prog_line s, p; var v;
     * Select BOOLEAN such that Follows(s, _) and Modifies(s, v);
     */
    std::shared_ptr<MockTokenizer> tokenizer2(new MockTokenizer());

    tokenizer2->insert(new IdentifierToken("prog_line"));
    tokenizer2->insert(new IdentifierToken("s"));
    tokenizer2->insert(new CommaToken());
    tokenizer2->insert(new IdentifierToken("p"));
    tokenizer2->insert(new SemiColonToken());
    tokenizer2->insert(new IdentifierToken("var"));
    tokenizer2->insert(new IdentifierToken("v"));
    tokenizer2->insert(new SemiColonToken());
    tokenizer2->insert(new IdentifierToken("Select"));
    tokenizer2->insert(new IdentifierToken("boolean"));
    tokenizer2->insert(new IdentifierToken("such"));
    tokenizer2->insert(new IdentifierToken("that"));
    tokenizer2->insert(new IdentifierToken("Follows"));
    tokenizer2->insert(new OpenBracketToken());
    tokenizer2->insert(new IdentifierToken("s"));
    tokenizer2->insert(new CommaToken());
    tokenizer2->insert(new WildCardToken());
    tokenizer2->insert(new CloseBracketToken());
    tokenizer2->insert(new IdentifierToken("and"));
    tokenizer2->insert(new IdentifierToken("Modifies"));
    tokenizer2->insert(new OpenBracketToken());
    tokenizer2->insert(new IdentifierToken("s"));
    tokenizer2->insert(new CommaToken());
    tokenizer2->insert(new IdentifierToken("v"));
    tokenizer2->insert(new CloseBracketToken());
    tokenizer2->insert(new SemiColonToken());
    tokenizer2->insert(new EOFToken());

    SimplePqlParser parser2(tokenizer2, ast, line_table, solver_table, pred_table);

    PqlQuerySet result2 = parser2.parse_query();

    EXPECT_EQ(result2.predicates["s"], statement_pred);
    EXPECT_EQ(result2.predicates["p"], statement_pred);
    EXPECT_EQ(result2.predicates["v"], variable_pred);

    EXPECT_TRUE(is_selector<PqlBooleanSelector>(result2.selector.get()));

    EXPECT_EQ(result2.clauses.size(), (size_t) 2);
    SimplePqlClause expected_clause21(follows_solver,
            new SimplePqlVariableTerm("s"),
            new SimplePqlWildcardTerm());

    EXPECT_TRUE(is_same_clause(result2.clauses[0].get(), &expected_clause21));

    SimplePqlClause expected_clause22(modifies_solver,
            new SimplePqlVariableTerm("s"),
            new SimplePqlVariableTerm("v"));

    EXPECT_TRUE(is_same_clause(result2.clauses[1].get(), &expected_clause22));
}


} // namespace test
} // namespace simple
