#include "rqm/rqm.h"

#include <gtest/gtest.h>
#include <iostream>
#include <rapidcheck/gtest.h>
#include <string>

TEST(RQM_ZNUM, instantiation)
{
    rqm::znum one = 1;
    EXPECT_EQ(one.to_int64_t(), 1);
}

TEST(RQM_ZNUM, simple_add)
{
    int64_t ia = 1;
    int64_t ib = -4294967296;
    rqm::znum a = ia;
    rqm::znum b = ib;
    rqm::znum c = a + b;
    int64_t ic = ia + ib;
    rqm::znum cexp = ic;
    EXPECT_EQ(c.to_int64_t(), ic);
    EXPECT_EQ(c, cexp);
}

TEST(RQM_ZNUM, simple_shift_right)
{
    int64_t ia = -2;
    int64_t ib = 1;
    rqm::znum a = ia;
    rqm::znum c = a >> ib;
    int64_t ic = ia >> ib;
    rqm::znum cexp = ic;
    EXPECT_EQ(c.to_int64_t(), ic);
    EXPECT_EQ(c, cexp);
}

TEST(RQM_ZNUM, simple_shift_right2)
{
    int64_t ia = 4294967296;
    int64_t ib = 0;
    rqm::znum a = ia;
    rqm::znum c = a >> ib;
    int64_t ic = ia >> ib;
    rqm::znum cexp = ic;
    EXPECT_EQ(c.to_int64_t(), ic);
    EXPECT_EQ(c, cexp);
}

TEST(RQM_ZNUM, simple_to_string)
{
    int64_t ia = -1113852700;
    rqm::znum a = ia;
    std::string v = rqm::to_string(a);
    std::string exp = "-1113852700";
    EXPECT_EQ(v, exp);
}

TEST(RQM_ZNUM, simple_from_string)
{
    std::string v = "-1113852700";
    int64_t exp = std::stod(v);
    rqm::znum a = rqm::znum::from_string(v);

    EXPECT_EQ(a, exp);
}

RC_GTEST_PROP(RQM_ZNUM, comparison, (int64_t ia, int64_t ib))
{
    rqm::znum a = ia;
    rqm::znum b = ib;
    RC_ASSERT((a == b) == (ia == ib));
    RC_ASSERT((a != b) == (ia != ib));
    RC_ASSERT((a < b) == (ia < ib));
    RC_ASSERT((a <= b) == (ia <= ib));
    RC_ASSERT((a > b) == (ia > ib));
    RC_ASSERT((a >= b) == (ia >= ib));
}

RC_GTEST_PROP(RQM_ZNUM, negate, (int64_t ia))
{
    RC_PRE(ia != std::numeric_limits<int64_t>::min());
    rqm::znum a = ia;
    RC_ASSERT(-a == -ia);
}

RC_GTEST_PROP(RQM_ZNUM, abs, (int64_t ia))
{
    RC_PRE(ia != std::numeric_limits<int64_t>::min());
    rqm::znum a = ia;
    RC_ASSERT(rqm::abs(a) == std::abs(ia));
}

RC_GTEST_PROP(RQM_ZNUM, operator_not, (int64_t ia))
{
    rqm::znum a = ia;
    RC_ASSERT(!a == !ia);
}

RC_GTEST_PROP(RQM_ZNUM, add, (int64_t ia, int64_t ib))
{
    int64_t tmp;
    RC_PRE(!__builtin_saddll_overflow(ia, ib, &tmp));
    rqm::znum a = ia;
    rqm::znum b = ib;
    rqm::znum c = a + b;
    RC_ASSERT(c == (ia + ib));
}

RC_GTEST_PROP(RQM_ZNUM, sub, (int64_t ia, int64_t ib))
{
    int64_t tmp;
    RC_PRE(!__builtin_ssubll_overflow(ia, ib, &tmp));
    rqm::znum a = ia;
    rqm::znum b = ib;
    rqm::znum c = a - b;
    RC_ASSERT(c == (ia - ib));
}

