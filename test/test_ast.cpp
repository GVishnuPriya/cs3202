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
#include "impl/ast.h"

using namespace simple;
using namespace simple::impl;

TEST(AstTest, BasicTest) {
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

    SimpleRoot root(proc);
    
    EXPECT_EQ(proc->get_statement(), assign);
    EXPECT_TRUE(assign->get_variable()->equals(var));

    EXPECT_EQ(*root.begin(), proc);
}
