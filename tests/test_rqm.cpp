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

TEST(RQM, simple_to_string)
{
    int64_t ia = -1113852700;
    rqm::num a = ia;
    std::string v = rqm::to_string(a);
    std::string exp = "-1113852700";
    EXPECT_EQ(v, exp);
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

RC_GTEST_PROP(RQM, divide_num_with_digit, (int64_t ia, int32_t ib32))
{
    RC_PRE(ib32 != 0);
    int64_t ib = ib32;
    rqm::num a = ia;
    rqm::num quotient = a / ib;
    RC_ASSERT(quotient == (ia / ib));
}

RC_GTEST_PROP(RQM, divide_by_itself, (int32_t ia))
{
    RC_PRE(ia != 0);
    rqm::num a = ia;
    rqm::num quotient = a / ia;
    RC_ASSERT(quotient == 1);
}

RC_GTEST_PROP(RQM, to_string, (int64_t ia))
{
    rqm::num a = ia;
    std::string as = rqm::to_string(a);
    std::string ias = std::to_string(ia);
    RC_ASSERT(as == ias);
}

RC_GTEST_PROP(RQM, repeated_doubling, (uint16_t n_times))
{
    RC_PRE(n_times < 1024); // let's stay within 1024 bits
    rqm::num v = 2;
    uint64_t n_bits = 1;
    for(uint32_t i = 0; i < n_times; ++i)
    {
        v = v * 2;
        ++n_bits;
    }
    uint32_t expected_n_digits = 1 + n_bits / rqm::detail::n_digit_bits;

    RC_ASSERT(v.get_n_digits() == expected_n_digits);
}

RC_GTEST_PROP(RQM, repeated_squaring, (uint8_t n_times))
{
    RC_PRE(n_times < 16);
    rqm::num v = 2;
    uint64_t n_bits = 1;
    for(uint32_t i = 0; i < n_times; ++i)
    {
        v = v * v;
        n_bits *= 2;
    }
    uint32_t expected_n_digits = 1 + n_bits / rqm::detail::n_digit_bits;

    RC_ASSERT(v.get_n_digits() == expected_n_digits);
}