RC_GTEST_PROP(RQM_ZNUM, mul_small, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::znum a = ia;
    rqm::znum b = ib;
    rqm::znum c = a * b;
    RC_ASSERT(c == (ia * ib));
}

RC_GTEST_PROP(RQM_ZNUM, mul_larger, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::znum a = ia;
    rqm::znum b = ib;

    rqm::znum c = a * b;

    rqm::znum scaled_a = a * 10000000;
    rqm::znum scaled_b = b * 10000000;

    rqm::znum scaled_c = c * 10000000 * 10000000;

    RC_ASSERT(scaled_a * scaled_b == scaled_c);
}

RC_GTEST_PROP(RQM_ZNUM, mul_znum_with_digit, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::znum a = ia;
    rqm::znum c = a * ib;
    RC_ASSERT(c == (ia * ib));
}

RC_GTEST_PROP(RQM_ZNUM, mul_digit_with_znum, (int32_t ia32, int32_t ib32))
{
    int64_t ia = ia32;
    int64_t ib = ib32;
    rqm::znum b = ib;
    rqm::znum c = ia * b;
    RC_ASSERT(c == (ia * ib));
}

RC_GTEST_PROP(RQM_ZNUM, divide_znum_with_digit, (int64_t ia, int32_t ib32))
{
    RC_PRE(ib32 != 0);
    int64_t ib = ib32;
    rqm::znum a = ia;
    rqm::znum quotient = a / ib;
    RC_ASSERT(quotient == (ia / ib));
}

RC_GTEST_PROP(RQM_ZNUM, modulo_znum_with_digit, (int64_t ia, int32_t ib32))
{
    RC_PRE(ib32 != 0);
    int64_t ib = ib32;
    rqm::znum a = ia;
    rqm::znum quotient = a % ib;
    RC_ASSERT(quotient == (ia % ib));
}

RC_GTEST_PROP(RQM_ZNUM, divide_with_digit_by_itself, (int32_t ia))
{
    RC_PRE(ia != 0);
    rqm::znum a = ia;
    rqm::znum quotient = a / ia;
    RC_ASSERT(quotient == 1);
}

RC_GTEST_PROP(RQM_ZNUM, divide, (int64_t ia, int64_t ib))
{
    RC_PRE(ib != 0);
    RC_PRE(!(ib == -1 && ia == std::numeric_limits<int64_t>::min()));
    rqm::znum a = ia;
    rqm::znum b = ib;
    rqm::znum quotient = a / b;
    RC_ASSERT(quotient == (ia / ib));
}

RC_GTEST_PROP(RQM_ZNUM, modulo, (int64_t ia, int64_t ib))
{
    RC_PRE(ib != 0);
    RC_PRE(!(ib == -1 && ia == std::numeric_limits<int64_t>::min()));
    rqm::znum a = ia;
    rqm::znum b = ib;
    rqm::znum quotient = a % b;
    RC_ASSERT(quotient == (ia % ib));
}

RC_GTEST_PROP(RQM_ZNUM, divide_by_itself, (int64_t ia))
{
    RC_PRE(ia != 0);
    rqm::znum a = ia;
    rqm::znum quotient = a / a;
    RC_ASSERT(quotient == 1);
}

RC_GTEST_PROP(RQM_ZNUM, multiply_divide, (int64_t ia, int64_t ib, int64_t ic, int64_t id))
{
    RC_PRE(ic != 0);
    RC_PRE(id != 0);
    rqm::znum exp = rqm::znum(ia) * rqm::znum(ib);
    rqm::znum divisor = rqm::znum(ic) * rqm::znum(id);
    rqm::znum dividend = exp * divisor;
    rqm::znum quotient = dividend / divisor;

    RC_ASSERT(quotient == exp);
}

RC_GTEST_PROP(RQM_ZNUM, to_string, (int64_t ia))
{
    rqm::znum a = ia;
    std::string as = rqm::to_string(a);
    std::string ias = std::to_string(ia);
    RC_ASSERT(as == ias);
}

RC_GTEST_PROP(RQM_ZNUM, from_string, (int64_t ia))
{
    std::string ias = std::to_string(ia);
    rqm::znum a = rqm::znum::from_string(ias);
    RC_ASSERT(a == ia);
}

