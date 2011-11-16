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
#include "simple/query.h"
#include "impl/parser/tokenizer.h"
#include "impl/query.h"
#include "impl/qvar.h"

namespace simple {
namespace parser {

typedef std::map<std::string, ConditionSet>     ResultTable;

class PqlParserError : public std::exception { };

class SimplePqlParser {
  public:
    SimplePqlParser(SimpleTokenizer *tokenizer, SolverTable *table) :
        _tokenizer(tokenizer), _solver_table(table)
    { }

    PqlQuerySet parse_query() {
        while(!current_token_is<EOFToken>()) {
            std::string first_word = current_token_as_keyword();

            if(first_word == "SELECT") {
                parse_main_query();
                break;
            } else {
                parse_predicate();
            }
        }

        return _query_set;
    }

    void parse_predicate() {
        std::string first_word = current_token_as_keyword();

        SimplePredicate *pred;

        if(first_word == "PROCEDURE") {
            pred = get_or_create_predicate<SimpleProcPredicate>();
        } else if(first_word == "PROG_LINE" || first_word == "STMT" || 
                first_word == "STMTLST") {
            pred = get_or_create_predicate<SimpleStatementPredicate>();
        } else if(first_word == "ASSIGN") {
            pred = get_or_create_predicate<SimpleAssignmentPredicate>();
        } else if(first_word == "WHILE") {
            pred = get_or_create_predicate<SimpleWhilePredicate>();
        } else if(first_word == "IF") {
            pred = get_or_create_predicate<SimpleConditionalPredicate>();
        } else if(first_word == "VAR" || first_word == "VARIABLE") {
            pred = get_or_create_predicate<SimpleVariablePredicate>();
        } else if(first_word == "CALL") {
            pred = get_or_create_predicate<SimpleCallPredicate>();
        } else {
            throw PqlParserError();
        }

        next_token(); // eat predicate keyword
        parse_query_variables(pred);
    }

    void parse_query_variables(SimplePredicate *pred) {
        while(true) {
            std::string qvar_name = current_token_as<IdentifierToken>()->get_content();

            // the query variable should not be defined twice
            if(_query_set.qvars_table.count(qvar_name) > 0) {
                throw PqlParserError();
            }

            QueryVariable *qvar = new SimpleQueryVariable(var_name, pred);
            _query_set.qvars_table[var_name] = QVarPtr(qvar);

            next_token(); // eat var name

            if(current_token_is<SemiColonToken>()) {
                next_token(); // eat ';'
                return;
            } else {
                // if it's not ';', it must be a comma ','
                current_token_as<CommaToken>();
                next_token(); // eat ','
            }
        }
    }

    void parse_main_query() {
        std::string first_keyword = current_token_as_keyword();

        if(first_keyword != "SELECT") {
            throw PqlParserError();
        }

        next_token();
        while(true) {
            std::string qvar_name = current_token_as<IdentifierToken>()->get_content();
            std::shared_ptr<QueryVariable> qvar = qvars()->get_qvar(qvar_name);
            
            // the query variable must be defined first
            if(qvar == NULL) {
                throw PqlParserError();
            }

            selected_qvars()->push_back(qvar);

            next_token(); // eat var name
            if(current_token_is<CommaToken>()) {
                next_token();
                continue;
            } else {
                next_token();
                break;
            }
        }

        if(current_token_as_keyword() == "SUCH") {
            next_token(); // eat "such"
            if(current_token_as_keyword() != "THAT") {
                throw PqlParserError();
            } else {
                next_token(); // eat "that"
            }
        } else {
            throw PqlParserError();
        }

        while(!current_token_is<EOFToken>()) {

        }
    }

    PqlSelector* parse_selector() {
        if(current_token_is<LessThanToken>()) {
            return parse_tuple_selector();
        } else if(current_token_is<IdentifierToken>() 
                && current_token_as_keyword() === "BOOLEAN") 
        {
            next_token(); // eat 'BOOLEAN'
            return new SimplePqlBooleanSelector();
        } else {
            std::string selected_var = current_token_as<
                    IdentifierToken>()->get_content();

            next_token(); // eat var name
            return new SimplePqlSingleVariableSelector(selected_var);
        }
    }

    QueryMatcher* parse_criteria() {
        std::string solver_name = current_token_as_keyword();
        next_token();

        if(current_token_is<OperatorToken>() &&
                current_token_as<OperatorToken>()->get_op() == '*') 
        {
            solver_name = std::string("I") + solver_name;
            next_token();
        }

        current_token_as<OpenBracketToken>();
        next_token();

        
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
        std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::to_upper);
    }

    SimpleToken* next_token() {
        _current_token = _tokenizer->next_token();
        if(current_token_is<NewLineToken>()) {
            ++_line;
            return next_token();
        } else {
            return _current_token;
        }
    }

    SimpleToken* current_token() {
        return _current_token;
    }

    QVarTable* qvars() {
        return _query_set->qvars.get();
    }

    template <typename Predicate>
    SimplePredicate* get_or_create_predicate() {
        std::string pred_name = Predicate::get_name();

        if(_predicate_table.count(pred_name) == 0) {
            SimplePredicate *pred = new Predicate(_ast);
            _predicate_table[pred_name] = PredicatePtr(pred);
            return pred;
        } else {
            return _predicate_table[pred_name].get();
        }
    }


  private:
    std::unique_ptr<SimpleTokenizer> _tokenizer;

    SimpleRoot      _ast;
    MatcherTable    *_matcher_table;

    std::map<std::string, PredicatePtr>
    _predicate_table;

    PqlQuerySet     _query_set;

    SimpleToken     *_current_token;
};



}
}
