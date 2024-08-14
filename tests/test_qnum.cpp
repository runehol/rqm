#include "rqm/qnum.h"

#include <gtest/gtest.h>
#include <iostream>
#include <rapidcheck/gtest.h>
#include <string>

TEST(RQM_QNUM, ZnumConstructor)
{
    rqm::znum nom(3);
    rqm::znum denom(4);
    rqm::qnum r(nom, denom);
    EXPECT_EQ(r.nom(), nom);
    EXPECT_EQ(r.denom(), denom);
}

TEST(RQM_QNUM, IntPairConstructor)
{
    rqm::qnum r(3, 4);
    EXPECT_EQ(r.nom(), rqm::znum(3));
    EXPECT_EQ(r.denom(), rqm::znum(4));
}

TEST(RQM_QNUM, IntPairConstructor_with_simplification)
{
    rqm::qnum r(12, -20);
    EXPECT_EQ(r.nom(), rqm::znum(-3));
    EXPECT_EQ(r.denom(), rqm::znum(5));
}

TEST(RQM_QNUM, from_double)
{
    EXPECT_EQ(rqm::qnum::from_double(0.75), rqm::qnum(3, 4));
    EXPECT_EQ(rqm::qnum::from_double(1337), rqm::qnum(1337, 1));
    EXPECT_EQ(rqm::qnum::from_double(15.5), rqm::qnum(31, 2));
    EXPECT_EQ(rqm::qnum::from_double(0x1p100), rqm::qnum(rqm::znum::one() << 100));
    EXPECT_THROW(rqm::qnum::from_double(0.0 / 0.0), std::out_of_range);
    EXPECT_THROW(rqm::qnum::from_double(1e300 * 1e300), std::out_of_range);
}

TEST(RQM_QNUM, to_double)
{
    EXPECT_EQ(rqm::to_double(rqm::qnum(3, 4)), 0.75);
    EXPECT_EQ(rqm::to_double(rqm::qnum(1337, 1)), 1337.0);
    EXPECT_EQ(rqm::to_double(rqm::qnum(31, 2)), 15.5);
    EXPECT_EQ(rqm::to_double(rqm::qnum(rqm::znum::one() << 100)), 0x1p100);
}

RC_GTEST_PROP(RQM_QNUM, roundtrip_double, (double v))
{
    RC_PRE(!std::isinf(v) && !std::isnan(v));
    rqm::qnum a = rqm::qnum::from_double(v);
    double v2 = rqm::to_double(a);
    RC_ASSERT(v2 == v);
}

TEST(RQM_QNUM, divide_by_zero)
{
    EXPECT_THROW(rqm::qnum(4, 0), std::out_of_range);
}

TEST(RQM_QNUM, FromString)
{
    rqm::qnum r = rqm::qnum::from_string("-3/4");
    EXPECT_EQ(r.nom(), rqm::znum(-3));
    EXPECT_EQ(r.denom(), rqm::znum(4));
}

TEST(RQM_QNUM, FromString2)
{
    rqm::qnum r = rqm::qnum::from_string("-3");
    EXPECT_EQ(r.nom(), rqm::znum(-3));
    EXPECT_EQ(r.denom(), rqm::znum(1));
}

TEST(RQM_QNUM, Abs)
{
    rqm::qnum r(-3, 4);
    rqm::qnum abs_r = abs(r);
    EXPECT_EQ(abs_r.nom(), rqm::znum(3));
    EXPECT_EQ(abs_r.denom(), rqm::znum(4));
}

TEST(RQM_QNUM, Equality)
{
    rqm::qnum r1(3, 4);
    rqm::qnum r2(3, 4);
    EXPECT_TRUE(r1 == r2);
}

TEST(RQM_QNUM, Inequality)
{
    rqm::qnum r1(3, 4);
    rqm::qnum r2(2, 4);
    EXPECT_TRUE(r1 != r2);
}

TEST(RQM_QNUM, LessThan)
{
    rqm::qnum r1(1, 4);
    rqm::qnum r2(3, 4);
    EXPECT_TRUE(r1 < r2);
}

TEST(RQM_QNUM, LessThanOrEqual)
{
    rqm::qnum r1(3, 4);
    rqm::qnum r2(3, 4);
    EXPECT_TRUE(r1 <= r2);
}

TEST(RQM_QNUM, GreaterThan)
{
    rqm::qnum r1(3, 4);
    rqm::qnum r2(1, 4);
    EXPECT_TRUE(r1 > r2);
}

TEST(RQM_QNUM, GreaterThanOrEqual)
{
    rqm::qnum r1(3, 4);
    rqm::qnum r2(3, 4);
    EXPECT_TRUE(r1 >= r2);
}

TEST(RQM_QNUM, Addition)
{
    rqm::qnum r1(1, 4);
    rqm::qnum r2(1, 4);
    rqm::qnum result = r1 + r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_QNUM, Subtraction)
{
    rqm::qnum r1(3, 4);
    rqm::qnum r2(1, 4);
    rqm::qnum result = r1 - r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_QNUM, Multiplication)
{
    rqm::qnum r1(1, 4);
    rqm::qnum r2(1, 4);
    rqm::qnum result = r1 * r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(16));
}

TEST(RQM_QNUM, MultiplicationInt)
{
    rqm::qnum r(1, 4);
    rqm::qnum result = r * 2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_QNUM, MultiplicationIntQnum)
{
    rqm::qnum r(1, 4);
    rqm::qnum result = 2 * r;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

TEST(RQM_QNUM, DivisionInt)
{
    rqm::qnum r(1, 4);
    rqm::qnum result = r / 2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(8));
}

TEST(RQM_QNUM, DivisionQnum)
{
    rqm::qnum r1(1, 4);
    rqm::qnum r2(1, 2);
    rqm::qnum result = r1 / r2;
    EXPECT_EQ(result.nom(), rqm::znum(1));
    EXPECT_EQ(result.denom(), rqm::znum(2));
}

RC_GTEST_PROP(RQM_QNUM, pre_increment, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::qnum a = rqm::qnum(in, id);
    ++a;
    RC_ASSERT(a == rqm::qnum(in, id) + 1);
}

RC_GTEST_PROP(RQM_QNUM, pre_decrement, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::qnum a = rqm::qnum(in, id);
    --a;
    RC_ASSERT(a == rqm::qnum(in, id) - 1);
}

RC_GTEST_PROP(RQM_QNUM, post_increment, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::qnum a = rqm::qnum(in, id);
    rqm::qnum b = a++;
    RC_ASSERT(a == rqm::qnum(in, id) + 1);
    RC_ASSERT(b == rqm::qnum(in, id));
}
RC_GTEST_PROP(RQM_QNUM, post_decrement, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::qnum a = rqm::qnum(in, id);
    rqm::qnum b = a--;
    RC_ASSERT(a == rqm::qnum(in, id) - 1);
    RC_ASSERT(b == rqm::qnum(in, id));
}

RC_GTEST_PROP(RQM_QNUM, operator_not, (int64_t in, uint32_t id))
{
    RC_PRE(id > uint32_t(0));
    rqm::qnum r(in, id);
    RC_ASSERT(!r == !in);
}

TEST(RQM_QNUM, ToString)
{
    rqm::qnum r(1, 4);
    std::string result = to_string(r);
    EXPECT_EQ(result, "1/4");
}

TEST(RQM_QNUM, StreamOutput)
{
    rqm::qnum r(1, 4);
    std::ostringstream os;
    os << r;
    EXPECT_EQ(os.str(), "1/4");
}
