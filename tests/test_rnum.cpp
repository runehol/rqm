#include "rqm/rnum.h"

#include <gtest/gtest.h>
#include <iostream>
#include <rapidcheck/gtest.h>
#include <string>

TEST(RQM_RNUM, ZnumConstructor)
{
    rqm::znum nom(3);
    rqm::znum denom(4);
    rqm::rnum r(nom, denom);
    EXPECT_EQ(r.nom(), nom);
    EXPECT_EQ(r.denom(), denom);
}

TEST(RQM_RNUM, IntPairConstructor)
{
    rqm::rnum r(3, 4);
    EXPECT_EQ(r.nom(), rqm::znum(3));
    EXPECT_EQ(r.denom(), rqm::znum(4));
}

TEST(RQM_RNUM, IntPairConstructor_with_simplification)
{
    rqm::rnum r(12, -20);
    EXPECT_EQ(r.nom(), rqm::znum(-3));
    EXPECT_EQ(r.denom(), rqm::znum(5));
}

TEST(RQM_RNUM, divide_by_zero)
{
    EXPECT_THROW(rqm::rnum(4, 0), std::out_of_range);
}

/*
TEST(RQM_RNUM, FromString)
{
    rqm::rnum r = rqm::rnum::from_string("3/4");
    EXPECT_EQ(r.nom(), rqm::znum(3));
    EXPECT_EQ(r.denom(), rqm::znum(4));
}
*/

TEST(RQM_RNUM, Abs)
{
    rqm::rnum r(-3, 4);
    rqm::rnum abs_r = abs(r);
    EXPECT_EQ(abs_r.nom(), rqm::znum(3));
    EXPECT_EQ(abs_r.denom(), rqm::znum(4));
}

TEST(RQM_RNUM, Equality)
{
    rqm::rnum r1(3, 4);
    rqm::rnum r2(3, 4);
    EXPECT_TRUE(r1 == r2);
}

TEST(RQM_RNUM, Inequality)
{
    rqm::rnum r1(3, 4);
    rqm::rnum r2(2, 4);
    EXPECT_TRUE(r1 != r2);
}

TEST(RQM_RNUM, LessThan)
{
    rqm::rnum r1(1, 4);
    rqm::rnum r2(3, 4);
    EXPECT_TRUE(r1 < r2);
}

TEST(RQM_RNUM, LessThanOrEqual)
{
    rqm::rnum r1(3, 4);
    rqm::rnum r2(3, 4);
    EXPECT_TRUE(r1 <= r2);
}

TEST(RQM_RNUM, GreaterThan)
{
    rqm::rnum r1(3, 4);
    rqm::rnum r2(1, 4);
    EXPECT_TRUE(r1 > r2);
}

TEST(RQM_RNUM, GreaterThanOrEqual)
{
    rqm::rnum r1(3, 4);
    rqm::rnum r2(3, 4);
    EXPECT_TRUE(r1 >= r2);
}

TEST(RQM_RNUM, Addition)
{
    rqm::rnum r1(1, 4);
    rqm::rnum r2(1, 4);
    rqm::rnum result = r1 + r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_RNUM, Subtraction)
{
    rqm::rnum r1(3, 4);
    rqm::rnum r2(1, 4);
    rqm::rnum result = r1 - r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_RNUM, Multiplication)
{
    rqm::rnum r1(1, 4);
    rqm::rnum r2(1, 4);
    rqm::rnum result = r1 * r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(16));
}

TEST(RQM_RNUM, MultiplicationInt)
{
    rqm::rnum r(1, 4);
    rqm::rnum result = r * 2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_RNUM, MultiplicationIntRnum)
{
    rqm::rnum r(1, 4);
    rqm::rnum result = 2 * r;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_RNUM, DivisionInt)
{
    rqm::rnum r(1, 4);
    rqm::rnum result = r / 2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(8));
}

TEST(RQM_RNUM, DivisionRnum)
{
    rqm::rnum r1(1, 4);
    rqm::rnum r2(1, 2);
    rqm::rnum result = r1 / r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

RC_GTEST_PROP(RQM_RNUM, pre_increment, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::rnum a = rqm::rnum(in, id);
    ++a;
    RC_ASSERT(a == rqm::rnum(in, id) + 1);
}

RC_GTEST_PROP(RQM_RNUM, pre_decrement, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::rnum a = rqm::rnum(in, id);
    --a;
    RC_ASSERT(a == rqm::rnum(in, id) - 1);
}

RC_GTEST_PROP(RQM_RNUM, post_increment, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::rnum a = rqm::rnum(in, id);
    rqm::rnum b = a++;
    RC_ASSERT(a == rqm::rnum(in, id) + 1);
    RC_ASSERT(b == rqm::rnum(in, id));
}
RC_GTEST_PROP(RQM_RNUM, post_decrement, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::rnum a = rqm::rnum(in, id);
    rqm::rnum b = a--;
    RC_ASSERT(a == rqm::rnum(in, id) - 1);
    RC_ASSERT(b == rqm::rnum(in, id));
}

RC_GTEST_PROP(RQM_RNUM, operator_not, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::rnum r(in, id);
    RC_ASSERT(!r == !in);
}

TEST(RQM_RNUM, ToString)
{
    rqm::rnum r(1, 4);
    std::string result = to_string(r);
    EXPECT_EQ(result, "1/4");
}

TEST(RQM_RNUM, StreamOutput)
{
    rqm::rnum r(1, 4);
    std::ostringstream os;
    os << r;
    EXPECT_EQ(os.str(), "1/4");
}
