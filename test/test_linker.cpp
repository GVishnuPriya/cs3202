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
#include "impl/condition.h"
#include "impl/linker.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;

ConditionTuplePtr make_tuples(
        ConditionPtr condition1, ConditionPtr condition2)
{
    return ConditionTuplePtr(
            new SimpleConditionTuple(condition1,
            new SimpleConditionTuple(condition2)));
}

ConditionTuplePtr make_tuples(ConditionPtr condition1, 
        ConditionPtr condition2, ConditionPtr condition3)
{
    return ConditionTuplePtr(
            new SimpleConditionTuple(condition1,
            make_tuples(condition2, condition3)));
}

std::vector<std::string> make_string_list(
        std::string str1, std::string str2)
{
    std::vector<std::string> result;
    result.push_back(str1);
    result.push_back(str2);
    return result;
}

std::vector<std::string> make_string_list(
        std::string str1, std::string str2, std::string str3)
{
    std::vector<std::string> result;
    result.push_back(str1);
    result.push_back(str2);
    result.push_back(str3);
    return result;
}

TEST(TupleTest, TuplePtrEquality) {
    SimpleAssignmentAst stat1;
    SimpleAssignmentAst stat2;

    ConditionPtr condition1(new SimpleStatementCondition(&stat1));
    ConditionPtr condition2(new SimpleStatementCondition(&stat2));

    ConditionTuplePtr tuple1(make_tuples(condition1, condition2));
    ConditionTuplePtr tuple2(make_tuples(condition1, condition2));

    EXPECT_EQ(tuple1, tuple2);

    TupleSet list1;
    list1.insert(tuple1);
    list1.insert(tuple2);

    TupleSet list2;
    list2.insert(tuple1);
    list2.insert(tuple2);

    EXPECT_EQ(list1, list2);
}

TEST(TupleTest, TuplePtrTripleEquality) {
    SimpleAssignmentAst stat1;
    SimpleAssignmentAst stat2;
    SimpleAssignmentAst stat3;

    ConditionPtr condition1(new SimpleStatementCondition(&stat1));
    ConditionPtr condition2(new SimpleStatementCondition(&stat2));
    ConditionPtr condition3(new SimpleStatementCondition(&stat3));

    ConditionTuplePtr tuple1(make_tuples(condition1, condition2, condition3));
    ConditionTuplePtr tuple2(make_tuples(condition1, condition2, condition3));

    EXPECT_EQ(tuple1, tuple2);

    TupleSet list1;
    list1.insert(tuple1);
    list1.insert(tuple2);

    TupleSet list2;
    list2.insert(tuple1);
    list2.insert(tuple2);

    EXPECT_EQ(list1, list2);
}

TEST(LinkerTest, EmptyResultTest) {
    SimpleQueryLinker linker1;
    EXPECT_EQ(linker1.is_initialized("x"), false);
    linker1.update_results("x", ConditionSet());
    EXPECT_EQ(linker1.is_initialized("x"), true);
}

