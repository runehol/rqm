#ifndef RQM_DETAIL_BASIC_ARITHMETIC_H
#define RQM_DETAIL_BASIC_ARITHMETIC_H

namespace rqm
{
    namespace detail
    {
        template<typename T>
        static inline signum_t internal_compare_unequal(T a, T b)
        {
            return a > b ? 1 : -1;
        }

        // compare a and b, assuming both are positive. this function ignores the signs in the view
        static inline signum_t abs_compare(numview a, numview b)
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

        static inline signum_t compare(numview a, numview b)
        {
            // if not the same size, return the difference of the signs
            if(a.signum != b.signum) return internal_compare_unequal(a.signum, b.signum);
            if(a.signum == 0) return 0; // both zero
            // okay, both are either positive or negative.
            return a.signum * abs_compare(a, b);
        }

    } // namespace detail
} // namespace rqm

#endif // RQM_DETAIL_BASIC_ARITHMETIC_H
