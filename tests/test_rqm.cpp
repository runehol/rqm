#include "rqm/rqm.h"

#include <gtest/gtest.h>
#include <iostream>
#include <rapidcheck/gtest.h>
#include <string>

TEST(RQM, instantiation)
{
    rqm::num one = 1;
    EXPECT_EQ(one.to_int64_t(), 1);
}

TEST(RQM, simple_add)
{
    int64_t ia = 1;
    int64_t ib = -4294967296;
    rqm::num a = ia;
    rqm::num b = ib;
    rqm::num c = a + b;
    int64_t ic = ia + ib;
    rqm::num cexp = ic;
    EXPECT_EQ(c.to_int64_t(), ic);
    EXPECT_EQ(c, cexp);
}

RC_GTEST_PROP(RQM, comparison, (int64_t ia, int64_t ib))
{
    rqm::num a = ia;
    rqm::num b = ib;
    RC_ASSERT((a == b) == (ia == ib));
    RC_ASSERT((a != b) == (ia != ib));
    RC_ASSERT((a < b) == (ia < ib));
    RC_ASSERT((a <= b) == (ia <= ib));
    RC_ASSERT((a > b) == (ia > ib));
    RC_ASSERT((a >= b) == (ia >= ib));
}

RC_GTEST_PROP(RQM, negate, (int64_t ia))
{
    rqm::num a = ia;
    RC_ASSERT(-a == -ia);
}

RC_GTEST_PROP(RQM, abs, (int64_t ia))
{
    rqm::num a = ia;
    RC_ASSERT(rqm::abs(a) == std::abs(ia));
}

RC_GTEST_PROP(RQM, add, (int64_t ia, int64_t ib))
{
    int64_t tmp;
    RC_PRE(!__builtin_saddll_overflow(ia, ib, &tmp));
    rqm::num a = ia;
    rqm::num b = ib;
    rqm::num c = a + b;
    RC_ASSERT(c == (ia + ib));
}

RC_GTEST_PROP(RQM, sub, (int64_t ia, int64_t ib))
{
    int64_t tmp;
    RC_PRE(!__builtin_ssubll_overflow(ia, ib, &tmp));
    rqm::num a = ia;
    rqm::num b = ib;
    rqm::num c = a - b;
    RC_ASSERT(c == (ia - ib));
}

RC_GTEST_PROP(RQM, mul_small, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::num a = ia;
    rqm::num b = ib;
    rqm::num c = a * b;
    RC_ASSERT(c == (ia * ib));
}

RC_GTEST_PROP(RQM, mul_larger, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::num a = ia;
    rqm::num b = ib;

    rqm::num c = a * b;

    rqm::num scaled_a = a * 10000000;
    rqm::num scaled_b = b * 10000000;

    rqm::num scaled_c = c * 10000000 * 10000000;

    RC_ASSERT(scaled_a * scaled_b == scaled_c);
}

RC_GTEST_PROP(RQM, mul_num_with_digit, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::num a = ia;
    rqm::num c = a * ib;
    RC_ASSERT(c == (ia * ib));
}

RC_GTEST_PROP(RQM, mul_digit_with_num, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::num b = ib;
    rqm::num c = ia * b;
    RC_ASSERT(c == (ia * ib));
}
