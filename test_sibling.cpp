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
#include "simple/util/ast_utils.h"
#include "impl/solvers/sibling.h"
#include "impl/ast.h"
#include "impl/condition.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(SiblingTest, FixtureTest1) {
    /*
     * proc first {
     *   x = y;
     * }
	 * proc second {
	 * }
     */
    SimpleProcAst *proc_first = new SimpleProcAst("first");
	SimpleProcAst *proc_second = new SimpleProcAst("second");

    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");
    SimpleVariable var2("y"); // different name
    SimpleVariable var3("y"); // same name different pointer

    assign->set_variable(var);
    assign->set_expr(new SimpleVariableAst(var2));
    assign->set_line(1);
    assign->set_proc(proc_first);

    proc_first->set_first_statement(assign);

	std::vector<ProcAst*> procs;
    procs.push_back(proc_first);
    procs.push_back(proc_second);

    SimpleRoot root(procs.begin(), procs.end());

    SiblingSolver solver(root);

    /*
     * Positive testing
     */
    EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc_first, proc_second)));
	EXPECT_TRUE((solver.validate<ProcAst, ProcAst>(proc_second, proc_first)));

	/*
	 * Negative testing
	 */
	SimpleProcAst *proc_wrong = new SimpleProcAst("wrong");
	EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc_wrong, proc_second)));
	EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc_second, proc_wrong)));
	EXPECT_FALSE((solver.validate<ProcAst, ProcAst>(proc_second, proc_second)));
}

}
}
