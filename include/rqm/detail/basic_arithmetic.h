#ifndef RQM_DETAIL_BASIC_ARITHMETIC_H
#define RQM_DETAIL_BASIC_ARITHMETIC_H

#include "rqm/detail/numview.h"
#include <algorithm>
#include <cassert>
#include <cstdint>

namespace rqm
{
    namespace detail
    {
        template<typename T>
        static inline signum_t compare_signum(T a, T b)
        {
            if(a < b) return -1;
            if(a > b) return 1;
            return 0;
        }

        template<typename T>
        static inline signum_t internal_compare_unequal(T a, T b)
        {
            return a > b ? 1 : -1;
        }

        // compare a and b, assuming both are positive. this function ignores the signs in the view
        static inline signum_t abs_compare(const numview a, const numview b)
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

        static inline signum_t compare(const numview a, const numview b)
        {
            // if not the same size, return the difference of the signs
            if(a.signum != b.signum) return internal_compare_unequal(a.signum, b.signum);
            if(a.signum == 0) return 0; // both zero
            // okay, both are either positive or negative.
            return a.signum * abs_compare(a, b);
        }

        static inline numview negate(numview a)
        {
            a.signum = -a.signum;
            return a;
        }

        static inline numview abs(numview a)
        {
            a.signum = std::abs(a.signum);
            return a;
        }

        static inline numview with_signum(signum_t signum, numview a)
        {
            a.signum = signum;
            return a;
        }

        static inline numview with_sign_unless_zero(signum_t signum, numview a)
        {
            a.signum = signum;
            if(a.n_digits == 0) a.signum = 0;
            return a;
        }

        static inline numview remove_high_zeros(numview c)
        {
            // adjust down after cancellation
            while(c.n_digits > 0 && c.digits[c.n_digits - 1] == 0)
            {
                --c.n_digits;
            }
            return c;
        }

        // add a and b, assuming both are positive. this function ignores the signs in the view
        static inline numview abs_add(numview c, const numview a, const numview b)
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
                carry = v >> n_digit_bits;
            }

            while(a_ptr < a_end)
            {
                double_digit_t v = double_digit_t(*a_ptr++) + carry;
                c.digits[c.n_digits++] = v;
                carry = v >> n_digit_bits;
            }

            while(b_ptr < b_end)
            {
                double_digit_t v = double_digit_t(*b_ptr++) + carry;
                c.digits[c.n_digits++] = v;
                carry = v >> n_digit_bits;
            }
            if(carry != 0)
            {
                c.digits[c.n_digits++] = carry;
                carry = 0;
            }
            return c;
        }

        // subtract b from a, assuming a is larger than b. this function ignores the signs in the view
        static inline numview abs_subtract_a_larger_than_b(numview c, const numview a, const numview b)
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
                carry = signed_double_digit_t(v) >> n_digit_bits;
            }
            assert(b_ptr == b_end);

            while(a_ptr < a_end)
            {
                double_digit_t v = double_digit_t(*a_ptr++) + carry;
                c.digits[c.n_digits++] = v;
                carry = signed_double_digit_t(v) >> n_digit_bits;
            }

            assert(carry == 0);

            return remove_high_zeros(c);
        }

        static inline numview add(numview c, numview a, numview b)
        {
            // take care of zero cases
            if(a.signum == 0) return b;
            if(b.signum == 0) return a;

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
                    return numview::zero();
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

        static inline uint32_t add_digit_estimate(uint32_t a_digits, uint32_t b_digits)
        {
            return std::max(a_digits, b_digits) + 1;
        }

        static inline uint32_t multiply_digit_estimate(uint32_t a_digits, uint32_t b_digits)
        {
            return a_digits + b_digits;
        }

        static inline numview zero_with_n_digits(numview c, uint32_t n_digits)
        {
            // set it all to zero
            c.n_digits = n_digits;
            memset(c.digits, 0, n_digits * sizeof(c.digits[0]));
            return c;
        }

        // multiply of positive numbers, ignoring sign. prefer a large and b small
        static inline numview abs_multiply(numview c, const numview a, const numview b)
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
                    carry = v >> n_digit_bits;
                }
                while(carry != 0)
                {
                    double_digit_t v = carry + double_digit_t(c.digits[c_idx]);
                    c.digits[c_idx++] = v;
                    carry = v >> n_digit_bits;
                }
            }

            return remove_high_zeros(c);
        }

        static inline numview multiply(numview c, const numview a, const numview b)
        {
            if(a.signum == 0) return numview::zero();
            if(b.signum == 0) return numview::zero();

            return with_signum(a.signum * b.signum, abs_multiply(c, a, b));
        }

        static inline numview multiply_with_single_digit(numview c, const numview a, digit_t b)
        {
            if(a.signum == 0) return numview::zero();
            if(b == 0) return numview::zero();
            double_digit_t b_val = b;
            double_digit_t carry = 0;
            c.signum = a.signum;
            c.n_digits = 0;
            for(uint32_t a_idx = 0; a_idx < a.n_digits; ++a_idx)
            {
                double_digit_t v = double_digit_t(a.digits[a_idx]) * b_val + carry;
                c.digits[c.n_digits++] = v;
                carry = v >> n_digit_bits;
            }
            if(carry != 0)
            {
                c.digits[c.n_digits++] = carry;
            }
            return c;
        }

        static inline uint32_t quotient_digit_estimate(uint32_t dividend_digits)
        {
            return dividend_digits;
        }

        static inline numview abs_divmod_by_single_digit(numview quotient, digit_t *remainder_ptr, const numview dividend, const digit_t divisor32)
        {

            quotient = zero_with_n_digits(quotient, dividend.n_digits);

            double_digit_t remainder = 0;
            double_digit_t divisor = divisor32;

            for(int32_t idx = dividend.n_digits - 1; idx >= 0; --idx)
            {
                assert(remainder < (1ull << n_digit_bits));
                remainder = (remainder << n_digit_bits) | dividend.digits[idx];
                double_digit_t q = remainder / divisor;
                remainder = remainder % divisor;
                assert(q < (1ull << n_digit_bits));
                quotient.digits[idx] = q;
            }
            assert(remainder < (1ull << n_digit_bits));
            if(remainder_ptr != nullptr)
            {
                *remainder_ptr = remainder;
            }

            return remove_high_zeros(quotient);
        }

        static inline numview divide_by_single_digit(numview quotient, const numview dividend, const digit_t divisor)
        {
            if(divisor == 0) throw std::out_of_range("divide by zero");
            if(dividend.signum == 0) return numview::zero();
            return with_sign_unless_zero(dividend.signum, abs_divmod_by_single_digit(quotient, nullptr, dividend, divisor));
        }

    } // namespace detail
} // namespace rqm

#endif // RQM_DETAIL_BASIC_ARITHMETIC_H
