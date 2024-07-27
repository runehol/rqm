#ifndef RQM_DETAIL_BASIC_ARITHMETIC_H
#define RQM_DETAIL_BASIC_ARITHMETIC_H

#include "rqm/detail/numview.h"
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
                double_digit_t v = double_digit_t(*a_ptr++) + double_digit_t(*b_ptr) + carry;
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
                double_digit_t v = double_digit_t(*b_ptr) + carry;
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
                double_digit_t v = double_digit_t(*a_ptr++) - double_digit_t(*b_ptr) + carry;
                c.digits[c.n_digits++] = v;
                carry = v >> n_digit_bits;
            }
            assert(b_ptr == b_end);

            while(a_ptr < a_end)
            {
                double_digit_t v = double_digit_t(*a_ptr++) + carry;
                c.digits[c.n_digits++] = v;
                carry = v >> n_digit_bits;
            }

            assert(carry == 0);

            return c;
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
                    return numview(nullptr);
                }
                if(comparison > 0)
                {
                    // b larger than a, swap the two and change the signs
                    std::swap(b, a);
                    a.signum = -a.signum;
                    b.signum = -b.signum;
                }

                // a is larger than b. subtract the two and attach the a sign
                return with_signum(a.signum, abs_subtract_a_larger_than_b(c, a, b));
            }
        }

    } // namespace detail
} // namespace rqm

#endif // RQM_DETAIL_BASIC_ARITHMETIC_H
