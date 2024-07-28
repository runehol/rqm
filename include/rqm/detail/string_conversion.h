#ifndef RQM_DETAIL_STRING_CONVERSION_H
#define RQM_DETAIL_STRING_CONVERSION_H

#include "rqm/detail/basic_arithmetic.h"
#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <string_view>

namespace rqm
{
    namespace detail
    {

        [[nodiscard]] static inline constexpr uint32_t to_string_buffer_estimate(uint32_t n_digits)
        {
            // 1 for possible sign, maximum number of decimals per digits times number of digits, and one for the terminating zero in the fast path
            return 1 + 1 + n_decimals_in_digit_high * n_digits;
        }

        [[nodiscard]] std::string_view to_string(char *end, const numview n);

        [[nodiscard]] static inline constexpr uint32_t from_chars_digit_estimate(uint32_t n_chars)
        {
            // at least one digit, plus one for each time we have a new batch of n_decimals_in_digit_low decimals
            return 1 + n_chars / n_decimals_in_digit_low;
        }

        // from_chars converts a numeric string in base-10 to a numview.
        // we take two buffers, dest1 and dest2, and may use either of them for the destination
        [[nodiscard]] numview from_chars(numview dest, const char *pos, const char *end);
    } // namespace detail
} // namespace rqm

#endif // RQM_DETAIL_STRING_CONVERSION_H
