
#include <string>
#include <memory>
#include <exception>
#include "simple/ast.h"
#include "simple/util/ast_utils.h"
#include "impl/ast.h"
#include "impl/parser/token.h"

namespace simple {
namespace parser {

class ParserError : public std::exception { };

class SimpleParser {
  public:
    SimpleParser(SimpleTokenizer *tokenizer, Iterator begin, Iterator end) :
        _tokenizer(tokenizer), _begin(begin), _end(end), _line(0)
    { }

    SimpleRoot parse_program() {

    }

    ExprAst* parse_expr() {
        ExprAst *primary = parse_primary();
        return parse_binary_op_rhs(0, primary);
    }

    SimpleStatementAst* parse_statement(ProcAst *proc, ContainerAst *parent = NULL) {
        std::string identifier = current_token_as<IdentifierToken>()->get_content();
        SimpleStatementAst *statement;
        int line = get_line_num();

        if(identifier == "if") {
            statement = parse_conditional(proc, parent);
        } else if(identifier == "while") {
            statement = parse_while(proc, parent);
        } else if(identifier == "call") {
            satement = parse_call();
        } else {
            statement = parse_assignment();
        }

        statement->set_proc(proc);
        statement->set_parent(parent);
        statement->set_line(line);

        return statement;
    }

    SimpleProcAst* parse_proc() {
        if(current_token_as<IdentifierToken>()->get_content() != "proc") {
            throw ParserError();
        }
        std::string name = next_token_as<
            IdentifierToken>()->get_content(); // eat 'proc'

        SimpleProcAst *proc = new SimpleProcAst(name);
        next_token_as<OpenBraceToken>(); // eat name
        next_token(); // eat '{'

        SimpleStatementAst *current_stat = parse_statement(proc);
        proc->set_first_statement(current_stat);

        while(!current_token_is<CloseBraceToken>()) {
            SimpleStatementAst *next_stat = parse_statement(proc);
            current_stat->set_next(next_stat->as_ast());
            next_stat->set_prev(current_stat->as_ast());

            current_stat = next_stat;
        }

        next_token(); // eat '}'

        return proc;
    }

    SimpleAssignmentAst* parse_assignment() {
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

    SimpleConditionalAst* parse_conditional(ProcAst *proc, ContainerAst *parent = NULL) {
        if(current_token_as<IdentifierToken>()->get_content() != "if") {
            throw ParserError();
        }

        // eat 'if'
        SimpleVariable var(next_token_as<IdentifierToken>()->get_content());
        SimpleConditionalAst *condition = new SimpleConditionalAst();
        condition->set_var(var);

        next_token_as<OpenBraceToken>(); // eat var name
        next_token(); // eat '{'

        SimpleStatementAst *current = parse_statement(proc, condition);
        condition->set_then_branch(current);
        
        while(!current_token_is<CloseBraceToken>()) {
            SimpleStatementAst *next = parse_statement(proc, condition);
            current->set_next(next->as_ast());
            next->set_prev(current->as_ast());
            
            current = next;
        }

        next_token(); // eat '}'

        *current = parse_statement(proc, condition);
        condition->set_else_branch(current);
        
        while(!current_token_is<CloseBraceToken>()) {
            SimpleStatementAst *next = parse_statement(proc, condition);
            current->set_next(next->as_ast());
            next->set_prev(current->as_ast());
            
            current = next;
        }
        
        next_token(); // eat '}'

        return condition;
    }

    SimpleWhileAst* parse_while(ProcAst *proc, ContainerAst *parent = NULL) {
        if(current_token_as<IdentifierToken>()->get_content() != "while") {
            throw ParserError();
        }

        // eat 'while'
        SimpleVariable var(next_token_as<IdentifierToken>()->get_content());
        SimpleWhileAst *loop = new SimpleWhileAst();
        loop->set_var(var);

        next_token_as<OpenBraceToken>(); // eat var name
        next_token(); // eat '{'

        SimpleStatementAst *current = parse_statement(proc, loop);
        condition->set_body(current);
        
        while(!current_token_is<CloseBraceToken>()) {
            SimpleStatementAst *next = parse_statement(proc, loop);
            current->set_next(next->as_ast());
            next->set_prev(current->as_ast());
            
            current = next;
        }

        next_token(); // eat '}'

        return loop;
    }

    SimpleCallAst* parse_call() {
        if(current_token_as<IdentifierToken>()->get_content() != "call") {
            throw ParserError();
        }

        // eat 'call'
        std::string proc_name(next_token_as<IdentifierToken>()->get_content());
        SimpleProcAst *proc;

        if(_procs_table.count(proc_name) == 0) {
            proc = new SimpleProcAst(proc_name);
            _procs_table[proc_name] = proc;
        } else {
            proc = _procs_table[proc_name];
        }

        SimpleCallAst *call = new SimpleCallAst();
        call->set_proc_called(proc);

        next_token_as<SemiColonToken>(); // eat proc name
        next_token(); // eat ';'

        return call;
    }
  protected:
    template <TokenType>
    TokenType* next_token_as() {
        return token_cast<TokenType>(next_token());
    }

    template <TokenType>
    SimpleToken* current_token_as() {
        return token_cast<TokenType>(current_token());
    }

    template <TokenType>
    bool current_token_is() {
        if(try_token<TokenType>(current_token()) != NULL) {
            return true;
        } else {
            return false;
        }
    }

    SimpleToken* next_token() {
        _current_token.reset(_tokenizer->next_token());
        return _current_token.get();
    }

    SimpleToken* current_token() {
        return _current_token.get();
    }

    SimpleExprAst* parse_binary_op_rhs(int precedence, ExprAst* lhs) {
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

    SimpleExprAst* parse_primary() {
        SimpleToken *token = current_token();
        if(try_token<IdentifierToken>(token)) {
            return parse_variable();
        } else if(try_token<IntegerToken>(token)) {
            return parse_const();
        } else if(try_token<OpenBracketToken>(token)) {
            return parse_parent_expr();
        } else {
            throw ParserError();
        }
    }

    SimpleConstAst* parse_const() {
        int = current_token_as<IntegerToken>()->get_value();
        next_token();
        return new SimpleConstAst(int);
    }

    SimpleVariableAst* parse_variable() {
        std::string name = current_token_as<IdentifierToken>()->get_content();
        next_token();
        return new SimpleVariableAst(name);
    }

    SimpleExprAst* parse_parent_expr() {
        current_token_as<OpenBracketToken>();
        next_token(); // eat'('

        SimpleExprAst *expr = parse_expr();

        current_token_as<CloseBracketToken>();
        next_token(); // eat ')'

        return expr;
    }

    int operator_precedence() {
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
  private:
    Iterator _begin;
    Iterator _end;
    unsigned int _line;
    std::map<std::string, SimpleProcAst*> _procs_table;

    std::unique_ptr<SimpleToken> _current_token;
    std::vector<ProcAst*> _proc;
};

} // namespace parser
} // namespace simple
