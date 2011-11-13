
#pragma once

#include <algorithm>
#include <string>
#include <exception>
#include "simple/query.h"
#include "impl/parser/tokenizer.h"

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

    void parse_predicate() {
        std::string first_word = current_token_as_keyword();

        SimplePredicate *pred;

        if(first_word == "PROCEDURE") {
            pred = _pred_table->get_predicate("procedure");
        } else if(first_word == "PROG_LINE" || first_word == "STMT" || 
                first_word == "STMTLST") {
            pred = _pred_table->get_predicate("statement");
        } else if(first_word == "ASSIGN") {
            pred = _pred_table->get_predicate("assignment");
        } else if(first_word == "WHILE") {
            pred = _pred_table->get_predicate("while");
        } else if(first_word == "IF") {
            pred = _pred_table->get_predicate("conditional");
        } else if(first_word == "VAR" || first_word == "VARIABLE") {
            pred = _pred_table->get_predicate("variable");
        } else if(first_word == "CALL") {
            pred = _pred_table->get_predicate("call");
        } else {
            throw PqlParserError();
        }

        next_token(); // eat predicate keyword
        parse_query_variables(pred);
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

    void parse_query_variables(SimplePredicate *pred) {
        while(true) {
            std::string var_name = current_token_as_keyword();

            // the query variable should not be defined twice
            if(qvars()->get_qvar(var_name) != NULL) {
                throw PqlParserError();
            }

            QueryVariable *qvar = new SimpleQueryVariable(var_name, pred);
            qvars()->insert(var_name, qvar);

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


  private:
    std::unique_ptr<SimpleTokenizer> _tokenizer;
    MatcherTable    *_matcher_table;
    PredicateTable  *_predicate_table;

    std::shared_ptr<PqlQuerySet> _query_set;

    SimpleToken     *_current_token;
};



}
}
