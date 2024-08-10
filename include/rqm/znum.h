#ifndef RQM_ZNUM_H
#define RQM_ZNUM_H

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
    class znum
    {
    public:
        class empty_with_n_digits
        {};

        // this class allocates, so we need the rule of five
        znum()
            : n_digits(0),
              is_stored_inline(true),
              signum(0)
        {}

        ~znum()

        {
            if(!stored_inline())
            {
                delete[] u.digits_ptr;
            }
        }

        znum(empty_with_n_digits, uint32_t _n_digits)
        {
            setup_storage(_n_digits);
            signum = 0;
        }

        znum(const znum &o) // copy constructor
        {
            signum = o.signum;
            digit_t *ptr = setup_storage(o.n_digits);
            std::memcpy(ptr, o.digits(), n_digits * sizeof(digit_t));
        }

        znum(znum &&o) noexcept // move constructor
        {
            signum = o.signum;
            is_stored_inline = o.stored_inline();
            if(is_stored_inline)
            {
                digit_t *ptr = setup_storage(o.n_digits);
                std::memcpy(ptr, o.u.digits_inline, n_digits * sizeof(digit_t));
            } else
            {

                n_digits = o.n_digits;
                u.digits_ptr = std::exchange(o.u.digits_ptr, nullptr);
            }
        }

        znum &operator=(const znum &o) // copy assignment
        {
            return *this = znum(o);
        }

        znum &operator=(znum &&o) noexcept // move assignment
        {
            if(&o != this)
            {
                signum = o.signum;
                if(!stored_inline())
                {
                    delete[] u.digits_ptr; // deallocate
                }
                u.digits_ptr = nullptr;

                is_stored_inline = o.stored_inline();
                if(is_stored_inline)
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

        explicit znum(numview o);

        numview to_numview() const;

        znum(int64_t value);
        int64_t to_int64_t() const;

        uint32_t get_n_digits() const { return n_digits; }

        void update_signum_n_digits(numview o);

        signum_t get_signum() const { return signum; }

    private:
        uint32_t *setup_storage(size_t _n_digits)
        {
            n_digits = _n_digits;
            is_stored_inline = _n_digits <= n_inline_digits;
            if(is_stored_inline)
            {
                return u.digits_inline;
            } else
            {
                return (u.digits_ptr = new digit_t[_n_digits]);
            }
        }

        static constexpr uint32_t n_inline_digits = 6;

        bool stored_inline() const { return is_stored_inline; }
        const digit_t *digits() const { return stored_inline() ? u.digits_inline : u.digits_ptr; }

        uint32_t n_digits;
        bool is_stored_inline;
        int16_t signum;
        union
        {
            digit_t *digits_ptr;
            digit_t digits_inline[n_inline_digits];
        } u;
    };

    znum abs(const znum &a);

    bool operator==(const znum &a, const znum &b);
    bool operator!=(const znum &a, const znum &b);
    bool operator<(const znum &a, const znum &b);
    bool operator<=(const znum &a, const znum &b);
    bool operator>(const znum &a, const znum &b);
    bool operator>=(const znum &a, const znum &b);

    znum operator-(const znum &a);
    znum operator+(const znum &a, const znum &b);
    znum operator-(const znum &a, const znum &b);
    znum operator*(const znum &a, const znum &b);
    znum operator*(const znum &a, int32_t b);
    znum operator*(int32_t a, const znum &b);
    znum operator/(const znum &a, int32_t b);
    int64_t operator%(const znum &a, int32_t b);
    znum operator/(const znum &a, const znum &b);
    znum operator<<(const znum &a, uint32_t b);
    znum operator>>(const znum &a, uint32_t b);

    // simple inline implementations of the pre/post increment/decrement operators. could be optimised further if necessary
    static inline znum &operator++(znum &a)
    {
        a = a + 1;
        return a;
    }
    static inline znum &operator--(znum &a)
    {
        a = a - 1;
        return a;
    }
    static inline znum operator++(znum &a, int)
    {
        return a + 1;
    }
    static inline znum operator--(znum &a, int)
    {
        return a - 1;
    }

    static inline bool operator!(const znum &a)
    {
        return a.get_signum() == 0;
    }

    std::ostream &operator<<(std::ostream &os, const znum &a);
    std::string to_string(const znum &a);
    znum from_string(const std::string_view sv);

    uint32_t countr_zero(const znum &v);

    znum gcd(const znum &a, const znum &b);

} // namespace rqm

#endif // RQM_ZNUM_H
