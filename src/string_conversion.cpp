
#include "string_conversion.h"
#include "basic_arithmetic.h"
#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <string_view>

namespace rqm
{
    [[nodiscard]] static char *chomp_leading_zeros(char *pos)
    {
        while(*pos == '0')
            ++pos;
        return pos;
    }

    [[nodiscard]] std::string_view to_string(char *end, const numview n)
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

    // from_chars converts a numeric string in base-10 to a numview.
    // we take two buffers, dest1 and dest2, and may use either of them for the destination
    [[nodiscard]] numview from_chars(numview dest, const char *pos, const char *end)
    {
        signum_t sign = 1;
        if(pos == end)
        {
            throw std::invalid_argument("no characters to convert");
        }
        if(*pos == '-')
        {
            sign = -1;
            ++pos;
        }

        // now onto the main parsing

        {
            MAKE_STACK_TEMPORARY_NUMVIEW(single_digit, 1);
            MAKE_STACK_TEMPORARY_NUMVIEW(tmp, from_chars_digit_estimate(end - pos));
            bool first = true;
            while(pos < end)
            {
                uint32_t n_digits = std::min<uint32_t>(end - pos, n_decimals_in_digit_low);
                auto [ptr, ec] = std::from_chars(pos, pos + n_digits, single_digit_storage[0]);
                if(ec != std::errc() || ptr != pos + n_digits)
                {
                    throw std::invalid_argument("Not a number");
                }
                pos = ptr;

                single_digit.n_digits = single_digit.signum = single_digit_storage[0] != 0;

                if(first)
                {
                    dest = copy_view(dest, single_digit);
                    first = false;
                } else
                {
                    digit_t scale = digit_t(std::pow(10.0, n_digits));
                    tmp = multiply_with_single_digit(tmp, dest, scale);
                    dest = add(dest, tmp, single_digit);
                }
            }
        }
        return with_sign_unless_zero(sign, dest);
    }
} // namespace rqm
