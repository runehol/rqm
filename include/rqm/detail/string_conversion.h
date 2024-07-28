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

        [[nodiscard]] static inline constexpr uint32_t to_string_buffer_estimate(uint32_t n_digits)
        {
            // 1 for possible sign, maximum number of decimals per digits times number of digits, and one for the terminating zero in the fast path
            return 1 + 1 + n_decimals_in_digit_high * n_digits;
        }

        [[nodiscard]] static inline std::string_view to_string(char *end, const numview n)
        {
            if(n.signum == 0) return std::string_view("0"); // special-case zeros

            // fast path single-digit value
            if(n.n_digits == 1)
            {
                signed_double_digit_t v = signed_double_digit_t(n.digits[0]) * n.signum;
                uint32_t max_possible = to_string_buffer_estimate(1);
                char *pos = end - max_possible;
                int n_written = snprintf(pos, max_possible, "%lld", v);
                return std::string_view(pos, n_written);
            }

            MAKE_STACK_TEMPORARY_NUMVIEW(value, n.n_digits);
            MAKE_STACK_TEMPORARY_NUMVIEW(value2, n.n_digits);

            memcpy(value.digits, n.digits, n.n_digits * sizeof(n.digits[0]));
            value.signum = 1; // it's positive now. negativeness is handled at the end
            value.n_digits = n.n_digits;

            char *pos = end;

            while(value.n_digits > 0)
            {
                digit_t value_to_format;

                value2 = abs_divmod_by_single_digit(value2, &value_to_format, value, decimal_digit_modulus);
                std::swap(value, value2);

                // value_to_format holds the remainder for this division. format it.
                char buf[n_decimals_in_digit_low + 1];
                int n_chars_written;
                if(value.n_digits > 0)
                {
                    // there's more left. make sure we have 9 characters with leading zeros if necessary
                    n_chars_written = snprintf(buf, n_decimals_in_digit_low + 1, "%09u", value_to_format);
                } else
                {
                    // just the actual non-zero values, please
                    n_chars_written = snprintf(buf, n_decimals_in_digit_low + 1, "%u", value_to_format);
                }
                pos -= n_chars_written;
                memcpy(pos, buf, n_chars_written);
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
