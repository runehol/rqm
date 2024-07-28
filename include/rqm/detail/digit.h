#ifndef RQM_DETAIL_DIGIT_H
#define RQM_DETAIL_DIGIT_H

#include <cstddef>
#include <cstdint>

namespace rqm
{
    namespace detail
    {
        using signum_t = int32_t; // sign as -1, 0, 1 representation

        using digit_t = uint32_t;
        static constexpr uint32_t n_digit_bits = sizeof(digit_t) * 8;

        using double_digit_t = uint64_t;
        using signed_double_digit_t = int64_t;
        static constexpr uint32_t n_double_digit_bits = sizeof(double_digit_t) * 8;

    } // namespace detail

} // namespace rqm

#endif // RQM_DETAIL_DIGIT_H