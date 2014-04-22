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

#include <utility>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>
#include "impl/ast.h"
#include "impl/parser/pql_parser.h"
#include "simple/util/term_utils.h"

// to denote type of select query (eg. v.varname)
enum SelectType {
    StmtNum,
    VarName,
    ProcName,
    Value,
    Default
};

namespace simple {
namespace parser {

using namespace simple;
using namespace simple::impl;

SimplePqlParser::SimplePqlParser(
        std::shared_ptr<SimpleTokenizer> tokenizer, 
        const SimpleRoot& ast,
        const LineTable& line_table,
        const SolverTable& solver_table,
        const PredicateTable& pred_table) :
    _tokenizer(tokenizer), _ast(ast),
    _line_table(line_table), _solver_table(solver_table), 
    _pred_table(pred_table)
{ 
    next_token();
}

PqlQuerySet& SimplePqlParser::get_query_set() {
    return _query_set;
}

PqlQuerySet SimplePqlParser::parse_query() {
    while(true) {
        if(current_token_is<EOFToken>()) {
            throw IncompleteParseError("EOF reached prematurely");
        }

        std::string first_word = current_token_as_keyword();

        if(first_word == "select") {
            parse_main_query();
            break;
        } else {
            parse_predicate();
        }
    }

    return _query_set;
}

void SimplePqlParser::parse_predicate() {
    std::string keyword = current_token_as_keyword();
    next_token(); // eat keyword

    std::shared_ptr<SimplePredicate> pred;

    if(keyword == "procedure") {
        pred = get_predicate("procedure");
    } else if(keyword == "prog_line" || keyword == "stmt" || 
            keyword == "stmtlist" || keyword == "stmtlst" || keyword == "statement") 
    {
        pred = get_predicate("statement");
    } else if(keyword == "assign") {
        pred = get_predicate("assign");
    } else if(keyword == "while") {
        pred = get_predicate("while");
    } else if(keyword == "if") {
        pred = get_predicate("if");
    } else if(keyword == "var" || keyword == "variable") {
        pred = get_predicate("var");
    } else if(keyword == "call") {
        pred = get_predicate("call");
    } else if (keyword == "const" || keyword == "constant") {
        pred = get_predicate("const");
    } else if(keyword == "op" || keyword == "operator") {
        pred = get_predicate("operator");
    } else if (keyword == "minus" || keyword == "plus" || keyword == "times"){
        pred = get_predicate(keyword);
    } else {
        pred = get_predicate(keyword);
    }

    while(true) {
        std::string qvar = current_token_as<
                    IdentifierToken>()->get_content();

        _query_set.predicates[qvar] = pred;

        next_token(); // eat var name

        if(current_token_is<SemiColonToken>()) {
            next_token(); // eat ';'
            return;
        }

        current_token_as<CommaToken>();
        next_token(); // eat comma
    }
}

void SimplePqlParser::parse_main_query() {
    _query_set.selector = parse_selector();

    while(!(current_token_is<EOFToken>() || 
            current_token_is<SemiColonToken>())) 
    {
        std::string keyword = current_token_as_keyword();
        next_token(); // eat keyword
                
        if(keyword == "such") {
            if(current_token_as_keyword() != "that") {
                throw ParseError("Expected \"that\" keyword after \"such\".");
            }

            next_token(); // eat "that"
            
            while(true) {
                _query_set.clauses.insert(parse_clause());

                if(current_token_is<IdentifierToken>() &&
                    current_token_as_keyword() == "and")
                {
                    next_token(); // eat "and"
                } else {
                    break;
                }
            }
        } else if(keyword == "with") {
            while(true) {
                _query_set.clauses.insert(parse_with());

                if(current_token_is<IdentifierToken>() &&
                    current_token_as_keyword() == "and")
                {
                    next_token(); // eat "and"
                } else {
                    break;
                }
            }
        } else if(keyword == "pattern") {
            while(true) {
                parse_pattern();

                if(current_token_is<IdentifierToken>() &&
                    current_token_as_keyword() == "and")
                {
                    next_token(); // eat "and"
                } else {
                    break;
                }
            }
        } else {
            throw ParseError("Invalid keyword " + keyword);
        }
    }
}

std::shared_ptr<PqlSelector> SimplePqlParser::parse_selector() {
    std::string first_keyword = current_token_as_keyword();
    next_token();

    if(first_keyword != "select") {
        throw ParseError("Expected\"select\" keyword");
    }

    if(current_token_is<LessThanToken>()) {
        return parse_tuple_selector();
    } else if(current_token_is<IdentifierToken>() 
            && current_token_as_keyword() == "boolean") 
    {
        next_token(); // eat 'BOOLEAN'
        return std::shared_ptr<PqlSelector>(
                new SimplePqlBooleanSelector());
    } else {
        std::string selected_var = current_token_as<
                IdentifierToken>()->get_content();

        next_token(); // eat var name
        
        SimplePqlSingleVarSelector *selector =  new SimplePqlSingleVarSelector(selected_var);
        
        if(!current_token_is<DotToken>()) {
            selector->set_select_type(Default);
            return std::shared_ptr<PqlSelector>(selector);
        }

        next_token(); // eat "."

        std::string field1 = current_token_as_keyword();

        if(field1 == "stmt") {
            selector->set_select_type(StmtNum);
            next_token_as<HashToken>();
            next_token(); // eat #
        } else if(field1 == "varname") {
            selector->set_select_type(VarName);
            next_token(); // eat keyword
        } else if(field1 == "procname") {
            selector->set_select_type(ProcName);
            next_token(); // eat keyword
        } else if(field1 == "value") {
            selector->set_select_type(Value);
            next_token(); // eat keyword
        } else {
            throw new ParseError("Invalid field name after dot");
        }

        return std::shared_ptr<PqlSelector>(selector);
    }
}

ClausePtr SimplePqlParser::parse_clause() {
    std::string solver_name = current_token_as_keyword();
    next_token();

    if(current_token_is<OperatorToken>() &&
            current_token_as<OperatorToken>()->get_op() == '*') 
    {
        solver_name = std::string("i") + solver_name;
        next_token();
    }

    std::shared_ptr<QuerySolver> solver = _solver_table[solver_name];

    if(!solver) {
        throw ParseError("No solver found for clause " + solver_name);
    }

    current_token_as<OpenBracketToken>();
    next_token();

    PqlTerm *first_term = parse_term();
    
    current_token_as<CommaToken>();
    next_token();

    PqlTerm *second_term = parse_term();

    current_token_as<CloseBracketToken>();
    next_token();

    return ClausePtr(new SimplePqlClause(solver, first_term, second_term));
}

PqlTerm* SimplePqlParser::parse_term() {
    if(current_token_is<LiteralToken>()) {
        ConditionPtr condition = parse_condition(string_trim(
                current_token_as<LiteralToken>()->get_content()));
        next_token();
        return new SimplePqlConditionTerm(condition);

    } else if(current_token_is<IntegerToken>()) {
        int line = current_token_as<IntegerToken>()->get_value();
        next_token();

        if(_line_table.count(line) > 0) {
            return new SimplePqlConditionTerm(
               new SimpleStatementCondition(get_statement(line)));
        } else {
            return new SimplePqlConditionTerm(
                new SimpleConstantCondition(line));
        }
    } else if(current_token_is<IdentifierToken>()) {
        std::string var_name = current_token_as<
                IdentifierToken>()->get_content();
        next_token();
        return new SimplePqlVariableTerm(var_name);

    } else if(current_token_is<WildCardToken>()) {
        next_token();
        return new SimplePqlWildcardTerm();

    } else {
        throw ParseError("Invalid clause term");
    }
}

ConditionPtr SimplePqlParser::parse_condition(const std::string& name) {
    ProcAst *proc = _ast.get_proc(name);
    if(proc) {
        return new SimpleProcCondition(proc);
    } else {
        return new SimpleVariableCondition(SimpleVariable(name));
    }
}

PqlTerm* SimplePqlParser::parse_expr_term() {
    if(current_token_is<LiteralToken>()) {
        std::string expr_string = string_trim(current_token_as<LiteralToken>()->get_content());
        next_token();

        std::shared_ptr<SimpleTokenizer> tokenizer(
            new IteratorTokenizer<std::string::iterator>(
                expr_string.begin(), expr_string.end()));

        ExprParser parser(tokenizer);

        ExprAst *expr = parser.parse_expr();

        return new SimplePqlConditionTerm(
            new SimplePatternCondition(expr));

    } else if(current_token_is<IdentifierToken>()) {
        std::string var_name = current_token_as<
                IdentifierToken>()->get_content();
        next_token();
        return new SimplePqlVariableTerm(var_name);

    } else {
        throw ParseError("Invalid pattern term");
    }
}

std::pair<PqlTerm*, bool> SimplePqlParser::parse_pattern_term() {
    if(!current_token_is<WildCardToken>()) {
        PqlTerm* term = parse_expr_term();
        return std::make_pair(term, false);
    }

    next_token();

    if(current_token_is<CloseBracketToken>()) {
        return std::make_pair(new SimplePqlWildcardTerm(), false);
    }
    
    if (current_token_is<CommaToken>()) {
         next_token();
    
        current_token_as<WildCardToken>();
        next_token(); // eat "_"
    
        return std::make_pair(new SimplePqlWildcardTerm(), false);
    }
    
    PqlTerm* term = parse_expr_term();
    
    current_token_as<WildCardToken>();
    next_token(); // eat "_"

    return std::make_pair(term, true);
   
}

void SimplePqlParser::parse_pattern() {
    PqlTerm* term1 = parse_term();

    current_token_as<OpenBracketToken>();
    next_token();

    PqlTerm* term2 = parse_term();

    current_token_as<CommaToken>();
    next_token();

    std::pair<PqlTerm*, bool> pattern_term = parse_pattern_term();

    PqlTerm* term3 = pattern_term.first;
    bool is_indirect = pattern_term.second;

    current_token_as<CloseBracketToken>();
    next_token();
    
    PqlTermType type2 = get_term_type(term2);
    PqlTermType type3 = get_term_type(term3);

    SolverPtr uses_solver = _solver_table["direct_uses"];

    SolverPtr expr_solver = is_indirect ? 
        _solver_table["iexpr"] : _solver_table["expr"];

    ClausePtr expr_clause(new SimplePqlClause(expr_solver, term1, term3));
    ClausePtr uses_clause(new SimplePqlClause(uses_solver, clone_term(term1), term2));

    if(type2 != WildcardTT && type3 == WildcardTT) {
        _query_set.clauses.insert(uses_clause);

    } else if(type2 == WildcardTT && type3 != WildcardTT) {
        _query_set.clauses.insert(expr_clause);

    } else if(type2 == WildcardTT && type3 == WildcardTT) {
        // if both term2 and term3 are wildcard we use direct uses to validate term1
        _query_set.clauses.insert(uses_clause);

    } else {
        _query_set.clauses.insert(uses_clause);
        _query_set.clauses.insert(expr_clause);
    }
}

void SimplePqlParser::eat_field() {
    if(!current_token_is<DotToken>()) return;

    next_token(); // eat "."

    std::string field1 = current_token_as_keyword();

    if(field1 == "stmt") {
        next_token_as<HashToken>();
        next_token(); // eat #
    } else if(field1 == "varname" || field1 == "procname" || field1 == "value") {
        next_token(); // eat keyword
    } else {
        throw new ParseError("Invalid field name after dot");
    }
}

PqlTerm* SimplePqlParser::parse_with_term() {
    if(current_token_is<LiteralToken>()) {
        ConditionPtr condition = parse_condition(string_trim(
                current_token_as<LiteralToken>()->get_content()));
        next_token();
        return new SimplePqlConditionTerm(condition);
    } 

    if(current_token_is<IntegerToken>()) {
        int line = current_token_as<IntegerToken>()->get_value();
        next_token();
        try {
            SimpleStatementCondition *condition = new SimpleStatementCondition(get_statement(line));
            return new SimplePqlConditionTerm(condition);
        } catch(ParseError& e) {
            return new SimplePqlConditionTerm(new SimpleConstantCondition(line));
        }
        
    }

    std::string qvar = current_token_as<IdentifierToken>()->get_content();
    PqlTerm *term = new SimplePqlVariableTerm(qvar);

    next_token(); // eat qvar
    
    if(!current_token_is<DotToken>()) return term;

    next_token(); // eat "."

    std::string field1 = current_token_as_keyword();
    std::shared_ptr<SimplePredicate> pred = _query_set.predicates[qvar];
    
    if(field1 == "stmt") {
        next_token_as<HashToken>();
        next_token(); // eat #
        if (pred == get_predicate("statement") 
            || pred == get_predicate("assign") 
            || pred == get_predicate("if")
            || pred == get_predicate("while") 
            || pred == get_predicate("call")) {
            return term;
        }
    } else if(field1 == "varname") {
        next_token(); // eat keyword
        if (pred == get_predicate("var")) return term;
    } else if(field1 == "procname") {
        next_token(); // eat keyword
        if (pred == get_predicate("procedure") || pred == get_predicate("call")) 
            return term;
    } else if(field1 == "value") {
        next_token(); // eat keyword
        if (pred == get_predicate("const")) return term;
    } 
    
    throw new ParseError("Invalid field name after dot");

}

ClausePtr SimplePqlParser::parse_with() {
    PqlTerm *term1 = parse_with_term();

    current_token_as<EqualToken>();
    next_token(); // eat =

    PqlTerm *term2 = parse_with_term();

    std::shared_ptr<QuerySolver> solver = _solver_table["equal"];

    return ClausePtr(new SimplePqlClause(solver, term1, term2));
}

std::shared_ptr<PqlSelector> SimplePqlParser::parse_tuple_selector() {
    next_token(); // eat '<'
    std::vector< std::string > tuples;

    std::string first = current_token_as<IdentifierToken>()->get_content();
    tuples.push_back(first);
    next_token();
    eat_field();

    while(current_token_is<CommaToken>()) {
        next_token(); // eat ','
        std::string qvar = current_token_as<IdentifierToken>()->get_content();
        tuples.push_back(qvar);
        next_token();

        eat_field();
    }

    current_token_as<MoreThanToken>();
    next_token();

    std::shared_ptr<PqlSelector> selector(new SimplePqlTupleSelector(tuples));
    return selector;
}

std::string SimplePqlParser::current_token_as_keyword() {
    std::string keyword = current_token_as<IdentifierToken>()->get_content();
    std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    return keyword;
}

SimpleToken* SimplePqlParser::next_token() {
    _current_token = _tokenizer->next_token();
    if(current_token_is<NewLineToken>()) {
        return next_token();
    } else {
        return _current_token;
    }
}

SimpleToken* SimplePqlParser::current_token() {
    return _current_token;
}

PredicatePtr SimplePqlParser::get_predicate(const std::string& name) {
    if(_pred_table.count(name) == 0) {
        throw ParseError("Invalid predicate " + name);
    }
    return _pred_table[name];
}

StatementAst* SimplePqlParser::get_statement(int line) {
    if(_line_table.count(line) == 0) {
        throw ParseError("No statement found at line " + line);
    }
    return _line_table[line];
}

std::string SimplePqlParser::string_trim(std::string str) {
    std::string s = str;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}
    
    
}
}
