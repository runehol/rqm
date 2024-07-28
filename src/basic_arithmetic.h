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
    [[nodiscard]] signum_t compare_signum(T a, T b)
    {
        if(a < b) return -1;
        if(a > b) return 1;
        return 0;
    }

    template<typename T>
    [[nodiscard]] signum_t internal_compare_unequal(T a, T b)
    {
        return a > b ? 1 : -1;
    }

    [[nodiscard]] signum_t compare(const numview a, const numview b);

    [[nodiscard]] static inline numview negate(numview a)
    {
        a.signum = -a.signum;
        return a;
    }

    [[nodiscard]] static inline numview abs(numview a)
    {
        a.signum = std::abs(a.signum);
        return a;
    }

    [[nodiscard]] static inline numview with_signum(signum_t signum, numview a)
    {
        a.signum = signum;
        return a;
    }

    [[nodiscard]] static inline numview with_sign_unless_zero(signum_t signum, numview a)
    {
        a.signum = signum;
        if(a.n_digits == 0) a.signum = 0;
        return a;
    }

    [[nodiscard]] static inline numview zero_out(numview a)
    {
        a.signum = 0;
        a.n_digits = 0;
        return a;
    }

    [[nodiscard]] static inline numview remove_high_zeros(numview c)
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

    [[nodiscard]] static inline numview add_always_into_destination(numview c, numview a, numview b)
    {
        if(a.signum == 0) return copy_view(c, b);
        if(b.signum == 0) return copy_view(c, a);
        return add(c, a, b);
    }

    [[nodiscard]] static inline uint32_t add_digit_estimate(uint32_t a_digits, uint32_t b_digits)
    {
        return std::max(a_digits, b_digits) + 1;
    }

    [[nodiscard]] static inline uint32_t multiply_digit_estimate(uint32_t a_digits, uint32_t b_digits)
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

    [[nodiscard]] static inline uint32_t quotient_digit_estimate(uint32_t dividend_digits)
    {
        return dividend_digits;
    }

    [[nodiscard]] numview abs_divmod_by_single_digit(numview quotient, digit_t *remainder_ptr, const numview dividend, const digit_t divisor32);

    [[nodiscard]] numview divide_by_single_digit(numview quotient, const numview dividend, const digit_t divisor);

} // namespace rqm

#endif // RQM_BASIC_ARITHMETIC_H
