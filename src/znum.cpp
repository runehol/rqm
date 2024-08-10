#include "rqm/znum.h"
#include <cstdint>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

#include "basic_arithmetic.h"
#include "numview.h"
#include "string_conversion.h"

namespace rqm
{

    znum::znum(int64_t value)
    {
        _signum = compare_signum(value, int64_t(0));
        int64_t abs_value = std::abs(value);
        is_stored_inline = true;
        u.digits_inline[0] = abs_value & 0xffffffff;
        u.digits_inline[1] = abs_value >> 32;
        uint32_t digs = 2;
        if(u.digits_inline[1] == 0)
        {
            digs = 1;
            if(u.digits_inline[0] == 0) digs = 0;
        }
        _n_digits = digs;
    }

    int64_t znum::to_int64_t() const
    {
        uint64_t v = 0;
        if(_n_digits > n_inline_digits) throw std::overflow_error("Out of range for an int64_t");

        const digit_t *ptr = digits();
        for(uint32_t idx = 0; idx < _n_digits; ++idx)
        {
            v |= uint64_t(ptr[idx]) << (idx * n_bits_in_digit);
        }
        if(v > std::numeric_limits<int64_t>::max()) throw std::overflow_error("Out of range for an int64_t");
        return v * _signum;
    }

    znum::znum(numview o)
        : _signum(o.signum)
    {
        digit_t *ptr = setup_storage(o.n_digits);
        std::memcpy(ptr, o.digits, _n_digits * sizeof(digit_t));
    }

    void znum::update_signum_n_digits(numview o)
    {
        _signum = o.signum;
        _n_digits = o.n_digits;
        assert(digits() == o.digits);
    }

    numview znum::to_numview() const
    {
        return numview(_n_digits, _signum, digits());
    }

    znum abs(const znum &a)
    {
        return znum(abs(a.to_numview()));
    }

    bool operator==(const znum &a, const znum &b)
    {
        return compare(a.to_numview(), b.to_numview()) == 0;
    }
    bool operator!=(const znum &a, const znum &b)
    {
        return compare(a.to_numview(), b.to_numview()) != 0;
    }
    bool operator<(const znum &a, const znum &b)
    {
        return compare(a.to_numview(), b.to_numview()) < 0;
    }
    bool operator<=(const znum &a, const znum &b)
    {
        return compare(a.to_numview(), b.to_numview()) <= 0;
    }
    bool operator>(const znum &a, const znum &b)
    {
        return compare(a.to_numview(), b.to_numview()) > 0;
    }
    bool operator>=(const znum &a, const znum &b)
    {
        return compare(a.to_numview(), b.to_numview()) >= 0;
    }

    znum operator-(const znum &a)
    {
        return znum(negate(a.to_numview()));
    }

    znum operator+(const znum &a, const znum &b)
    {
        znum c(znum::empty_with_n_digits(), add_digit_estimate(a.n_digits(), b.n_digits()));
        c.update_signum_n_digits(add(c.to_numview(), a.to_numview(), b.to_numview()));
        return c;
    }

    znum operator-(const znum &a, const znum &b)
    {
        znum c(znum::empty_with_n_digits(), add_digit_estimate(a.n_digits(), b.n_digits()));
        c.update_signum_n_digits(add(c.to_numview(), a.to_numview(), negate(b.to_numview())));
        return c;
    }

    znum operator*(const znum &a, const znum &b)
    {
        znum c(znum::empty_with_n_digits(), multiply_digit_estimate(a.n_digits(), b.n_digits()));
        c.update_signum_n_digits(multiply(c.to_numview(), a.to_numview(), b.to_numview()));
        return c;
    }

    znum operator*(const znum &a, int32_t b)
    {
        bool negative = false;
        uint32_t bu = b;
        if(b < 0)
        {
            bu = -b;
            negative = true;
        }

        znum c(znum::empty_with_n_digits(), multiply_digit_estimate(a.n_digits(), 1));

        numview res = multiply_with_single_digit(c.to_numview(), a.to_numview(), bu);
        if(negative)
        {
            res = negate(res);
        }
        c.update_signum_n_digits(res);
        return c;
    }

