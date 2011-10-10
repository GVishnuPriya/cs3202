
#pragma once

#include "simple/solver.h"

class ModifiesSolver {
  public:
    ModifiesSolver(std::shared_ptr<SimpleRoot> ast, SolverTable table) : _ast(ast) { }

    /*
     * Dependencies allows a solver to depend on another solver so that 
     * they can share memoized tables. the solver's dependencies are 
     * guaranteed to be in the solver table during construction, or 
     * runtime exception will be raised.
     */
    std::list<std::string> get_dependencies() {
        return std::list<std::string>(); // no dependency
    }

    std::string get_name() {
        return "Modifies"
    }

    /*
     * SOLVE RIGHT PART
     */

    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return ConditionSet(); // empty set
    }

    template <>
    ConditionSet solve_right<StatementAst>(StatementAst *ast) {
        ModifiesRightStatementVisitor visitor(this);

        ast->accept_statement_visitor(&visitor);

        return visitor.return_result();
    }

    template <>
    ConditionSet solve_right<ConditionalAst>(ConditionalAst *ast) {
        ConditionSet result;
        
        StatementAst *then = ast->get_then_branch();
        while(then != NULL) {
            result.union_with(solve_right<StatementAst>(then));
            then = then->next();
        }

        StatementAst *el = ast->get_else_branch();
        while(el != NULL) {
            result.union_with(solve_right<StatementAst>(el));
            then = then->next();
        }

        return result;
    }

    template <>
    ConditionSet solve_right<WhileAst>(WhileAst *ast) {
        ConditionSet result;

        StatementAst *body = ast->get_body();
        while(body != NULL) {
            result.union_with(solve_right<StatementAst>(body));
        }

        return result;
    }

    template <>
    ConditionalSet solve_right<AssignmentAst>(AssignmentAst *ast) {
        ConditionSet result;
        result.insert(new VariableCondition(
                ast->get_lhs_variable()));
        return result;
    }

    template <>
    ConditionalSet solve_right<CallAst>(CallAst *ast) {
        return solve_right<ProcAst>(ast->get_proc());
    }

    template <>
    ConditionalSet solve_right<ProcAst>(ProcAst *ast) {
        ConditionSet result;
        
        StatementAst *body = ast->get_statement();
        while(body != NULL) {
            result.union_with(solve_right<StatementAst>(body));
        }

        return result;
    }

    /*
     * SOLVE LEFT PART
     */

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return ConditionSet(); // empty set
    }

    template <>
    ConditionSet solve_left<SimpleVariable>(SimpleVariable *variable) {
        ConditionSet result;
        for(auto pit = _ast_root.begin_procs(); pit != _ast_root.end_procs(); ++pit) {
            result.union_with(solve_variable<ProcAst>(*pit));
        }

        return result;
    }

    template <typename Condition>
    ConditionSet solve_variable(Condition *condition) {
        return ConditionSet(); // empty set
    }

    template <>
    ConditionSet solve_variable<ProcAst>(ProcAst *ast, SimpleVariable *variable) {
        ConditionSet result;
        StatementAst *statements = ast->get_statement();

        while(statements != NULL) {
            result.union_with(solve_variable<StatementAst>(statements));
            statements = statements.next();
        }

        if(!result.is_empty()) {
            result.insert(new ProcCondition(ast));
        }

        return result;
    }

    template <>
    ConditionSet solve_variable<CallAst>(CallAst *ast, SimpleVariable *variable) {
        if(validate<CallAst, SimpleVariable>(ast, variable)) {
            ConditionSet result;
            result.insert(new StatementCondition(ast));
        } else {
            return ConditionSet(); // empty set
        }
    }

    template <>
    ConditionSet solve_variable<AssignmentAst>(
            AssignmentAst *ast, SimpleVariable *variable) 
    {
        if(validate<ExprAst, SimpleVariable>(ast, variable)) {
            ConditionSet result;
            result.insert(new StatementCondition(ast));
            return result;
        } else {
            return ConditionSet(); // empty set
        }
    }

    template <>
    bool validate<StatementAst, SimpleVariable>(
            StatementAst *ast, SimpleVariable *var) 
    {
        ModifiesValidateStatementVisitor visitor(this, var);
        ast->accept_statement_visitor(&visitor);
        return visitor.return_result();
    }

    template <>
    bool validate<ProcAst, SimpleVariable>(
            ProcAst *ast, SimpleVariable *var)
    {
        StatementAst *body = ast->get_statement();
        while(body != NULL) {
            if(validate<StatementAst, SimpleVariable>(body, var)) {
                return true;
            }
            body = body->next();
        }

        return false;
    }

    template <>
    bool validate<AssignmentAst, SimpleVariable>(
            AssignmentAst *ast, SimpleVariable *var) 
    {
        return ast->get_lhs_variable() == *var;
    }

    template <>
    bool validate<ConditionalAst, SimpleVariable>(
            ConditionalAst *ast, SimpleVariable *var) 
    {
        StatementAst *then_branch = ast->get_then_branch();
        while(then_branch != NULL) {
            if(validate<StatementAst, SimpleVariable>(then_branch, var)) {
                return true;
            }
            then_branch = then_branch->next();
        }

        StatementAst *else_branch = ast->get_else_branch();
        while(else_branch != NULL) {
            if(validate<StatementAst, SimpleVariable>(else_branch, var)) {
                return true;
            }
            else_branch = else_branch->next();
        }

        return false;
    }

    template <>
    bool validate<WhileAst, SimpleVariable>(
            WhileAst *ast, SimpleVariable *var)
    {
        StatementAst *body = ast->get_body();
        while(body != NULL) {
            if(validate<StatementAst, SimpleVariable>(body, var)) {
                return true;
            }
            body = body->next();
        }

        return false;
    }

    template <>
    bool validate<CallAst, SimpleVariable>(
            CallAst *ast, SimpleVariable *var)
    {
        return validate<ProcAst, SimpleVariable>(ast->get_proc_called());
    }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2) {
        return false;
    }

  private:
    std::shared_ptr<SimpleRoot> _ast_root;

    class ModifiesRightStatementVisitor : public StatementVisitor {
      public:
        ModifiesRightStatementVisitor(ModifiesSolver *visitor) : _visitor(visitor) { }

        virtual void visit_conditional(ConditionalAst *ast) {
            _result = _solver->solve_right<ConditionalAst>(ast);
        }

        virtual void visit_while(WhileAst *ast) {
            _result = _solver->solve_right<WhileAst>(ast);
        }

        virtual void visit_assignment(AssignmentAst *ast) {
            _result = _solver->solve_right<AssignmentAst>(ast);
        }

        virtual void visit_call(CallAst *ast) {
            _result = _solver->solve_right<CallAst>(ast);
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        ModifiesSolver *_solver;
        ConditionSet _result;
    };

    class ModifiesLeftStatementVisitor : public StatementVisitor {
      public:
        ModifiesLeftStatementVisitor(ModifiesSolver *visitor) : _visitor(visitor) { }

        virtual void visit_conditional(ConditionalAst *ast) {
            _result = _solver->solve_left<ConditionalAst>(ast);
        }

        virtual void visit_while(WhileAst *ast) {
            _result = _solver->solve_left<WhileAst>(ast);
        }

        virtual void visit_assignment(AssignmentAst *ast) {
            _result = _solver->solve_left<AssignmentAst>(ast);
        }

        virtual void visit_call(CallAst *ast) {
            _result = _solver->solve_left<CallAst>(ast);
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        ModifiesSolver *_solver;
        ConditionSet _result;
    };

    class ModifiesValidateStatementVisitor : public StatementVisitor {
      public:
        ModifiesValidateStatementVisitor(ModifiesSolver *visitor, SimpleVariable *var) : 
            _visitor(visitor), _var(var) { }

        virtual void visit_conditional(ConditionalAst *ast) {
            _result = _solver->solve_right<ConditionalAst, SimpleVariable>(ast, _var);
        }

        virtual void visit_while(WhileAst *ast) {
            _result = _solver->solve_right<WhileAst, SimpleVariable>(ast, _var);
        }

        virtual void visit_assignment(AssignmentAst *ast) {
            _result = _solver->solve_right<AssignmentAst, SimpleVariable>(ast, _var);
        }

        virtual void visit_call(CallAst *ast) {
            _result = _solver->solve_right<CallAst, SimpleVariable>(ast, _var);
        }

        ConditionSet return_result() {
            return std::move(_result);
        }

      private:
        ModifiesSolver *_solver;
        SimpleVariable *_var;
        ConditionSet _result;
    };

};
