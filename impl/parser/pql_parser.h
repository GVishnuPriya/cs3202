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

#pragma once

#include <algorithm>
#include <string>
#include <exception>
#include "simple/predicate.h"
#include "simple/query.h"
#include "simple/solver.h"
#include "impl/condition.h"
#include "impl/parser/tokenizer.h"
#include "impl/query.h"

namespace simple {
namespace parser {

using namespace simple;
using namespace simple::impl;

class PqlParserError : public std::exception { };

class SimplePqlParser {
  public:
    SimplePqlParser(
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

    PqlQuerySet& get_query_set() {
        return _query_set;
    }

    PqlQuerySet parse_query() {
        while(!current_token_is<EOFToken>()) {
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

    void parse_predicate() {
        std::string keyword = current_token_as_keyword();
        next_token(); // eat keyword

        std::shared_ptr<SimplePredicate> pred;

        if(keyword == "procedure") {
            pred = get_predicate("procedure");
        } else if(keyword == "prog_line" || keyword == "stmt" || 
                keyword == "stmtlist" || keyword == "statement") 
        {
            pred = get_predicate("statement");
        } else if(keyword == "assign") {
            pred = get_predicate("assignment");
        } else if(keyword == "while") {
            pred = get_predicate("while");
        } else if(keyword == "if") {
            pred = get_predicate("conditional");
        } else if(keyword == "var" || keyword == "variable") {
            pred = get_predicate("variable");
        } else if(keyword == "call") {
            pred = get_predicate("call");
        } else {
            throw PqlParserError();
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

    void parse_main_query() {
        _query_set.selector = parse_selector();

        while(!(current_token_is<EOFToken>() || 
                    current_token_is<SemiColonToken>())) 
        {
            std::string keyword = current_token_as_keyword();
            next_token(); // eat keyword

            if(keyword == "such") {
                if(current_token_as_keyword() != "that") {
                    throw PqlParserError();
                } else {
                    next_token(); // eat "that"

                    _query_set.clauses.push_back(parse_clause());
                }
            } else if(keyword == "and") {
                    _query_set.clauses.push_back(parse_clause());
            } else if(keyword == "with") {
                parse_with();
            } else if(keyword == "pattern") {
                parse_pattern();
            } else {
                throw PqlParserError();
            }
        }
    }

    std::shared_ptr<PqlSelector> parse_selector() {
        std::string first_keyword = current_token_as_keyword();
        next_token();

        if(first_keyword != "select") {
            throw PqlParserError();
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
            return std::shared_ptr<PqlSelector>(
                new SimplePqlSingleVarSelector(selected_var));
        }
    }

    std::shared_ptr<PqlClause> parse_clause() {
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
            throw PqlParserError();
        }

        current_token_as<OpenBracketToken>();
        next_token();

        PqlTerm *first_term = parse_term();
        
        current_token_as<CommaToken>();
        next_token();

        PqlTerm *second_term = parse_term();

        current_token_as<CloseBracketToken>();
        next_token();

        return std::shared_ptr<PqlClause>(
                new SimplePqlClause(solver, first_term, second_term));
    }

    PqlTerm* parse_term() {
        if(current_token_is<LiteralToken>()) {
            ConditionPtr condition = parse_condition(
                    current_token_as<LiteralToken>()->get_content());
            next_token();
            return new SimplePqlConditionTerm(condition);

        } else if(current_token_is<IntegerToken>()) {
            int line = current_token_as<IntegerToken>()->get_value();
            next_token();
            return new SimplePqlConditionTerm(
                   new SimpleStatementCondition(get_statement(line)));
            
        } else if(current_token_is<IdentifierToken>()) {
            std::string var_name = current_token_as<
                    IdentifierToken>()->get_content();
            next_token();
            return new SimplePqlVariableTerm(var_name);

        } else if(current_token_is<WildCardToken>()) {
            next_token();
            return new SimplePqlWildcardTerm();

        } else {
            throw PqlParserError();
        }
    }

    ConditionPtr parse_condition(const std::string& name) {
        ProcAst *proc = _ast.get_proc(name);
        if(proc) {
            return new SimpleProcCondition(proc);
        } else {
            return new SimpleVariableCondition(SimpleVariable(name));
        }
    }

    void parse_pattern() {
        throw PqlParserError(); // not implemented
    }

    void parse_with() {
        throw PqlParserError(); // not implemented
    }
    
    std::shared_ptr<PqlSelector> parse_tuple_selector() {
        throw PqlParserError(); // not implemented
    }

    template <typename Token>
    Token* current_token_as() {
        return token_cast<Token>(current_token());
    }

  protected:
    template <typename Token>
    Token* next_token_as() {
        return token_cast<Token>(next_token());
    }

    template <typename Token>
    bool current_token_is() {
        if(try_token<Token>(current_token()) != NULL) {
            return true;
        } else {
            return false;
        }
    }

    std::string current_token_as_keyword() {
        std::string keyword = current_token_as<IdentifierToken>()->get_content();
        std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
        return keyword;
    }

    SimpleToken* next_token() {
        _current_token = _tokenizer->next_token();
        if(current_token_is<NewLineToken>()) {
            return next_token();
        } else {
            return _current_token;
        }
    }

    SimpleToken* current_token() {
        return _current_token;
    }

    PredicatePtr get_predicate(const std::string& name) {
        if(_pred_table.count(name) == 0) {
            throw PqlParserError();
        }
        return _pred_table[name];
    }

    StatementAst* get_statement(int line) {
        if(_line_table.count(line) == 0) {
            throw PqlParserError();
        }
        return _line_table[line];
    }

  private:
    std::shared_ptr<SimpleTokenizer> _tokenizer;

    SimpleRoot      _ast;
    LineTable       _line_table;
    SolverTable     _solver_table;
    PredicateTable  _pred_table;
    PqlQuerySet     _query_set;

    SimpleToken     *_current_token;
};



}
}
