#ifndef RQM_NUM_H
#define RQM_NUM_H

#include "rqm/detail/numview.h"
#include <cstdint>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

#include "rqm/detail/basic_arithmetic.h"
#include "rqm/detail/string_conversion.h"

namespace rqm
{

    /**
       big integer class

       The value is held as sign-magnitude with signum indicating whether the value is positive, negative or zero.
       If the unsigned value would fit into a pointer, it's held inline with the number, otherwise we allocate memory to hold the digits
    */
    class num
    {
    public:
        // this class allocates, so we need the rule of five
        num()
            : n_digits(0),
              signum(0)
        {}

        ~num()

        {
            if(!stored_inline())
            {
                delete[] u.digits_ptr;
            }
        }

        num(const num &o) // copy constructor
        {
            signum = o.signum;
            detail::digit_t *ptr = setup_storage(o.n_digits);
            std::memcpy(ptr, o.digits(), n_digits * sizeof(detail::digit_t));
        }

        num(num &&o) noexcept // move constructor
        {
            signum = o.signum;
            if(o.stored_inline())
            {
                detail::digit_t *ptr = setup_storage(o.n_digits);
                std::memcpy(ptr, o.u.digits_inline, n_digits * sizeof(detail::digit_t));
            } else
            {
                n_digits = o.n_digits;
                u.digits_ptr = std::exchange(o.u.digits_ptr, nullptr);
            }
        }

        num &operator=(const num &o) // copy assignment
        {
            return *this = num(o);
        }

        num &operator=(num &&o) noexcept // move assignment
        {
            if(&o != this)
            {
                signum = o.signum;
                if(!stored_inline())
                {
                    delete[] u.digits_ptr; // deallocate
                }
                u.digits_ptr = nullptr;

                if(o.stored_inline())
                {
                    n_digits = o.n_digits;
                    std::memcpy(u.digits_inline, o.u.digits_inline, n_digits * sizeof(detail::digit_t));
                } else
                {
                    std::swap(n_digits, o.n_digits);
                    std::swap(u.digits_ptr, o.u.digits_ptr);
                }
            }
            return *this;
        }

        explicit num(detail::numview o)
            : signum(o.signum)
        {
            detail::digit_t *ptr = setup_storage(o.n_digits);
            std::memcpy(ptr, o.digits, n_digits * sizeof(detail::digit_t));
        }
        detail::numview to_numview() const { return detail::numview(n_digits, signum, digits()); }

        num(int64_t value)
        {
            signum = detail::compare_signum(value, int64_t(0));
            int64_t abs_value = std::abs(value);
            u.digits_inline[0] = abs_value & 0xffffffff;
            u.digits_inline[1] = abs_value >> 32;
            uint32_t digs = 2;
            if(u.digits_inline[1] == 0)
            {
                digs = 1;
                if(u.digits_inline[0] == 0) digs = 0;
            }
            n_digits = digs;
        }

        int64_t to_int64_t() const
        {
            uint64_t v = 0;
            if(n_digits > n_inline_digits) throw std::overflow_error("Out of range for an int64_t");

            const detail::digit_t *ptr = digits();
            for(uint32_t idx = 0; idx < n_digits; ++idx)
            {
                v |= uint64_t(ptr[idx]) << (idx * detail::n_digit_bits);
            }
            if(v > std::numeric_limits<int64_t>::max()) throw std::overflow_error("Out of range for an int64_t");
            return v * signum;
        }
        uint32_t get_n_digits() const { return n_digits; }

    private:
        uint32_t *setup_storage(size_t _n_digits)
        {
            n_digits = _n_digits;
            if(_n_digits <= n_inline_digits)
            {
                return u.digits_inline;
            } else
            {
                return (u.digits_ptr = new detail::digit_t[_n_digits]);
            }
        }

        static constexpr uint32_t n_inline_digits = 2;

        bool stored_inline() const { return n_digits <= n_inline_digits; }
        const detail::digit_t *digits() const { return stored_inline() ? u.digits_inline : u.digits_ptr; }

