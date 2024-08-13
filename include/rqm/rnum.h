#ifndef RQM_RNUM_H
#define RQM_RNUM_H

#include <cstdint>
#include <cstring>
#include <iosfwd>
#include <string>
#include <string_view>
#include <utility>

#include "rqm/znum.h"

namespace rqm
{

    struct numview;
    /**
       rational number class

       The value is held as two znums, the nominator and the denominator.
       It is always held in canonical form, that is, gcd(nominator, denominator) = 1, denominator >= 1
    */
    class rnum
    {
    public:
        rnum() = default; // makes a zero

        rnum(znum value)
            : nominator(std::move(value)),
              denominator(znum::one())
        {}

        rnum(int64_t value)
            : nominator(znum(value)),
              denominator(znum::one())
        {}

        rnum(znum nom, znum denom);

        rnum(int64_t nom, int64_t denom);

        static rnum from_string(const std::string_view sv);
        static rnum from_double(double value);

        static rnum zero() { return rnum(); }
        static rnum one() { return rnum(znum::one()); }
        static rnum minus_one() { return rnum(znum::minus_one()); }

        signum_t signum() const { return nominator.signum(); }

        const znum &nom() const { return nominator; }
        const znum &denom() const { return denominator; }

        uint32_t n_bits() const { return nominator.n_bits() + denominator.n_bits(); }

    private:
        void canonicalize();

        znum nominator = znum::zero();
        znum denominator = znum::one();
    };

    rnum abs(const rnum &a);

    signum_t compare(const rnum &a, const rnum &b);
    bool operator==(const rnum &a, const rnum &b);
    bool operator!=(const rnum &a, const rnum &b);
    bool operator<(const rnum &a, const rnum &b);
    bool operator<=(const rnum &a, const rnum &b);
    bool operator>(const rnum &a, const rnum &b);
    bool operator>=(const rnum &a, const rnum &b);

    rnum operator-(const rnum &a);
    rnum operator+(const rnum &a, const rnum &b);
    rnum operator-(const rnum &a, const rnum &b);
    rnum operator*(const rnum &a, const rnum &b);
    rnum operator*(const rnum &a, int32_t b);
    rnum operator*(int32_t a, const rnum &b);
    rnum operator/(const rnum &a, int32_t b);
    rnum operator/(const rnum &a, const rnum &b);

    // simple inline implementations of the pre/post increment/decrement operators. could be optimised further if necessary
    static inline rnum &operator++(rnum &a)
    {
        a = a + 1;
        return a;
    }
    static inline rnum &operator--(rnum &a)
    {
        a = a - 1;
        return a;
    }
    static inline rnum operator++(rnum &a, int)
    {
        rnum tmp = a;
        ++a;
        return tmp;
    }
    static inline rnum operator--(rnum &a, int)
    {
        rnum tmp = a;
        --a;
        return tmp;
    }

    static inline bool operator!(const rnum &a)
    {
        return a.signum() == 0;
    }

    std::ostream &operator<<(std::ostream &os, const rnum &a);
    std::string to_string(const rnum &a);
    double to_double(const rnum &a);

} // namespace rqm

#endif // RQM_RNUM_H
