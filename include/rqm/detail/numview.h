#ifndef RQM_NUMVIEW_H
#define RQM_NUMVIEW_H

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

        /**
           A non-owning view of a number, used for calculations.
           The storage is borrowed from elsewhere, and must be kept alive for the duration of this view.
         */
        struct numview
        {
            /**
               Construct a number with value and borrowed storage
             */
            numview(uint32_t _n_digits, signum_t _signum, const digit_t *_digits)
                : n_digits(_n_digits),
                  signum(_signum),
                  digits(const_cast<digit_t *>(_digits))
            {}

            /**
               Construct an empty number with borrowed storage provided, used for results
             */
            numview(digit_t *_digits)
                : n_digits(0),
                  signum(0),
                  digits(_digits)
            {}

            uint32_t n_digits;
            signum_t signum;
            digit_t *digits;
        };
    } // namespace detail

} // namespace rqm

#endif // RQM_NUMVIEW_H
