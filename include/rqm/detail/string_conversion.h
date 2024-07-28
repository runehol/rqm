#ifndef RQM_DETAIL_STRING_CONVERSION_H
#define RQM_DETAIL_STRING_CONVERSION_H

#include "rqm/detail/basic_arithmetic.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <string_view>

namespace rqm
{
    namespace detail
    {

        [[nodiscard]] static inline char *chomp_leading_zeros(char *pos)
        {
            while(*pos == '0')
                ++pos;
            return pos;
        }

        [[nodiscard]] static inline uint32_t to_string_buffer_estimate(uint32_t n_digits)
        {
            // 1 for possible sign, maximum number of decimals per digits times number of digits
            return 100 + 1 + n_decimals_in_digit_high * n_digits;
        }

        [[nodiscard]] static inline std::string_view to_string(char *dest_end, const numview n)
        {
            if(n.signum == 0) return std::string_view("0"); // special-case zeros

            MAKE_STACK_TEMPORARY_NUMVIEW(value, n.n_digits);
            MAKE_STACK_TEMPORARY_NUMVIEW(value2, n.n_digits);

            memcpy(value.digits, n.digits, n.n_digits * sizeof(n.digits[0]));
            value.signum = 1; // it's positive now. negativeness is handled at the end
            value.n_digits = n.n_digits;

            char *pos = dest_end;
            char *end = pos;

            while(value.n_digits > 0)
            {
                digit_t value_to_format;

                value2 = abs_divmod_by_single_digit(value2, &value_to_format, value, decimal_digit_modulus);
                std::swap(value, value2);

                // value_to_format holds the remainder for this division. format it.
                char buf[n_decimals_in_digit_low + 1];
                snprintf(buf, n_decimals_in_digit_low + 1, "%09u", value_to_format);
                pos -= n_decimals_in_digit_low;
                memcpy(pos, buf, n_decimals_in_digit_low);
            }

            pos = chomp_leading_zeros(pos);

            if(n.signum < 0)
            {
                *--pos = '-';
            }
            return std::string_view(pos, end - pos);
        }

    } // namespace detail
} // namespace rqm

#endif // RQM_DETAIL_STRING_CONVERSION_H
