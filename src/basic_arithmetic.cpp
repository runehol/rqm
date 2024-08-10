
#include "basic_arithmetic.h"
#include <algorithm>
#include <cassert>
#include <cstdint>

namespace rqm
{

    static constexpr digit_t countl_zero(digit_t x)
    {
        return __builtin_clz(x);
    }

    static constexpr digit_t countr_zero(digit_t x)
    {
        return __builtin_ctz(x);
    }

    // compare a and b, assuming both are positive. this function ignores the signs in the view
    [[nodiscard]] static signum_t abs_compare(const numview a, const numview b)
    {
        if(a.n_digits != b.n_digits)
        {
            return internal_compare_unequal(a.n_digits, b.n_digits);
        }
        for(int32_t idx = a.n_digits - 1; idx >= 0; --idx)
        {
            digit_t ad = a.digits[idx], bd = b.digits[idx];
            if(ad != bd) return internal_compare_unequal(ad, bd);
        }
        return 0;
    }

    [[nodiscard]] signum_t compare(const numview a, const numview b)
    {
        // if not the same size, return the difference of the signs
        if(a.signum != b.signum) return internal_compare_unequal(a.signum, b.signum);
        if(a.signum == 0) return 0; // both zero
        // okay, both are either positive or negative.
        return a.signum * abs_compare(a, b);
    }

    // add a and b, assuming both are positive. this function ignores the signs in the view
    [[nodiscard]] static numview abs_add(numview c, const numview a, const numview b)
    {
        c.n_digits = 0;
        double_digit_t carry = 0;

        const digit_t *a_ptr = a.digits;
        const digit_t *a_end = a.digits + a.n_digits;
        const digit_t *b_ptr = b.digits;
        const digit_t *b_end = b.digits + b.n_digits;

        while(a_ptr < a_end && b_ptr < b_end)
        {
            double_digit_t v = double_digit_t(*a_ptr++) + double_digit_t(*b_ptr++) + carry;
            c.digits[c.n_digits++] = v;
            carry = v >> n_bits_in_digit;
        }

        while(a_ptr < a_end)
        {
            double_digit_t v = double_digit_t(*a_ptr++) + carry;
            c.digits[c.n_digits++] = v;
            carry = v >> n_bits_in_digit;
        }

        while(b_ptr < b_end)
        {
            double_digit_t v = double_digit_t(*b_ptr++) + carry;
            c.digits[c.n_digits++] = v;
            carry = v >> n_bits_in_digit;
        }
        if(carry != 0)
        {
            c.digits[c.n_digits++] = carry;
            carry = 0;
        }
        return c;
    }

    // add a and b, assuming both are positive. this function ignores the signs in the view
    [[nodiscard]] static numview abs_add_digit(numview c, const numview a, digit_t b)
    {
        c.n_digits = 0;

        const digit_t *a_ptr = a.digits;
        const digit_t *a_end = a.digits + a.n_digits;

        double_digit_t carry = b;
        while(a_ptr < a_end)
        {
            double_digit_t v = double_digit_t(*a_ptr++) + carry;
            c.digits[c.n_digits++] = v;
            carry = v >> n_bits_in_digit;
        }

        if(carry != 0)
        {
            c.digits[c.n_digits++] = carry;
            carry = 0;
        }
        return c;
    }

    // subtract b from a, assuming a is larger than b. this function ignores the signs in the view
    // okay to alias a and c, as long as a is large enough.
    [[nodiscard]] numview abs_subtract_a_larger_than_b(numview c, const numview a, const numview b)
    {
        c.n_digits = 0;
        double_digit_t carry = 0;
        const digit_t *a_ptr = a.digits;
        const digit_t *a_end = a.digits + a.n_digits;
        const digit_t *b_ptr = b.digits;
        const digit_t *b_end = b.digits + b.n_digits;
        assert(a.n_digits >= b.n_digits);
        while(b_ptr < b_end)
        {
            double_digit_t v = double_digit_t(*a_ptr++) - double_digit_t(*b_ptr++) + carry;
            c.digits[c.n_digits++] = v;
            carry = signed_double_digit_t(v) >> n_bits_in_digit;
        }
        assert(b_ptr == b_end);

        while(a_ptr < a_end)
        {
            double_digit_t v = double_digit_t(*a_ptr++) + carry;
            c.digits[c.n_digits++] = v;
            carry = signed_double_digit_t(v) >> n_bits_in_digit;
        }

        assert(carry == 0);

        return remove_high_zeros(c);
    }

