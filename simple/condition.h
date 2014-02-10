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

#include "simple/ast.h"

namespace simple {

class ConditionVisitor;

class SimpleCondition {
  public:
    virtual std::size_t get_hash() = 0;

    virtual void accept_condition_visitor(ConditionVisitor *visitor) = 0;
    
    virtual ~SimpleCondition() { }
};

class StatementCondition : public SimpleCondition {
  public:
    virtual StatementAst* get_statement_ast() = 0;

    virtual ~StatementCondition() { }
};

class ProcCondition : public SimpleCondition {
  public:
    virtual ProcAst* get_proc_ast() = 0;

    virtual ~ProcCondition() { }
};

class VariableCondition : public SimpleCondition {
  public:
    virtual SimpleVariable* get_variable() = 0;
    
    virtual ~VariableCondition() { }
};

class ConstantCondition : public SimpleCondition {
  public:
    virtual SimpleConstant* get_constant() = 0;

    virtual ~ConstantCondition() { }
};

class PatternCondition : public SimpleCondition {
  public:
    virtual ExprAst* get_expr_ast() = 0;

    virtual ~PatternCondition() { }
};

class ConditionVisitor {
  public:
    virtual void visit_statement_condition(StatementCondition*) = 0;
    virtual void visit_proc_condition(ProcCondition*) = 0;
    virtual void visit_variable_condition(VariableCondition*) = 0;
    virtual void visit_constant_condition(ConstantCondition*) = 0;
    virtual void visit_pattern_condition(PatternCondition*) = 0;

    virtual ~ConditionVisitor() { }
};


} // namespace simple
