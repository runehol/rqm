#include "rqm/rnum.h"
#include <cstdint>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "basic_arithmetic.h"
#include "numview.h"
#include "string_conversion.h"

namespace rqm
{
    rnum::rnum(znum nom, znum denom)
        : nominator(std::move(nom)),
          denominator(std::move(denom))
    {
        canonicalize();
    }

    rnum::rnum(int64_t nom, int64_t denom)
        : nominator(nom),
          denominator(denom)
    {
        canonicalize();
    }

    // method that places the number in canonical form (gcd(nom, denom) = 1)
    void rnum::canonicalize()
    {
        if(denominator.get_signum() == 0) throw std::out_of_range("divide by zero");
        if(denominator.get_signum() == -1)
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

    rnum abs(const rnum &a)
    {
        return rnum(abs(a.nom()), a.denom());
    }

    // these two rely on the rnums being in canonical form - gcd(nom, denom) == 1, nom >= 1
    bool operator==(const rnum &a, const rnum &b)
    {
        return a.nom() == b.nom() && a.denom() == b.denom();
    }
    bool operator!=(const rnum &a, const rnum &b)
    {
        return a.nom() != b.nom() || a.denom() != b.denom();
    }

    bool operator<(const rnum &a, const rnum &b)
    {
        return a.nom() * b.denom() < b.nom() * a.denom();
    }
    bool operator<=(const rnum &a, const rnum &b)
    {
        return a.nom() * b.denom() <= b.nom() * a.denom();
    }

    bool operator>(const rnum &a, const rnum &b)
    {
        return a.nom() * b.denom() > b.nom() * a.denom();
    }
    bool operator>=(const rnum &a, const rnum &b)
    {
        return a.nom() * b.denom() >= b.nom() * a.denom();
    }

    rnum operator-(const rnum &a)
    {
        return rnum(-a.nom(), a.denom());
    }

    rnum operator+(const rnum &a, const rnum &b)
    {
        return rnum(a.nom() * b.denom() + b.nom() * a.denom(), a.denom() * b.denom());
    }

    rnum operator-(const rnum &a, const rnum &b)
    {
        return a + (-b);
    }

    rnum operator*(const rnum &a, const rnum &b)
    {
        return rnum(a.nom() * b.nom(), a.denom() * b.denom());
    }

    rnum operator*(const rnum &a, int32_t b)
    {
        return rnum(a.nom() * b, a.denom());
    }

    rnum operator/(const rnum &a, int32_t b)
    {
        return rnum(a.nom(), a.denom() * b);
    }

    rnum operator/(const rnum &a, const rnum &b)
    {
        return rnum(a.nom() * b.denom(), a.denom() * b.nom());
    }

    rnum operator*(int32_t a, const rnum &b)
    {
        return b * a;
    }

    std::ostream &operator<<(std::ostream &os, const rnum &a)
    {
        os << a.nom() << "/" << a.denom();
        return os;
    }

    std::string to_string(const rnum &a)
    {
        std::stringstream ss;
        ss << a;
        return ss.str();
    }

    rnum rnum::from_string(const std::string_view sv)
    {
        auto slash_pos = sv.find('/');
        if(slash_pos != sv.npos)
        {
            std::string_view str_nom = sv.substr(0, slash_pos);
            std::string_view str_denom = sv.substr(slash_pos + 1, sv.npos);
            return rnum(znum::from_string(str_nom), znum::from_string(str_denom));

        } else
        {
            return rnum(znum::from_string(sv));
        }
    }

} // namespace rqm