    [[nodiscard]] numview add(numview c, numview a, numview b)
    {
        // take care of zero cases
        if(a.signum == 0) return copy_view(c, b);
        if(b.signum == 0) return copy_view(c, a);

        if(a.signum == b.signum)
        {
            return with_signum(a.signum, abs_add(c, a, b));
        } else
        {
            // unequal sign, and both of them non-zero
            signum_t comparison = abs_compare(a, b);
            if(comparison == 0)
            {
                // they're equal! return a zero view
                return zero_out(c);
            }
            if(comparison < 0)
            {
                // b larger than a, swap the two
                std::swap(b, a);
            }

            // a is larger than b. subtract the two and attach the a sign
            return with_signum(a.signum, abs_subtract_a_larger_than_b(c, a, b));
        }
    }

    // multiply of positive numbers, ignoring sign. prefer a large and b small
    [[nodiscard]] static numview abs_multiply(numview c, const numview a, const numview b)
    {
        c = zero_with_n_digits(c, multiply_digit_estimate(a.n_digits, b.n_digits));

        for(uint32_t b_idx = 0; b_idx < b.n_digits; ++b_idx)
        {
            double_digit_t b_val = b.digits[b_idx];
            double_digit_t carry = 0;
            uint32_t c_idx = b_idx;
            for(uint32_t a_idx = 0; a_idx < a.n_digits; ++a_idx)
            {
                double_digit_t v = double_digit_t(a.digits[a_idx]) * b_val + carry + double_digit_t(c.digits[c_idx]);
                c.digits[c_idx++] = v;
                carry = v >> n_bits_in_digit;
            }
            while(carry != 0)
            {
                double_digit_t v = carry + double_digit_t(c.digits[c_idx]);
                c.digits[c_idx++] = v;
                carry = v >> n_bits_in_digit;
            }
        }

        return remove_high_zeros(c);
    }

    [[nodiscard]] numview multiply(numview c, const numview a, const numview b)
    {
        if(a.signum == 0) return zero_out(c);
        if(b.signum == 0) return zero_out(c);

        return with_signum(a.signum * b.signum, abs_multiply(c, a, b));
    }

    [[nodiscard]] numview multiply_with_single_digit(numview c, const numview a, digit_t b)
    {
        if(a.signum == 0) return zero_out(c);
        if(b == 0) return zero_out(c);
        double_digit_t b_val = b;
        double_digit_t carry = 0;
        c.signum = a.signum;
        c.n_digits = 0;
        for(uint32_t a_idx = 0; a_idx < a.n_digits; ++a_idx)
        {
            double_digit_t v = double_digit_t(a.digits[a_idx]) * b_val + carry;
            c.digits[c.n_digits++] = v;
            carry = v >> n_bits_in_digit;
        }
        if(carry != 0)
        {
            c.digits[c.n_digits++] = carry;
        }
        return c;
    }

    [[nodiscard]] numview abs_divmod_by_single_digit(numview quotient, digit_t *remainder_ptr, const numview dividend, const digit_t divisor32)
    {

        quotient = zero_with_n_digits(quotient, dividend.n_digits);

        double_digit_t remainder = 0;
        double_digit_t divisor = divisor32;

        for(int32_t idx = dividend.n_digits - 1; idx >= 0; --idx)
        {
            assert(remainder < (1ull << n_bits_in_digit));
            remainder = (remainder << n_bits_in_digit) | dividend.digits[idx];
            double_digit_t q = remainder / divisor;
            remainder = remainder % divisor;
            assert(q < (1ull << n_bits_in_digit));
            quotient.digits[idx] = q;
        }
        assert(remainder < (1ull << n_bits_in_digit));
        if(remainder_ptr != nullptr)
        {
            *remainder_ptr = remainder;
        }

        return remove_high_zeros(quotient);
    }

    [[nodiscard]] numview divmod_by_single_digit(numview quotient, int64_t *modulo, const numview dividend, const digit_t divisor)
    {
        if(divisor == 0) throw std::out_of_range("divide by zero");
        if(dividend.signum == 0) return zero_out(quotient);
        digit_t non_neg_modulo = 0;
        quotient = with_sign_unless_zero(dividend.signum, abs_divmod_by_single_digit(quotient, &non_neg_modulo, dividend, divisor));

        if(modulo != nullptr)
        {
            *modulo = int64_t(non_neg_modulo) * dividend.signum;
        }
        return quotient;
    }

