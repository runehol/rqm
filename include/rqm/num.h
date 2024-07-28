#ifndef RQM_NUM_H
#define RQM_NUM_H

#include <cstdint>
#include <cstring>
#include <iosfwd>
#include <string>
#include <string_view>
#include <utility>

#include "rqm/digit.h"
namespace rqm
{

    struct numview;
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

        explicit num(numview o);

        numview to_numview() const;

        num(int64_t value);
        int64_t to_int64_t() const;

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
                return (u.digits_ptr = new digit_t[_n_digits]);
            }
        }

        static constexpr uint32_t n_inline_digits = 2;

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

    num abs(const num &a);

    bool operator==(const num &a, const num &b);
    bool operator!=(const num &a, const num &b);
    bool operator<(const num &a, const num &b);
    bool operator<=(const num &a, const num &b);
    bool operator>(const num &a, const num &b);
    bool operator>=(const num &a, const num &b);

    num operator-(const num &a);
    num operator+(const num &a, const num &b);
    num operator-(const num &a, const num &b);
    num operator*(const num &a, const num &b);
    num operator*(const num &a, int32_t b);
    num operator*(int32_t a, const num &b);
    num operator/(const num &a, int32_t b);

    std::ostream &operator<<(std::ostream &os, const num &a);
    std::string to_string(const num &a);
    num from_string(const std::string_view sv);

} // namespace rqm

#endif // RQM_NUM_H
