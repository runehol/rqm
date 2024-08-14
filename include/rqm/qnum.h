#ifndef RQM_QNUM_H
#define RQM_QNUM_H

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
    class qnum
    {
    public:
        qnum() = default; // makes a zero

        qnum(znum value)
            : nominator(std::move(value)),
              denominator(znum::one())
        {}

        qnum(int64_t value)
            : nominator(znum(value)),
              denominator(znum::one())
        {}

        qnum(znum nom, znum denom);

        qnum(int64_t nom, int64_t denom);

        static qnum from_string(const std::string_view sv);
        static qnum from_double(double value);

        static qnum zero() { return qnum(); }
        static qnum one() { return qnum(znum::one()); }
        static qnum minus_one() { return qnum(znum::minus_one()); }

        signum_t signum() const { return nominator.signum(); }

        const znum &nom() const { return nominator; }
        const znum &denom() const { return denominator; }

        uint32_t n_bits() const { return nominator.n_bits() + denominator.n_bits(); }

    private:
        void canonicalize();

        znum nominator = znum::zero();
        znum denominator = znum::one();
    };

    qnum abs(const qnum &a);

    signum_t compare(const qnum &a, const qnum &b);
    bool operator==(const qnum &a, const qnum &b);
    bool operator!=(const qnum &a, const qnum &b);
    bool operator<(const qnum &a, const qnum &b);
    bool operator<=(const qnum &a, const qnum &b);
    bool operator>(const qnum &a, const qnum &b);
    bool operator>=(const qnum &a, const qnum &b);

    qnum operator-(const qnum &a);
    qnum operator+(const qnum &a, const qnum &b);
    qnum operator-(const qnum &a, const qnum &b);
    qnum operator*(const qnum &a, const qnum &b);
    qnum operator*(const qnum &a, int32_t b);
    qnum operator*(int32_t a, const qnum &b);
    qnum operator/(const qnum &a, int32_t b);
    qnum operator/(const qnum &a, const qnum &b);

    // simple inline implementations of the pre/post increment/decrement operators. could be optimised further if necessary
    static inline qnum &operator++(qnum &a)
    {
        a = a + 1;
        return a;
    }
    static inline qnum &operator--(qnum &a)
    {
        a = a - 1;
        return a;
    }
    static inline qnum operator++(qnum &a, int)
    {
        qnum tmp = a;
        ++a;
        return tmp;
    }
    static inline qnum operator--(qnum &a, int)
    {
        qnum tmp = a;
        --a;
        return tmp;
    }

    static inline bool operator!(const qnum &a)
    {
        return a.signum() == 0;
    }

    std::ostream &operator<<(std::ostream &os, const qnum &a);
    std::string to_string(const qnum &a);
    double to_double(const qnum &a);

} // namespace rqm

#endif // RQM_QNUM_H
