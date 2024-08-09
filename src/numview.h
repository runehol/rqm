#ifndef RQM_DETAIL_NUMVIEW_H
#define RQM_DETAIL_NUMVIEW_H

#include "rqm/digit.h"
#include <cstddef>
#include <cstdint>

namespace rqm
{

    /**
       A non-owning view of a number, used for calculations.
       The storage is borrowed from elsewhere, and must be kept alive for the duration of this view.
     */
    struct numview
    {
        /**
           Construct a number with value and borrowed storage
         */
        constexpr numview(uint32_t _n_digits, signum_t _signum, const digit_t *_digits)
            : n_digits(_n_digits),
              signum(_signum),
              digits(const_cast<digit_t *>(_digits))
        {}

        /**
           Construct an empty number with borrowed storage provided, used for results
         */
        constexpr numview(digit_t *_digits)
            : n_digits(0),
              signum(0),
              digits(_digits)
        {}

        uint32_t n_digits;
        signum_t signum;
        digit_t *digits;
    };

#define MAKE_STACK_TEMPORARY_NUMVIEW(name, n_digits)                                                                                                                                                   \
    digit_t name##_storage[(n_digits)];                                                                                                                                                                \
    numview name(name##_storage)

} // namespace rqm

#endif // RQM_DETAIL_NUMVIEW_H