TEST(LinkerTest, TrivialTest) {
    SimpleAssignmentAst stat11;
    SimpleAssignmentAst stat12;
    SimpleAssignmentAst stat13;
    SimpleAssignmentAst stat14;
    SimpleAssignmentAst stat15;

    SimpleAssignmentAst stat21;
    SimpleAssignmentAst stat22;
    SimpleAssignmentAst stat23;
    SimpleAssignmentAst stat24;
    SimpleAssignmentAst stat25;

    ConditionPtr condition11(new SimpleStatementCondition(&stat11));
    ConditionPtr condition12(new SimpleStatementCondition(&stat12));
    ConditionPtr condition13(new SimpleStatementCondition(&stat13));
    ConditionPtr condition14(new SimpleStatementCondition(&stat14));
    ConditionPtr condition15(new SimpleStatementCondition(&stat15));

    ConditionPtr condition21(new SimpleStatementCondition(&stat21));
    ConditionPtr condition22(new SimpleStatementCondition(&stat22));
    ConditionPtr condition23(new SimpleStatementCondition(&stat23));
    ConditionPtr condition24(new SimpleStatementCondition(&stat24));
    ConditionPtr condition25(new SimpleStatementCondition(&stat25));

    SimpleQueryLinker linker1;

    /*
     * Before:
     * X = 11,12,13
     * Y = 23,24,25
     *
     * Apply:
     * <X,Y> = <11,23>, <13,25>, <14, 23>, <11, 21>
     *
     * After:
     * X = 11, 13
     * Y = 23, 25
     */

    ConditionSet x1;
    x1.insert(condition11);
    x1.insert(condition12);
    x1.insert(condition13);

    ConditionSet y1;
    y1.insert(condition23);
    y1.insert(condition24);
    y1.insert(condition25);

    linker1.init_qvar("x", x1);
    linker1.init_qvar("y", y1);

    EXPECT_EQ(linker1.get_conditions("x"), x1);
    EXPECT_EQ(linker1.get_conditions("y"), y1);

    std::set<ConditionPair> links1;
    links1.insert(ConditionPair(condition11, condition23));
    links1.insert(ConditionPair(condition13, condition25));

    linker1.update_links("x", "y", links1);

    ConditionSet new_x1;
    new_x1.insert(condition11);
    new_x1.insert(condition13);

    ConditionSet new_y1;
    new_y1.insert(condition23);
    new_y1.insert(condition25);

    EXPECT_EQ(linker1.get_conditions("x"), new_x1);
    EXPECT_EQ(linker1.get_conditions("y"), new_y1);

    /*
     * Updating the link again with additional links that
     * has conditions not currently in the query variable
     * will not affect the original results.
     */
    std::set<ConditionPair> links2;
    links2.insert(ConditionPair(condition11, condition23));
    links2.insert(ConditionPair(condition13, condition25));
    links2.insert(ConditionPair(condition14, condition23));
    links2.insert(ConditionPair(condition11, condition21));

    linker1.update_links("x", "y", links2);

    EXPECT_EQ(linker1.get_conditions("x"), new_x1);
    EXPECT_EQ(linker1.get_conditions("y"), new_y1);


    TupleSet tuples_xy;
    tuples_xy.insert(make_tuples(condition11, condition23));
    tuples_xy.insert(make_tuples(condition13, condition25));

    TupleSet tuples_yx;
    tuples_yx.insert(make_tuples(condition23, condition11));
    tuples_yx.insert(make_tuples(condition25, condition13));

    EXPECT_EQ(linker1.make_tuples(make_string_list("x", "y")), tuples_xy);
    EXPECT_EQ(linker1.make_tuples(make_string_list("y", "x")), tuples_yx);
            
    ConditionSet x2;
    x2.insert(condition11);
    x2.insert(condition12);
    x2.insert(condition13);
    x2.insert(condition14);
    x2.insert(condition15);

    /*
     * Updating with a superset of original results should produce the 
     * same results
     */
    linker1.update_results("x", x2);
    EXPECT_EQ(linker1.get_conditions("x"), new_x1);

    ConditionSet y2;
    y2.insert(condition21);
    y2.insert(condition22);
    y2.insert(condition23);
    y2.insert(condition24);

    ConditionSet new_x2;
    new_x2.insert(condition11);

    ConditionSet new_y2;
    new_y2.insert(condition23);

    /*
     * Updating with a different set of results will produce the
     * intersection between the new results and the old results.
     *
     * In this case because condition25 is not in the new results,
     * it is removed from the old results.
     */
    linker1.update_results("y", y2);
    EXPECT_EQ(linker1.get_conditions("y"), new_y2);

    /*
     * Since x is linked to y, the removal of condition25 affects
     * the removal of condition12 as well.
     */
    EXPECT_EQ(linker1.get_conditions("x"), new_x2);

    TupleSet tuples_xy2;
    tuples_xy2.insert(make_tuples(condition11, condition23));
    EXPECT_EQ(linker1.make_tuples(make_string_list("x", "y")), tuples_xy2);
}