        uint32_t n_digits;
        detail::signum_t signum;
        union
        {
            detail::digit_t *digits_ptr;
            detail::digit_t digits_inline[n_inline_digits];
        } u;
    };

    static inline num abs(const num &a)
    {
        return num(detail::abs(a.to_numview()));
    }

    static inline bool operator==(const num &a, const num &b)
    {
        return detail::compare(a.to_numview(), b.to_numview()) == 0;
    }
    static inline bool operator!=(const num &a, const num &b)
    {
        return detail::compare(a.to_numview(), b.to_numview()) != 0;
    }
    static inline bool operator<(const num &a, const num &b)
    {
        return detail::compare(a.to_numview(), b.to_numview()) < 0;
    }
    static inline bool operator<=(const num &a, const num &b)
    {
        return detail::compare(a.to_numview(), b.to_numview()) <= 0;
    }
    static inline bool operator>(const num &a, const num &b)
    {
        return detail::compare(a.to_numview(), b.to_numview()) > 0;
    }
    static inline bool operator>=(const num &a, const num &b)
    {
        return detail::compare(a.to_numview(), b.to_numview()) >= 0;
    }

    static inline num operator-(const num &a)
    {
        return num(detail::negate(a.to_numview()));
    }

    static inline num operator+(const num &a, const num &b)
    {
        MAKE_STACK_TEMPORARY_NUMVIEW(c, detail::add_digit_estimate(a.get_n_digits(), b.get_n_digits()));

        return num(detail::add(c, a.to_numview(), b.to_numview()));
    }

    static inline num operator-(const num &a, const num &b)
    {
        MAKE_STACK_TEMPORARY_NUMVIEW(c, detail::add_digit_estimate(a.get_n_digits(), b.get_n_digits()));

        return num(detail::add(c, a.to_numview(), detail::negate(b.to_numview())));
    }

    static inline num operator*(const num &a, const num &b)
    {
        MAKE_STACK_TEMPORARY_NUMVIEW(c, detail::multiply_digit_estimate(a.get_n_digits(), b.get_n_digits()));

        return num(detail::multiply(c, a.to_numview(), b.to_numview()));
    }

    static inline num operator*(const num &a, int32_t b)
    {
        bool negative = false;
        uint32_t bu = b;
        if(b < 0)
        {
            bu = -b;
            negative = true;
        }

        MAKE_STACK_TEMPORARY_NUMVIEW(c, detail::multiply_digit_estimate(a.get_n_digits(), 1));

        detail::numview res = detail::multiply_with_single_digit(c, a.to_numview(), bu);
        if(negative)
        {
            res = detail::negate(res);
        }
        return num(res);
    }

    static inline num operator/(const num &a, int32_t b)
    {
        bool negative = false;
        uint32_t bu = b;
        if(b < 0)
        {
            bu = -b;
            negative = true;
        }
        MAKE_STACK_TEMPORARY_NUMVIEW(c, detail::quotient_digit_estimate(a.get_n_digits()));

        detail::numview res = detail::divide_by_single_digit(c, a.to_numview(), bu);
        if(negative)
        {
            res = detail::negate(res);
        }
        return num(res);
    }

    static inline num operator*(int32_t a, const num &b)
    {
        return b * a;
    }

    static inline std::ostream &operator<<(std::ostream &os, const num &a)
    {
        uint32_t buf_size = detail::to_string_buffer_estimate(a.get_n_digits());
        char buf[buf_size];
        std::string_view sv = detail::to_string(&buf[buf_size], a.to_numview());
        return (os << sv);
    }

    static inline std::string to_string(const num &a)
    {
        uint32_t buf_size = detail::to_string_buffer_estimate(a.get_n_digits());
        char buf[buf_size];
        std::string_view sv = detail::to_string(&buf[buf_size], a.to_numview());
        return std::string(sv);
    }

    static inline num from_string(const std::string_view sv)
    {
        MAKE_STACK_TEMPORARY_NUMVIEW(c, detail::from_chars_digit_estimate(sv.size()));
        return num(detail::from_chars(c, sv.cbegin(), sv.cend()));
    }

} // namespace rqm

#endif // RQM_NUM_H