TEST(RQM_ZNUM, from_string_edge_cases)
{
    EXPECT_THROW(rqm::znum::from_string(""), std::invalid_argument);
    EXPECT_THROW(rqm::znum::from_string("-"), std::invalid_argument);
    EXPECT_THROW(rqm::znum::from_string("%%#$%&/()"), std::invalid_argument);
    EXPECT_THROW(rqm::znum::from_string("4123*"), std::invalid_argument);
}

RC_GTEST_PROP(RQM_ZNUM, repeated_doubling, (uint8_t n_times))
{
    rqm::znum v = 2;
    uint64_t n_bits = 1;
    for(uint32_t i = 0; i < n_times; ++i)
    {
        v = v * 2;
        ++n_bits;
    }
    uint32_t expected_n_digits = 1 + n_bits / rqm::n_bits_in_digit;

    RC_ASSERT(v.get_n_digits() == expected_n_digits);
}

RC_GTEST_PROP(RQM_ZNUM, repeated_squaring, (uint8_t n_times))
{
    RC_PRE(n_times < 16);
    rqm::znum v = 2;
    uint64_t n_bits = 1;
    for(uint32_t i = 0; i < n_times; ++i)
    {
        v = v * v;
        n_bits *= 2;
    }
    uint32_t expected_n_digits = 1 + n_bits / rqm::n_bits_in_digit;

    RC_ASSERT(v.get_n_digits() == expected_n_digits);
}

RC_GTEST_PROP(RQM_ZNUM, shift_left, (int32_t ia, uint8_t ib))
{
    RC_PRE(ib <= 32);
    rqm::znum a = ia;
    rqm::znum result = a << ib;
    RC_ASSERT(result == (int64_t(ia) << ib));
}

RC_GTEST_PROP(RQM_ZNUM, shift_right, (int64_t ia, uint32_t ib))
{
    rqm::znum a = ia;
    rqm::znum result = a >> ib;
    int64_t expected_result = ia < 0 ? -1 : 0;
    if(ib < 64)
    {
        expected_result = ia >> ib;
    }
    RC_ASSERT(result == expected_result);
}

RC_GTEST_PROP(RQM_ZNUM, shift_left_right_returns_same, (int64_t ia, uint16_t ib))
{
    rqm::znum a = ia;
    rqm::znum a2 = a << ib;
    rqm::znum a3 = a2 >> ib;
    RC_ASSERT(a == a3);
}

RC_GTEST_PROP(RQM_ZNUM, pre_increment, (int64_t ia))
{
    rqm::znum a = ia;
    ++a;
    RC_ASSERT(a == rqm::znum(ia) + 1);
}

RC_GTEST_PROP(RQM_ZNUM, pre_decrement, (int64_t ia))
{
    rqm::znum a = ia;
    --a;
    RC_ASSERT(a == rqm::znum(ia) - 1);
}

RC_GTEST_PROP(RQM_ZNUM, post_increment, (int64_t ia))
{
    rqm::znum a = ia;
    rqm::znum b = a++;
    RC_ASSERT(a == rqm::znum(ia) + 1);
    RC_ASSERT(b == ia);
}
RC_GTEST_PROP(RQM_ZNUM, post_decrement, (int64_t ia))
{
    rqm::znum a = ia;
    rqm::znum b = a--;
    RC_ASSERT(a == rqm::znum(ia) - 1);
    RC_ASSERT(b == ia);
}

template<typename T>
T euclidean_gcd(T a, T b)
{
    a = std::abs(a);
    b = std::abs(b);
    while(b != 0)
    {
        T t = b;
        b = a % b;
        a = t;
    }
    return a;
}

RC_GTEST_PROP(RQM_ZNUM, gcd, (int64_t ia, int64_t ib))
{
    rqm::znum a = ia;
    rqm::znum b = ib;
    rqm::znum c = rqm::gcd(ia, ib);
    uint32_t exp = euclidean_gcd(ia, ib);
    RC_ASSERT(c == exp);
}