    znum operator/(const znum &a, int32_t b)
    {
        bool negative = false;
        uint32_t bu = b;
        if(b < 0)
        {
            bu = -b;
            negative = true;
        }
        znum c(znum::empty_with_n_digits(), quotient_digit_estimate(a.n_digits(), 1));

        numview res = divmod_by_single_digit(c.to_numview(), nullptr, a.to_numview(), bu);
        if(negative)
        {
            res = negate(res);
        }
        c.update_signum_n_digits(res);
        return c;
    }

    int32_t operator%(const znum &a, int32_t b)
    {
        uint32_t bu = b;
        if(b < 0)
        {
            bu = -b;
        }
        MAKE_STACK_TEMPORARY_NUMVIEW(quotient, quotient_digit_estimate(a.n_digits(), 1));
        int64_t modulo = 0;
        quotient = divmod_by_single_digit(quotient, &modulo, a.to_numview(), bu);
        return modulo;
    }

    znum operator/(const znum &a, const znum &b)
    {
        znum c(znum::empty_with_n_digits(), quotient_digit_estimate(a.n_digits(), 1));
        c.update_signum_n_digits(divmod(c.to_numview(), nullptr, a.to_numview(), b.to_numview()));
        return c;
    }

    znum operator%(const znum &a, const znum &b)
    {
        MAKE_STACK_TEMPORARY_NUMVIEW(quotient, quotient_digit_estimate(a.n_digits(), b.n_digits()));
        znum c(znum::empty_with_n_digits(), modulo_digit_estimate(a.n_digits(), b.n_digits()));

        numview modulo = c.to_numview();
        quotient = divmod(quotient, &modulo, a.to_numview(), b.to_numview());
        c.update_signum_n_digits(modulo);
        return c;
    }

    znum operator*(int32_t a, const znum &b)
    {
        return b * a;
    }

    znum operator<<(const znum &a, uint32_t b)
    {
        znum c(znum::empty_with_n_digits(), shift_left_digit_estimate(a.n_digits(), b));
        c.update_signum_n_digits(shift_left(c.to_numview(), a.to_numview(), b));
        return c;
    }

    znum operator>>(const znum &a, uint32_t b)
    {
        znum c(znum::empty_with_n_digits(), shift_right_digit_estimate(a.n_digits(), b));
        c.update_signum_n_digits(shift_right(c.to_numview(), a.to_numview(), b));
        return c;
    }

    std::ostream &operator<<(std::ostream &os, const znum &a)
    {
        uint32_t buf_size = to_string_buffer_estimate(a.n_digits());
        char buf[buf_size];
        std::string_view sv = to_string(&buf[buf_size], a.to_numview());
        return (os << sv);
    }

    std::string to_string(const znum &a)
    {
        uint32_t buf_size = to_string_buffer_estimate(a.n_digits());
        char buf[buf_size];
        std::string_view sv = to_string(&buf[buf_size], a.to_numview());
        return std::string(sv);
    }

    znum znum::from_string(const std::string_view sv)
    {
        znum c(znum::empty_with_n_digits(), from_chars_digit_estimate(sv.size()));

        c.update_signum_n_digits(from_chars(c.to_numview(), sv.cbegin(), sv.cend()));
        return c;
    }

    uint32_t countr_zero(const znum &v)
    {
        return countr_zero(v.to_numview());
    }

    znum gcd(const znum &a, const znum &b)
    {
        znum c(znum::empty_with_n_digits(), gcd_digit_estimate(a.n_digits(), b.n_digits()));
        c.update_signum_n_digits(binary_gcd(c.to_numview(), a.to_numview(), b.to_numview()));
        return c;
    }

} // namespace rqm