    [[nodiscard]] static numview divmod_normalised(numview quotient, numview *remainder, numview dividend, const numview divisor)
    {
        assert(dividend.n_digits > 0);
        assert(divisor.n_digits > 0);
        assert(dividend.n_digits >= divisor.n_digits);
        digit_t msb_divisor = divisor.digits[divisor.n_digits - 1];
        digit_t nextsb_divisor = divisor.n_digits >= 2 ? divisor.digits[divisor.n_digits - 2] : 0;
        assert((msb_divisor & (1 << (n_bits_in_digit - 1))) != 0); // has been normalised

        // now let's go with Knuth's algorithm for division of non-negative integers, Art of Computer Programming Volume 2, 4.3, Algorithm D

        uint32_t n = divisor.n_digits;
        uint32_t m = dividend.n_digits - n - 1;

        MAKE_STACK_TEMPORARY_NUMVIEW(qv, n + 1);

        constexpr double_digit_t b = double_digit_t(1) << n_bits_in_digit;
        for(int32_t j = m; j >= 0; j--)
        {
            double_digit_t dividend2 = (double_digit_t(dividend.digits[j + n]) << n_bits_in_digit) | double_digit_t(dividend.digits[j + n - 1]);
            double_digit_t q_hat = dividend2 / msb_divisor;
            double_digit_t r_hat = dividend2 % msb_divisor;

            double_digit_t u_jn_2 = j + n >= 2 ? dividend.digits[j + n - 2] : 0;

            while(q_hat == b || q_hat * nextsb_divisor > b * r_hat + u_jn_2)
            {
                --q_hat;
                r_hat += msb_divisor;

                if(r_hat >= b) break;
            }

            numview dividend_until_j = dividend;
            dividend_until_j.digits += j;
            dividend_until_j.n_digits = n + 1;

            qv = multiply_with_single_digit(qv, divisor, q_hat);
            while(abs_compare(dividend_until_j, qv) < 0)
            {
                --q_hat;
                r_hat += msb_divisor;
                qv = multiply_with_single_digit(qv, divisor, q_hat);
            }

            quotient.digits[j] = q_hat;
            dividend_until_j = abs_subtract_a_larger_than_b(dividend_until_j, dividend_until_j, qv);
        }
        quotient.n_digits = m + 1;
        *remainder = remove_high_zeros(dividend);
        return with_sign_unless_zero(dividend.signum * divisor.signum, remove_high_zeros(quotient));
    }

    [[nodiscard]] numview divmod(numview quotient, numview *remainder, const numview dividend, const numview divisor)
    {
        if(divisor.signum == 0) throw std::out_of_range("divide by zero");
        if(dividend.signum == 0 || divisor.n_digits > dividend.n_digits)
        {
            if(remainder != nullptr) *remainder = copy_view(*remainder, dividend);
            return zero_out(quotient);
        }
        uint32_t normalization_shift = countl_zero(divisor.digits[divisor.n_digits - 1]);
        MAKE_STACK_TEMPORARY_NUMVIEW(norm_dividend, dividend.n_digits + 1);
        MAKE_STACK_TEMPORARY_NUMVIEW(norm_divisor, divisor.n_digits); // won't overflow
        MAKE_STACK_TEMPORARY_NUMVIEW(norm_remainder, divisor.n_digits);
        norm_dividend = shift_left(norm_dividend, dividend, normalization_shift);
        norm_divisor = shift_left(norm_divisor, divisor, normalization_shift);

        if(norm_dividend.n_digits == dividend.n_digits)
        {
            norm_dividend.digits[norm_dividend.n_digits++] = 0; // put an extra zero in there, the divmod_normalised algorithm needs it
        }

        quotient = divmod_normalised(quotient, &norm_remainder, norm_dividend, norm_divisor);
        if(remainder != nullptr)
        {
            if(norm_remainder.n_digits == 0) norm_remainder.signum = 0;
            *remainder = shift_right(*remainder, norm_remainder, normalization_shift);
        }
        return quotient;
    }

