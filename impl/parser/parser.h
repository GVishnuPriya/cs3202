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

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include "simple/ast.h"
#include "simple/util/ast_utils.h"
#include "impl/ast.h"
#include "impl/parse_error.h"
#include "impl/parser/token.h"
#include "impl/parser/tokenizer.h"

namespace simple {
namespace parser {

using namespace simple::impl;
using namespace simple;

class SimpleParser {
  public:
    SimpleParser(SimpleTokenizer *tokenizer);

    SimpleRoot parse_program();

    ExprAst* parse_expr();

    SimpleStatementAst* parse_statement(ProcAst *proc, ContainerAst *parent = NULL);

    SimpleProcAst* parse_proc();

    SimpleAssignmentAst* parse_assignment();

    SimpleIfAst* parse_if(ProcAst *proc, ContainerAst *parent = NULL);

    SimpleWhileAst* parse_while(ProcAst *proc, ContainerAst *parent = NULL);

    SimpleCallAst* parse_call();

    int current_source_line();
    int current_statement_line();

    std::map<std::string, SimpleProcAst*>& get_procs_table();

    SimpleProcAst* get_proc(const std::string& proc_name);

    const LineTable& get_source_line_table();
    const LineTable& get_statement_line_table();

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

    SimpleToken* next_token();

    SimpleToken* current_token();

    SimpleProcAst* get_or_create_proc(const std::string& proc_name);

    ExprAst* parse_binary_op_rhs(int precedence, ExprAst* lhs);

    ExprAst* parse_primary();

    SimpleConstAst* parse_const();

    SimpleVariableAst* parse_variable();

    ExprAst* parse_parent_expr();

    int operator_precedence();

    /*
     * Validate if all call statements refer to a valid procedure.
     */
    void validate_all_procs_exist();

  private:
    std::map<std::string, SimpleProcAst*> _procs_table;
    
    int _source_line;
    int _statement_line;

    LineTable _source_line_table;
    LineTable _statement_line_table;

    /*
     * The tokenizer takes ownership of the produced token pointers
     * so that it can reuse the token objects in future. Current token
     * is assumed to be destroyed once next_token() is called.
     */
    SimpleToken* _current_token;
    std::vector<ProcAst*> _procs;

    std::unique_ptr<SimpleTokenizer> _tokenizer;
};

} // namespace parser
} // namespace simple