TEST(LinkerTest, ThreeWayTest) {
    SimpleAssignmentAst stat11(11);
    SimpleAssignmentAst stat12(12);
    SimpleAssignmentAst stat13(13);
    SimpleAssignmentAst stat14(14);
    SimpleAssignmentAst stat15(15);

    SimpleAssignmentAst stat21(21);
    SimpleAssignmentAst stat22(22);
    SimpleAssignmentAst stat23(23);
    SimpleAssignmentAst stat24(24);
    SimpleAssignmentAst stat25(25);

    SimpleAssignmentAst stat31(31);
    SimpleAssignmentAst stat32(32);
    SimpleAssignmentAst stat33(33);
    SimpleAssignmentAst stat34(34);
    SimpleAssignmentAst stat35(35);

    ConditionPtr condition11(new SimpleStatementCondition(&stat11));
    ConditionPtr condition12(new SimpleStatementCondition(&stat12));
    ConditionPtr condition13(new SimpleStatementCondition(&stat13));
    ConditionPtr condition14(new SimpleStatementCondition(&stat14));
    ConditionPtr condition15(new SimpleStatementCondition(&stat15));

    ConditionPtr condition21(new SimpleStatementCondition(&stat21));
    ConditionPtr condition22(new SimpleStatementCondition(&stat22));
    ConditionPtr condition23(new SimpleStatementCondition(&stat23));
    ConditionPtr condition24(new SimpleStatementCondition(&stat24));
    ConditionPtr condition25(new SimpleStatementCondition(&stat25));

    ConditionPtr condition31(new SimpleStatementCondition(&stat31));
    ConditionPtr condition32(new SimpleStatementCondition(&stat32));
    ConditionPtr condition33(new SimpleStatementCondition(&stat33));
    ConditionPtr condition34(new SimpleStatementCondition(&stat34));
    ConditionPtr condition35(new SimpleStatementCondition(&stat35));

    /*
     * Initial:
     * X = 11, 12, 13
     * Y = 22, 23, 24
     * Z = 33, 34, 35
     *
     * Insert relation:
     * <X, Y> = <11, 22>, <11, 23>, <13, 23>, <12, 24>
     * <Y, Z> = <22, 35>, <23, 33>, <24, 34>, <24, 33>
     * <X, Z> = <11, 35>, <11, 33>, <13, 33>, <12, 34>, <12, 33>
     * <Z, X> = <35, 11>, <33, 11>, <33, 13>, <34, 12>, <33, 12>
     *
     * All Possible relations:
     * <X, Y, Z> = 
     * <11, 22, 35>, <11, 23, 33>, <13, 23, 33>, 
     * <12, 24, 34>, <12, 24, 33>
     *
     * The alternative ordering should produce the same result,
     * NOT the following:
     * <Y, Z, X> !=
     * <22, 35, 11>, <23, 33, 11>, <23, 33, 13>, <23, 33, 12>,
     * <24, 34, 12>, <24, 33, 11>, <24, 33, 13>, <24, 33, 12>
     *
     * <Z, X, Y> !=
     * <35, 11, 22>, <35, 11, 23>, <33, 11, 22>, <33, 11, 23>,
     * <33, 13, 23>, <34, 12, 24>, <33, 12, 24>
     *
     * Remove 11 from X:
     * <X, Y> = <13, 23>, <12, 24>
     * <Y, Z> = <23, 33>, <24, 34>, <24, 33>
     *
     * X = 12, 13
     * Y = 23, 24
     * Z = 33, 34
     *
     * All Possible Relations:
     * <X, Y, Z> = 
     * <13, 23, 33>, <12, 24, 34>, <12, 24, 33>
     *
     * Remove 23 from Z:
     * <X, Y> = <12, 24>
     * <Y, Z> = <24, 34>
     *
     * All Possible Relations:
     * <X, Y, Z> = <12, 24, 34>
     */

    ConditionSet x1;
    x1.insert(condition11);
    x1.insert(condition12);
    x1.insert(condition13);
    x1.insert(condition14);
    x1.insert(condition15);

    ConditionSet y1;
    y1.insert(condition21);
    y1.insert(condition22);
    y1.insert(condition23);
    y1.insert(condition24);
    y1.insert(condition25);

    ConditionSet z1;
    z1.insert(condition31);
    z1.insert(condition32);
    z1.insert(condition33);
    z1.insert(condition34);
    z1.insert(condition35);

    SimpleQueryLinker linker;

    linker.init_qvar("x", x1);
    linker.init_qvar("y", y1);
    linker.init_qvar("z", z1);

    std::set<ConditionPair> links_xy1;
    links_xy1.insert(ConditionPair(condition11, condition22));
    links_xy1.insert(ConditionPair(condition11, condition23));
    links_xy1.insert(ConditionPair(condition13, condition23));
    links_xy1.insert(ConditionPair(condition12, condition24));

    std::set<ConditionPair> links_yz1;
    links_yz1.insert(ConditionPair(condition22, condition35));
    links_yz1.insert(ConditionPair(condition23, condition33));
    links_yz1.insert(ConditionPair(condition24, condition34));
    links_yz1.insert(ConditionPair(condition24, condition33));

    linker.update_links("x", "y", links_xy1);
    linker.update_links("y", "z", links_yz1);

    ConditionSet x2;
    x2.insert(condition11);
    x2.insert(condition12);
    x2.insert(condition13);

    ConditionSet y2;
    y2.insert(condition22);
    y2.insert(condition23);
    y2.insert(condition24);

    ConditionSet z2;
    z2.insert(condition33);
    z2.insert(condition34);
    z2.insert(condition35);

    EXPECT_EQ(linker.get_conditions("x"), x2);
    EXPECT_EQ(linker.get_conditions("y"), y2);
    EXPECT_EQ(linker.get_conditions("z"), z2);

    TupleSet tuples_xyz1;
    tuples_xyz1.insert(make_tuples(condition11, condition22, condition35));
    tuples_xyz1.insert(make_tuples(condition11, condition23, condition33));
    tuples_xyz1.insert(make_tuples(condition13, condition23, condition33));
    tuples_xyz1.insert(make_tuples(condition12, condition24, condition34));
    tuples_xyz1.insert(make_tuples(condition12, condition24, condition33));

    EXPECT_EQ(linker.make_tuples(make_string_list("x", "y", "z")), tuples_xyz1);

/*
    ConditionSet xz11_indirect;
    xz11_indirect.insert(condition33);
    xz11_indirect.insert(condition35);
    EXPECT_EQ(linker.get_indirect_links("x", "z", condition11), xz11_indirect);

    ConditionSet xz12_indirect;
    xz12_indirect.insert(condition33);
    xz12_indirect.insert(condition34);
    EXPECT_EQ(linker.get_indirect_links("x", "z", condition12), xz12_indirect);

    ConditionSet xz13_indirect;
    xz13_indirect.insert(condition33);
    EXPECT_EQ(linker.get_indirect_links("x", "z", condition13), xz13_indirect);

    ConditionSet zx33_indirect;
    zx33_indirect.insert(condition11);
    zx33_indirect.insert(condition12);
    zx33_indirect.insert(condition13);
    EXPECT_EQ(linker.get_indirect_links("z", "x", condition33), zx33_indirect);

    ConditionSet zx34_indirect;
    zx34_indirect.insert(condition12);
    EXPECT_EQ(linker.get_indirect_links("z", "x", condition34), zx34_indirect);

    ConditionSet zx35_indirect;
    zx35_indirect.insert(condition11);
    EXPECT_EQ(linker.get_indirect_links("z", "x", condition35), zx35_indirect);
*/

    TupleSet tuples_xz1;
    tuples_xz1.insert(make_tuples(condition11, condition35));
    tuples_xz1.insert(make_tuples(condition11, condition33));
    tuples_xz1.insert(make_tuples(condition13, condition33));
    tuples_xz1.insert(make_tuples(condition12, condition34));
    tuples_xz1.insert(make_tuples(condition12, condition33));
    
    EXPECT_EQ(linker.make_tuples(make_string_list("x", "z")), tuples_xz1);

    TupleSet tuples_yzx1;
    tuples_yzx1.insert(make_tuples(condition22, condition35, condition11));
    tuples_yzx1.insert(make_tuples(condition23, condition33, condition11));
    tuples_yzx1.insert(make_tuples(condition23, condition33, condition13));
    tuples_yzx1.insert(make_tuples(condition24, condition34, condition12));
    tuples_yzx1.insert(make_tuples(condition24, condition33, condition12));

    EXPECT_EQ(linker.make_tuples(make_string_list("y", "z", "x")), tuples_yzx1);

    TupleSet tuples_zxy1;
    tuples_zxy1.insert(make_tuples(condition35, condition11, condition22));
    tuples_zxy1.insert(make_tuples(condition33, condition11, condition23));
    tuples_zxy1.insert(make_tuples(condition33, condition13, condition23));
    tuples_zxy1.insert(make_tuples(condition34, condition12, condition24));
    tuples_zxy1.insert(make_tuples(condition33, condition12, condition24));
    
    EXPECT_EQ(linker.make_tuples(make_string_list("z", "x", "y")), tuples_zxy1);
}

