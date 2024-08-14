#include "rqm/qnum.h"
#include <cstdint>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "basic_arithmetic.h"
#include "convert_qnum_to_floating_point.h"
#include "numview.h"
#include "string_conversion.h"

namespace rqm
{
    qnum::qnum(znum nom, znum denom)
        : nominator(std::move(nom)),
          denominator(std::move(denom))
    {
        canonicalize();
    }

    qnum::qnum(int64_t nom, int64_t denom)
        : nominator(nom),
          denominator(denom)
    {
        canonicalize();
    }

    // method that places the number in canonical form (gcd(nom, denom) = 1)
    void qnum::canonicalize()
    {
        if(denominator.signum() == 0) throw std::out_of_range("divide by zero");
        if(denominator.signum() == -1)
        {
            nominator = -nominator;
            denominator = -denominator;
        }

        znum gcd_val = gcd(nominator, denominator);
        if(!gcd_val.is_one())
        {
            nominator = nominator / gcd_val;
            denominator = denominator / gcd_val;
        }
    }

    qnum qnum::from_double(double value)
    {
        if(std::isnan(value)) throw std::out_of_range("cannot represent NaN as a rational number");
        if(std::isinf(value)) throw std::out_of_range("cannot represent infinity as a rational number");

        int exp = 0;
        double fmantissa = frexp(value, &exp);
        // get to integers
        fmantissa *= (1ull << 53);
        exp -= 53;
        int64_t imantissa = std::llrint(fmantissa);
        znum nominator = imantissa;
        znum denominator = znum::one();
        if(exp > 0)
        {
            nominator = nominator << exp;
        } else if(exp < 0)
        {
            denominator = denominator << -exp;
        }
        return qnum(std::move(nominator), std::move(denominator));
    }

    qnum abs(const qnum &a)
    {
        return qnum(abs(a.nom()), a.denom());
    }

    // these two rely on the qnums being in canonical form - gcd(nom, denom) == 1, nom >= 1
    bool operator==(const qnum &a, const qnum &b)
    {
        return a.nom() == b.nom() && a.denom() == b.denom();
    }
    bool operator!=(const qnum &a, const qnum &b)
    {
        return a.nom() != b.nom() || a.denom() != b.denom();
    }

    signum_t compare(const qnum &a, const qnum &b)
    {
        return compare(a.nom() * b.denom(), b.nom() * a.denom());
    }

    bool operator<(const qnum &a, const qnum &b)
    {
        return compare(a, b) < 0;
    }
    bool operator<=(const qnum &a, const qnum &b)
    {
        return compare(a, b) <= 0;
    }

    bool operator>(const qnum &a, const qnum &b)
    {
        return compare(a, b) > 0;
    }
    bool operator>=(const qnum &a, const qnum &b)
    {
        return compare(a, b) >= 0;
    }

    qnum operator-(const qnum &a)
    {
        return qnum(-a.nom(), a.denom());
    }

    qnum operator+(const qnum &a, const qnum &b)
    {
        return qnum(a.nom() * b.denom() + b.nom() * a.denom(), a.denom() * b.denom());
    }

    qnum operator-(const qnum &a, const qnum &b)
    {
        return a + (-b);
    }

    qnum operator*(const qnum &a, const qnum &b)
    {
        return qnum(a.nom() * b.nom(), a.denom() * b.denom());
    }

    qnum operator*(const qnum &a, int32_t b)
    {
        return qnum(a.nom() * b, a.denom());
    }

    qnum operator/(const qnum &a, int32_t b)
    {
        return qnum(a.nom(), a.denom() * b);
    }

    qnum operator/(const qnum &a, const qnum &b)
    {
        return qnum(a.nom() * b.denom(), a.denom() * b.nom());
    }

    qnum operator*(int32_t a, const qnum &b)
    {
        return b * a;
    }

    std::ostream &operator<<(std::ostream &os, const qnum &a)
    {
        os << a.nom() << "/" << a.denom();
        return os;
    }

    std::string to_string(const qnum &a)
    {
        std::stringstream ss;
        ss << a;
        return ss.str();
    }

    qnum qnum::from_string(const std::string_view sv)
    {
        auto slash_pos = sv.find('/');
        if(slash_pos != sv.npos)
        {
            std::string_view str_nom = sv.substr(0, slash_pos);
            std::string_view str_denom = sv.substr(slash_pos + 1, sv.npos);
            return qnum(znum::from_string(str_nom), znum::from_string(str_denom));

        } else
        {
            return qnum(znum::from_string(sv));
        }
    }

    double to_double(const qnum &a)
    {
        uint64_t iresult = convert_qnum_to_floating_point<11, 52>(a);
        double fresult;
        memcpy(&fresult, &iresult, sizeof(double));
        return fresult;
    }

} // namespace rqm
