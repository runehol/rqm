#ifndef RQM_NUM_H
#define RQM_NUM_H

#include "rqm/numview.h"
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>

#include "rqm/detail/basic_arithmetic.h"

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
            digit_t *ptr = setup_storage(o.n_digits);
            std::memcpy(ptr, o.digits(), n_digits * sizeof(digit_t));
        }

        num(num &&o) noexcept // move constructor
        {
            signum = o.signum;
            if(o.stored_inline())
            {
                digit_t *ptr = setup_storage(o.n_digits);
                std::memcpy(ptr, o.u.digits_inline, n_digits * sizeof(digit_t));
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
                    std::memcpy(u.digits_inline, o.u.digits_inline, n_digits * sizeof(digit_t));
                } else
                {
                    std::swap(n_digits, o.n_digits);
                    std::swap(u.digits_ptr, o.u.digits_ptr);
                }
            }
            return *this;
        }

        num(int64_t value)
        {
            signum = detail::compare_signum(value, int64_t(0));
            int64_t abs_value = std::abs(value);
            u.digits_inline[0] = abs_value & 0xffffffff;
            u.digits_inline[1] = abs_value >> 32;
            uint32_t digs = 2;
            if(u.digits_inline[1] == 0) digs = 1;
            if(u.digits_inline[0] == 0) digs = 0;
            n_digits = digs;
        }

        num(numview o)
            : signum(o.signum)
        {
            digit_t *ptr = setup_storage(o.n_digits);
            std::memcpy(ptr, o.digits, n_digits * sizeof(digit_t));
        }
        operator numview() const { return numview(n_digits, signum, digits()); }

        int32_t to_int32_t() const
        {
            uint32_t v = u.digits_inline[0];
            if(n_digits > 1 || v >= 1u << 31) throw std::overflow_error("Out of range integer");

            return v * signum;
        }

        bool operator==(const num &o) const { return detail::compare(*this, o) == 0; }
        bool operator!=(const num &o) const { return detail::compare(*this, o) != 0; }
        bool operator<(const num &o) const { return detail::compare(*this, o) < 0; }
        bool operator<=(const num &o) const { return detail::compare(*this, o) <= 0; }
        bool operator>(const num &o) const { return detail::compare(*this, o) > 0; }
        bool operator>=(const num &o) const { return detail::compare(*this, o) >= 0; }

    private:
        uint32_t *setup_storage(size_t _n_digits)
        {
            n_digits = _n_digits;
            if(_n_digits <= n_inline_digits)
            {
                return u.digits_inline;
            } else
            {
                return (u.digits_ptr = new digit_t[_n_digits]);
            }
        }

        static constexpr uint32_t n_inline_digits = sizeof(digit_t *) / sizeof(digit_t);

        bool stored_inline() const { return n_digits <= n_inline_digits; }
        const digit_t *digits() const { return stored_inline() ? u.digits_inline : u.digits_ptr; }

        uint32_t n_digits;
        signum_t signum;
        union
        {
            digit_t *digits_ptr;
            digit_t digits_inline[n_inline_digits];
        } u;
    };

} // namespace rqm

#endif // RQM_NUM_H