TEST(LinkerTest, UpdateWithMoreLinks) {
    /*
     * First Insert:
     * <x, y> = <11, 21>, <12, 22>
     *
     * Then Insert:
     * <x, y> = <11, 22>, <12, 21>, <12, 22>
     * 
     * Result should be the intersection of the two
     * <x, y> = <12, 22>
     * 
     */

    SimpleAssignmentAst stat11(11);
    SimpleAssignmentAst stat12(12);

    SimpleAssignmentAst stat21(21);
    SimpleAssignmentAst stat22(22);

    ConditionPtr condition11(new SimpleStatementCondition(&stat11));
    ConditionPtr condition12(new SimpleStatementCondition(&stat12));

    ConditionPtr condition21(new SimpleStatementCondition(&stat21));
    ConditionPtr condition22(new SimpleStatementCondition(&stat22));

    SimpleQueryLinker linker;

    ConditionSet x1;
    x1.insert(condition11);
    x1.insert(condition12);

    ConditionSet y1;
    y1.insert(condition21);
    y1.insert(condition22);

    linker.init_qvar("x", x1);
    linker.init_qvar("y", y1);

    std::set<ConditionPair> links1;
    links1.insert(ConditionPair(condition11, condition21));
    links1.insert(ConditionPair(condition12, condition22));

    linker.update_links("x", "y", links1);

    std::set<ConditionPair> links2;
    links2.insert(ConditionPair(condition11, condition22));
    links2.insert(ConditionPair(condition12, condition21));
    links2.insert(ConditionPair(condition12, condition22));

    linker.update_links("x", "y", links2);

    ConditionSet expected_x;
    expected_x.insert(condition12);
    EXPECT_EQ(linker.get_conditions("x"), expected_x);

    ConditionSet expected_y;
    expected_y.insert(condition22);
    EXPECT_EQ(linker.get_conditions("y"), expected_y);

    TupleSet expected;
    expected.insert(make_tuples(condition12, condition22));
    
    EXPECT_EQ(linker.make_tuples(make_string_list("x", "y")), expected);
}

