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

#include <iterator>
#include "gtest/gtest.h"
#include "impl/frontend.h"
#include "test/fixture.h"

#include "test/fixtures/basic_queries.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;

TEST(FrontEndTest, IntegratedTest) {
    std::string source = 
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

    SimplePqlFrontEnd frontend(source.begin(), source.end());

    std::string query1 = 
        "stmt s; \n"
        "Select s such that Follows(1, s);";

    std::string result1;
    
    frontend.process_query(query1.begin(), query1.end(), std::back_inserter(result1));
    EXPECT_EQ(result1, "s = 2");

    std::string query2 = 
        "stmt s; var v; \n"
        "Select v such that Next(2, s) and Modifies(s, v);";

    std::string result2;
    frontend.process_query(query2.begin(), query2.end(), std::back_inserter(result2));
    EXPECT_EQ(result2, "v = \"b\", \"c\"");
}

class FrontEndFixtureTest : public testing::TestWithParam<PqlTestFixture> {

};

TEST_P(FrontEndFixtureTest, BasicTest) {
    PqlTestFixture fixture = GetParam();
    SimplePqlFrontEnd frontend(fixture.source.begin(), fixture.source.end());

    for(std::vector<PqlQueryFixture>::iterator it = fixture.queries.begin();
            it != fixture.queries.end(); ++it)
    {
        std::string result;
        frontend.process_query(it->query.begin(), it->query.end(), std::back_inserter(result));
        EXPECT_EQ(result, it->expected);
    }
}

INSTANTIATE_TEST_CASE_P(BasicFixtures, FrontEndFixtureTest,
        testing::ValuesIn(get_basic_test_fixtures()));

}
}
