#ifndef RQM_BASIC_ARITHMETIC_H
#define RQM_BASIC_ARITHMETIC_H

#include "numview.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rqm
{
    numview static inline copy_view(numview dest, const numview src)
    {
        dest.signum = src.signum;
        dest.n_digits = src.n_digits;
        memcpy(dest.digits, src.digits, src.n_digits * sizeof(src.digits[0]));
        return dest;
    }

    template<typename T>
    [[nodiscard]] constexpr signum_t compare_signum(T a, T b)
    {
        if(a < b) return -1;
        if(a > b) return 1;
        return 0;
    }

    template<typename T>
    [[nodiscard]] constexpr signum_t internal_compare_unequal(T a, T b)
    {
        return a > b ? 1 : -1;
    }

    [[nodiscard]] signum_t compare(const numview a, const numview b);

    [[nodiscard]] constexpr static inline numview negate(numview a)
    {
        a.signum = -a.signum;
        return a;
    }

    [[nodiscard]] constexpr static inline numview abs(numview a)
    {
        a.signum = std::abs(a.signum);
        return a;
    }

    [[nodiscard]] constexpr static inline numview with_signum(signum_t signum, numview a)
    {
        a.signum = signum;
        return a;
    }

    [[nodiscard]] constexpr static inline numview with_sign_unless_zero(signum_t signum, numview a)
    {
        a.signum = signum;
        if(a.n_digits == 0) a.signum = 0;
        return a;
    }

    [[nodiscard]] constexpr static inline numview zero_out(numview a)
    {
        a.signum = 0;
        a.n_digits = 0;
        return a;
    }

    [[nodiscard]] constexpr static inline numview remove_high_zeros(numview c)
    {
        // adjust down after cancellation
        while(c.n_digits > 0 && c.digits[c.n_digits - 1] == 0)
        {
            --c.n_digits;
        }
        return c;
    }

    // subtract b from a, assuming a is larger than b. this function ignores the signs in the view
    [[nodiscard]] numview abs_subtract_a_larger_than_b(numview c, const numview a, const numview b);

    [[nodiscard]] numview add(numview c, numview a, numview b);

    [[nodiscard]] constexpr static inline uint32_t add_digit_estimate(uint32_t a_digits, uint32_t b_digits)
    {
        return std::max(a_digits, b_digits) + 1;
    }

    [[nodiscard]] constexpr static inline uint32_t multiply_digit_estimate(uint32_t a_digits, uint32_t b_digits)
    {
        return a_digits + b_digits;
    }

    [[nodiscard]] static inline numview zero_with_n_digits(numview c, uint32_t n_digits)
    {
        // set it all to zero
        c.n_digits = n_digits;
        memset(c.digits, 0, n_digits * sizeof(c.digits[0]));
        return c;
    }

    [[nodiscard]] numview multiply(numview c, const numview a, const numview b);

    [[nodiscard]] numview multiply_with_single_digit(numview c, const numview a, digit_t b);

    [[nodiscard]] constexpr static inline uint32_t quotient_digit_estimate(uint32_t dividend_digits, uint32_t divisor_digits)
    {
        return std::max<int64_t>(0, int64_t(dividend_digits) - int64_t(divisor_digits) + 1);
    }

    [[nodiscard]] constexpr static inline uint32_t modulo_digit_estimate(uint32_t dividend_digits, uint32_t divisor_digits)
    {
        return divisor_digits;
    }

    [[nodiscard]] numview abs_divmod_by_single_digit(numview quotient, digit_t *remainder_ptr, const numview dividend, const digit_t divisor32);

    [[nodiscard]] numview divmod_by_single_digit(numview quotient, int64_t *modulo, const numview dividend, const digit_t divisor);

    [[nodiscard]] numview divmod(numview quotient, numview *remainder, const numview dividend, const numview divisor);

    template<typename T>
    constexpr T cdiv(T a, T b)
    {
        return (a + b - 1) / b;
    }

    [[nodiscard]] constexpr static inline uint32_t shift_left_digit_estimate(uint32_t a_digits, uint32_t left_shift_amount)
    {
        return a_digits + cdiv<uint64_t>(left_shift_amount, n_bits_in_digit);
    }

    [[nodiscard]] constexpr static inline uint32_t shift_right_digit_estimate(uint32_t a_digits, uint32_t right_shift_amount)
    {
        // make sure we always have one digit present, just in case.
        return std::max<int64_t>(1, a_digits - right_shift_amount / n_bits_in_digit);
    }

    [[nodiscard]] numview shift_left(numview c, const numview a, uint32_t shift_amount);

    [[nodiscard]] numview shift_right(numview c, const numview a, uint32_t shift_amount);

    uint32_t countr_zero(const numview v);

    [[nodiscard]] constexpr static inline uint32_t gcd_digit_estimate(uint32_t a_digits, uint32_t b_digits)
    {
        return std::min(a_digits, b_digits);
    }

    [[nodiscard]] numview binary_gcd(numview c, numview a, numview b);

} // namespace rqm

#endif // RQM_BASIC_ARITHMETIC_H
