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

#include "impl/parser/parser.h"

namespace simple {
namespace parser {

using namespace simple;
using namespace simple::impl;

SimpleParser::SimpleParser(SimpleTokenizer *tokenizer) :
    _source_line(1),
    _statement_line(1),
    _tokenizer(tokenizer)
{ 
    next_token();
}

SimpleRoot SimpleParser::parse_program() {
    while(!current_token_is<EOFToken>()) {
        _procs.push_back(parse_proc());
    }

    validate_all_procs_exist();
    return SimpleRoot(_procs.begin(), _procs.end());
}

ExprAst* SimpleParser::parse_expr() {
    ExprAst *primary = parse_primary();
    return parse_binary_op_rhs(0, primary);
}

SimpleStatementAst* SimpleParser::parse_statement(ProcAst *proc, ContainerAst *parent) {
    std::string identifier = current_token_as<IdentifierToken>()->get_content();
    SimpleStatementAst *statement;

    int source_line = _source_line;
    int statement_line = _statement_line++;

    if(identifier == "if") {
        statement = parse_if(proc, parent);
    } else if(identifier == "while") {
        statement = parse_while(proc, parent);
    } else if(identifier == "call") {
        statement = parse_call();
    } else {
        statement = parse_assignment();
    }

    statement->set_proc(proc);
    statement->set_container(parent);
    statement->set_source_line(source_line);
    statement->set_statement_line(statement_line);

    _source_line_table[source_line] = statement->as_ast();
    _statement_line_table[statement_line] = statement->as_ast();

    return statement;
}

SimpleProcAst* SimpleParser::parse_proc() {
    if(current_token_as<IdentifierToken>()->get_content() != "procedure") {
        throw ParseError("Expected \"procedure\" keyword for procedure declaration.");
    }

    std::string name = next_token_as<
        IdentifierToken>()->get_content(); // eat 'proc'

    SimpleProcAst *proc = get_or_create_proc(name);

    next_token_as<OpenBraceToken>(); // eat name
    next_token(); // eat '{'

    SimpleStatementAst *current_stat = parse_statement(proc);
    proc->set_first_statement(current_stat->as_ast());

    while(!current_token_is<CloseBraceToken>()) {
        SimpleStatementAst *next_stat = parse_statement(proc);
        current_stat->set_next(next_stat->as_ast());
        next_stat->set_prev(current_stat->as_ast());

        current_stat = next_stat;
    }

    next_token(); // eat '}'

    return proc;
}

SimpleAssignmentAst* SimpleParser::parse_assignment() {
    SimpleVariable var(current_token_as<IdentifierToken>()->get_content());
    next_token_as<EqualToken>(); // eat var name
    next_token(); // eat '='

    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    assign->set_variable(var);
    assign->set_expr(parse_expr());

    current_token_as<SemiColonToken>();
    next_token(); // eat ';'

    return assign;
}

SimpleIfAst* SimpleParser::parse_if(ProcAst *proc, ContainerAst *parent) {
    if(current_token_as<IdentifierToken>()->get_content() != "if") {
        throw ParseError("Expected \"if\" token at line " + _source_line);
    }

    // eat 'if'
    SimpleVariable var(next_token_as<IdentifierToken>()->get_content());
    SimpleIfAst *condition = new SimpleIfAst();
    condition->set_variable(var);

    // eat var name
    next_token();

    if(!current_token_is<IdentifierToken>()) {
        throw ParseError("Expected \"then\" token after if expression at line " + _source_line);   
    }

    std::string then_token = current_token_as<IdentifierToken>()->get_content();
    if(then_token != "then") {
        throw ParseError("Expected \"then\" token after if expression at line " + _source_line);   
    }

    next_token_as<OpenBraceToken>(); // eat then
    next_token(); // eat '{'

    SimpleStatementAst *current = parse_statement(proc, condition);
    condition->set_then_branch(current->as_ast());
    
    while(!current_token_is<CloseBraceToken>()) {
        SimpleStatementAst *next = parse_statement(proc, condition);
        current->set_next(next->as_ast());
        next->set_prev(current->as_ast());
        
        current = next;
    }

    // eat '}'
    if(next_token_as<IdentifierToken>()->get_content() != "else") {
        throw ParseError("Expected \"else\" token at line " + _source_line);   
    }

    next_token_as<OpenBraceToken>(); // eat 'else'
    next_token(); // eat '{'

    current = parse_statement(proc, condition);
    condition->set_else_branch(current->as_ast());
    
    while(!current_token_is<CloseBraceToken>()) {
        SimpleStatementAst *next = parse_statement(proc, condition);
        current->set_next(next->as_ast());
        next->set_prev(current->as_ast());
        
        current = next;
    }
    
    next_token(); // eat '}'

    return condition;
}

SimpleWhileAst* SimpleParser::parse_while(ProcAst *proc, ContainerAst *parent) {
    if(current_token_as<IdentifierToken>()->get_content() != "while") {
        throw ParseError("Expected \"while\" token at line " + _source_line);   
    }

    // eat 'while'
    SimpleVariable var(next_token_as<IdentifierToken>()->get_content());
    SimpleWhileAst *loop = new SimpleWhileAst();
    loop->set_variable(var);

    next_token_as<OpenBraceToken>(); // eat var name
    next_token(); // eat '{'

    SimpleStatementAst *current = parse_statement(proc, loop);
    loop->set_body(current->as_ast());
    
    while(!current_token_is<CloseBraceToken>()) {
        SimpleStatementAst *next = parse_statement(proc, loop);
        current->set_next(next->as_ast());
        next->set_prev(current->as_ast());
        
        current = next;
    }

    next_token(); // eat '}'

    return loop;
}

SimpleCallAst* SimpleParser::parse_call() {
    if(current_token_as<IdentifierToken>()->get_content() != "call") {
        throw ParseError("Expected \"call\" token at line " + _source_line);   
    }

    // eat 'call'
    std::string proc_name(next_token_as<IdentifierToken>()->get_content());
    SimpleProcAst *proc = get_or_create_proc(proc_name);

    SimpleCallAst *call = new SimpleCallAst();
    call->set_proc_called(proc);

    next_token_as<SemiColonToken>(); // eat proc name
    next_token(); // eat ';'

    return call;
}

int SimpleParser::current_source_line() {
    return _source_line;
}

int SimpleParser::current_statement_line() {
    return _statement_line;
}

std::map<std::string, SimpleProcAst*>& SimpleParser::get_procs_table() {
    return _procs_table;
}

SimpleProcAst* SimpleParser::get_proc(const std::string& proc_name) {
    if(_procs_table.count(proc_name) == 0) {
        return NULL;
    } else {
        return _procs_table[proc_name];
    }
}

const LineTable& SimpleParser::get_source_line_table() {
    return _source_line_table;
}

const LineTable& SimpleParser::get_statement_line_table() {
    return _statement_line_table;
}

SimpleToken* SimpleParser::next_token() {
    _current_token = _tokenizer->next_token();
    if(current_token_is<NewLineToken>()) {
        ++_source_line;
        return next_token();
    } else {
        return _current_token;
    }
}

SimpleToken* SimpleParser::current_token() {
    return _current_token;
}

SimpleProcAst* SimpleParser::get_or_create_proc(const std::string& proc_name) {
    SimpleProcAst *proc;

    if(_procs_table.count(proc_name) == 0) {
        proc = new SimpleProcAst(proc_name);
        _procs_table[proc_name] = proc;
    } else {
        proc = _procs_table[proc_name];
    }

    return proc;
}

ExprAst* SimpleParser::parse_binary_op_rhs(int precedence, ExprAst* lhs) {
    while(true) {
        int current_precedence = operator_precedence();
        if(current_precedence < precedence) {
            return lhs;
        }

        char current_op = current_token_as<OperatorToken>()->get_op();
        next_token();

        ExprAst *rhs = parse_primary();
        int next_precedence = operator_precedence();
        if(current_precedence < next_precedence) {
            rhs = parse_binary_op_rhs(current_precedence+1, rhs);
        }
        lhs = new SimpleBinaryOpAst(current_op, lhs, rhs);
    }
}

ExprAst* SimpleParser::parse_primary() {
    SimpleToken *token = current_token();
    if(try_token<IdentifierToken>(token)) {
        return parse_variable();
    } else if(try_token<IntegerToken>(token)) {
        return parse_const();
    } else if(try_token<OpenBracketToken>(token)) {
        return parse_parent_expr();
    } else {
        throw ParseError("Invalid primary expression at line " + _source_line);
    }
}

SimpleConstAst* SimpleParser::parse_const() {
    int value = current_token_as<IntegerToken>()->get_value();
    next_token();
    return new SimpleConstAst(value);
}

SimpleVariableAst* SimpleParser::parse_variable() {
    std::string name = current_token_as<IdentifierToken>()->get_content();
    next_token();
    return new SimpleVariableAst(name);
}

ExprAst* SimpleParser::parse_parent_expr() {
    current_token_as<OpenBracketToken>();
    next_token(); // eat'('

    ExprAst *expr = parse_expr();

    current_token_as<CloseBracketToken>();
    next_token(); // eat ')'

    return expr;
}

int SimpleParser::operator_precedence() {
    if(current_token_is<OperatorToken>()) {
        OperatorToken *token = current_token_as<OperatorToken>();
        switch(token->get_op()) {
            case '+':
            case '-':
                return 100;
            break;
            case '*':
                return 200;
            break;
            default:
                return -1; // not supported at the moment
            break;
        }
    } else {
        return -1;
    }
}

/*
 * Validate if all call statements refer to a valid procedure.
 */
void SimpleParser::validate_all_procs_exist() {
    if(_procs.size() != _procs_table.size()) {
        throw ParseError("Missing procedure definition in source file");
    }

    for(std::vector<ProcAst*>::iterator it = _procs.begin();
            it != _procs.end(); ++it)
    {
        std::string proc_name = (*it)->get_name();
        if(_procs_table.count(proc_name) != 1 ||
           _procs_table[proc_name] != *it) 
        {
            throw ParseError("Missing procedure definition " + proc_name);
        }
    }
}

}
}

