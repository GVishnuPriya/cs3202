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

#include "test/fixture.h"

namespace simple {
namespace test {

std::vector<PqlTestFixture> get_basic_test_fixtures() {
    std::vector<PqlTestFixture> fixtures;

    PqlTestFixture fixture1;
    fixture1.source = 
        "proc test1 { \n"
        "   a = 1; \n"
        "   while i { \n"
        "       call test2; \n"
        "       if j then { \n"
        "           x = (x+y)*(3+z); } else { \n"
        "           y = 2; } } \n"
        "   b = 4; } \n"
        "proc test2 { \n"
        "   c = 3; } \n";

    PqlQueryFixture query11;
    query11.query = 
        "stmt s; \n"
        "Select s such that Follows(1, s);";
    query11.expected = "s = 2";

    fixture1.queries.push_back(query11);

    PqlQueryFixture query12;
    query12.query =
        "stmt s; var v; \n"
        "Select v such that Next(2, s) and Modifies(s, v);";
    query12.expected = "v = \"b\", \"c\"";

    fixture1.queries.push_back(query12);

    fixtures.push_back(fixture1);

    return fixtures;
}

}
}