    [[nodiscard]] numview shift_left(numview c, const numview a, uint32_t shift_amount)
    {
        if(a.signum == 0) return zero_out(c);
        c.signum = a.signum;

        uint32_t shift_whole_digits = shift_amount / n_bits_in_digit;
        uint32_t shift_left_within_digits = shift_amount % n_bits_in_digit;

        c.n_digits = 0;
        for(uint32_t idx = 0; idx < shift_whole_digits; ++idx)
        {
            c.digits[c.n_digits++] = 0;
        }

        if(shift_left_within_digits == 0)
        {
            for(uint32_t idx = 0; idx < a.n_digits; ++idx)
            {
                c.digits[c.n_digits++] = a.digits[idx];
            }
        } else
        {
            uint32_t shift_right_within_digits = n_bits_in_digit - shift_left_within_digits;
            digit_t extra = 0;
            for(uint32_t idx = 0; idx < a.n_digits; ++idx)
            {
                digit_t ad = a.digits[idx];
                c.digits[c.n_digits++] = extra | (ad << shift_left_within_digits);
                extra = ad >> shift_right_within_digits;
            }
            if(extra != 0)
            {
                c.digits[c.n_digits++] = extra;
            }
        }
        return c;
    }

    [[nodiscard]] numview shift_right(numview c, const numview a, uint32_t shift_amount)
    {
        if(a.signum == 0) return zero_out(c);

        uint64_t shift_whole_digits = shift_amount / n_bits_in_digit;
        uint32_t shift_right_within_digits = shift_amount % n_bits_in_digit;
        uint32_t shift_left_within_digits = n_bits_in_digit - shift_right_within_digits;

        digit_t extra = 0;
        c.n_digits = std::max<int64_t>(0, a.n_digits - shift_whole_digits);
        if(shift_right_within_digits == 0)
        {
            for(int64_t idx = a.n_digits - 1 - shift_whole_digits; idx >= 0; --idx)
            {
                digit_t ad = a.digits[idx + shift_whole_digits];
                c.digits[idx] = ad;
            }

        } else
        {
            for(int64_t idx = a.n_digits - 1 - shift_whole_digits; idx >= 0; --idx)
            {
                digit_t ad = a.digits[idx + shift_whole_digits];

                c.digits[idx] = extra | (ad >> shift_right_within_digits);
                extra = ad << shift_left_within_digits;
            }
        }

        // arithmetic shift right is a flooring division. therefore, if we have a negative a, we should add 1 to the magnitude (in sign-magnitude representation) if we shift out any 1 bits
        if(a.signum == -1)
        {
            bool round_up = extra != 0;
            if(!round_up)
            {
                for(uint32_t idx = 0; idx < shift_whole_digits && idx < a.n_digits; ++idx)
                {
                    round_up = a.digits[idx] != 0;
                    if(round_up) break;
                }
            }
            if(round_up)
            {
                // a little naughty, our routines are generally not resistant against aliasing. but this one is safe
                c = abs_add_digit(c, c, 1);
            }
        }

        return with_sign_unless_zero(a.signum, remove_high_zeros(c));
    }

    uint32_t countr_zero(const numview v)
    {
        assert(v.signum == 1);
        uint32_t result = 0;
        for(uint32_t i = 0; i < v.n_digits; ++i, result += n_bits_in_digit)
        {
            digit_t vd = v.digits[i];
            if(vd != 0)
            {
                result += countr_zero(vd);
                break;
            }
        }
        return result;
    }

    [[nodiscard]] numview binary_gcd(numview c, numview a, numview b)
    {
        // make both sides non-negative
        a.signum = std::abs(a.signum);
        b.signum = std::abs(b.signum);

        // handle the zero cases. gcd(a, 0) = 0, gcd(0, b) = 0, gcd(0, 0) = 0
        if(a.signum == 0) return copy_view(c, b);
        if(b.signum == 0) return copy_view(c, a);

        MAKE_STACK_TEMPORARY_NUMVIEW(aa, a.n_digits);
        MAKE_STACK_TEMPORARY_NUMVIEW(bb, b.n_digits);

        uint32_t a_n_pow2s = countr_zero(a);
        uint32_t b_n_pow2s = countr_zero(b);
        uint32_t common_pow2s = std::min(a_n_pow2s, b_n_pow2s);
        aa = shift_right(aa, a, a_n_pow2s);
        bb = shift_right(bb, b, b_n_pow2s);

        while(true)
        {
            int cmp = abs_compare(aa, bb);
            if(cmp == 0) break; // found it
            if(cmp < 0)
            {
                std::swap(aa, bb);
            }
            // now aa > bb

            aa = abs_subtract_a_larger_than_b(aa, aa, bb);
            uint32_t aa_n_pow2s = countr_zero(aa);
            aa = shift_right(aa, aa, aa_n_pow2s);
        }

        return shift_left(c, aa, common_pow2s);
    }

} // namespace rqm
