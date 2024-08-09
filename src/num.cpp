#include "rqm/num.h"
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

    num::num(int64_t value)
    {
        signum = compare_signum(value, int64_t(0));
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
        n_digits = digs;
    }

    int64_t num::to_int64_t() const
    {
        uint64_t v = 0;
        if(n_digits > n_inline_digits) throw std::overflow_error("Out of range for an int64_t");

        const digit_t *ptr = digits();
        for(uint32_t idx = 0; idx < n_digits; ++idx)
        {
            v |= uint64_t(ptr[idx]) << (idx * n_bits_in_digit);
        }
        if(v > std::numeric_limits<int64_t>::max()) throw std::overflow_error("Out of range for an int64_t");
        return v * signum;
    }

    num::num(numview o)
        : signum(o.signum)
    {
        digit_t *ptr = setup_storage(o.n_digits);
        std::memcpy(ptr, o.digits, n_digits * sizeof(digit_t));
    }

    void num::update_signum_n_digits(numview o)
    {
        signum = o.signum;
        n_digits = o.n_digits;
        assert(digits() == o.digits);
    }

    numview num::to_numview() const
    {
        return numview(n_digits, signum, digits());
    }

    num abs(const num &a)
    {
        return num(abs(a.to_numview()));
    }

    bool operator==(const num &a, const num &b)
    {
        return compare(a.to_numview(), b.to_numview()) == 0;
    }
    bool operator!=(const num &a, const num &b)
    {
        return compare(a.to_numview(), b.to_numview()) != 0;
    }
    bool operator<(const num &a, const num &b)
    {
        return compare(a.to_numview(), b.to_numview()) < 0;
    }
    bool operator<=(const num &a, const num &b)
    {
        return compare(a.to_numview(), b.to_numview()) <= 0;
    }
    bool operator>(const num &a, const num &b)
    {
        return compare(a.to_numview(), b.to_numview()) > 0;
    }
    bool operator>=(const num &a, const num &b)
    {
        return compare(a.to_numview(), b.to_numview()) >= 0;
    }

    num operator-(const num &a)
    {
        return num(negate(a.to_numview()));
    }

    num operator+(const num &a, const num &b)
    {
        num c(num::empty_with_n_digits(), add_digit_estimate(a.get_n_digits(), b.get_n_digits()));
        c.update_signum_n_digits(add(c.to_numview(), a.to_numview(), b.to_numview()));
        return c;
    }

    num operator-(const num &a, const num &b)
    {
        num c(num::empty_with_n_digits(), add_digit_estimate(a.get_n_digits(), b.get_n_digits()));
        c.update_signum_n_digits(add(c.to_numview(), a.to_numview(), negate(b.to_numview())));
        return c;
    }

    num operator*(const num &a, const num &b)
    {
        num c(num::empty_with_n_digits(), multiply_digit_estimate(a.get_n_digits(), b.get_n_digits()));
        c.update_signum_n_digits(multiply(c.to_numview(), a.to_numview(), b.to_numview()));
        return c;
    }

    num operator*(const num &a, int32_t b)
    {
        bool negative = false;
        uint32_t bu = b;
        if(b < 0)
        {
            bu = -b;
            negative = true;
        }

        num c(num::empty_with_n_digits(), multiply_digit_estimate(a.get_n_digits(), 1));

        numview res = multiply_with_single_digit(c.to_numview(), a.to_numview(), bu);
        if(negative)
        {
            res = negate(res);
        }
        c.update_signum_n_digits(res);
        return c;
    }

    num operator/(const num &a, int32_t b)
    {
        bool negative = false;
        uint32_t bu = b;
        if(b < 0)
        {
            bu = -b;
            negative = true;
        }
        num c(num::empty_with_n_digits(), quotient_digit_estimate(a.get_n_digits(), 1));

        numview res = divide_by_single_digit(c.to_numview(), a.to_numview(), bu);
        if(negative)
        {
            res = negate(res);
        }
        c.update_signum_n_digits(res);
        return c;
    }

    num operator/(const num &a, const num &b)
    {
        num c(num::empty_with_n_digits(), quotient_digit_estimate(a.get_n_digits(), 1));
        c.update_signum_n_digits(divmod(c.to_numview(), nullptr, a.to_numview(), b.to_numview()));
        return c;
    }

    num operator*(int32_t a, const num &b)
    {
        return b * a;
    }

    num operator<<(const num &a, uint32_t b)
    {
        num c(num::empty_with_n_digits(), shift_left_digit_estimate(a.get_n_digits(), b));
        c.update_signum_n_digits(shift_left(c.to_numview(), a.to_numview(), b));
        return c;
    }

    num operator>>(const num &a, uint32_t b)
    {
        num c(num::empty_with_n_digits(), shift_right_digit_estimate(a.get_n_digits(), b));
        c.update_signum_n_digits(shift_right(c.to_numview(), a.to_numview(), b));
        return c;
    }

    std::ostream &operator<<(std::ostream &os, const num &a)
    {
        uint32_t buf_size = to_string_buffer_estimate(a.get_n_digits());
        char buf[buf_size];
        std::string_view sv = to_string(&buf[buf_size], a.to_numview());
        return (os << sv);
    }

    std::string to_string(const num &a)
    {
        uint32_t buf_size = to_string_buffer_estimate(a.get_n_digits());
        char buf[buf_size];
        std::string_view sv = to_string(&buf[buf_size], a.to_numview());
        return std::string(sv);
    }

    num from_string(const std::string_view sv)
    {
        num c(num::empty_with_n_digits(), from_chars_digit_estimate(sv.size()));

        c.update_signum_n_digits(from_chars(c.to_numview(), sv.cbegin(), sv.cend()));
        return c;
    }

    uint32_t countr_zero(const num &v)
    {
        return countr_zero(v.to_numview());
    }

} // namespace rqm