TEST(LinkerTest, TwoWayLinks) {
    /*
     * First Insert:
     * <x, y> = <11, 21>, <11, 22>, <12, 22>
     *
     * Then Insert:
     * <y, x> = <21, 11>, <22, 12>, <22, 13>
     * 
     * Result should be the intersection of the two
     * <x, y> = <11, 21>, <12, 22>
     * 
     */

    SimpleAssignmentAst stat11(11);
    SimpleAssignmentAst stat12(12);
    SimpleAssignmentAst stat13(13);

    SimpleAssignmentAst stat21(21);
    SimpleAssignmentAst stat22(22);

    ConditionPtr condition11(new SimpleStatementCondition(&stat11));
    ConditionPtr condition12(new SimpleStatementCondition(&stat12));
    ConditionPtr condition13(new SimpleStatementCondition(&stat13));

    ConditionPtr condition21(new SimpleStatementCondition(&stat21));
    ConditionPtr condition22(new SimpleStatementCondition(&stat22));

    SimpleQueryLinker linker;

    ConditionSet x1;
    x1.insert(condition11);
    x1.insert(condition12);
    x1.insert(condition13);

    ConditionSet y1;
    y1.insert(condition21);
    y1.insert(condition22);

    linker.init_qvar("x", x1);
    linker.init_qvar("y", y1);

    std::set<ConditionPair> links1;
    links1.insert(ConditionPair(condition11, condition21));
    links1.insert(ConditionPair(condition11, condition22));
    links1.insert(ConditionPair(condition12, condition22));

    linker.update_links("x", "y", links1);

    std::set<ConditionPair> links2;
    links2.insert(ConditionPair(condition21, condition11));
    links2.insert(ConditionPair(condition22, condition12));
    links2.insert(ConditionPair(condition22, condition13));

    linker.update_links("y", "x", links2);

    TupleSet expected1;
    expected1.insert(make_tuples(condition11, condition21));
    expected1.insert(make_tuples(condition12, condition22));
    
    EXPECT_EQ(linker.make_tuples(make_string_list("x", "y")), expected1);

    TupleSet expected2;
    expected2.insert(make_tuples(condition21, condition11));
    expected2.insert(make_tuples(condition22, condition12));
    
    EXPECT_EQ(linker.make_tuples(make_string_list("y", "x")), expected2);
}

TEST(LinkerTest, UnlinkedPermutations) {

}

TEST(LinkerTest, IndirectLinks) {

}


}
}
