#ifndef RQM_CONVERT_RNUM_TO_FLOATING_POINT_H
#define RQM_CONVERT_RNUM_TO_FLOATING_POINT_H

#include "rqm/rnum.h"
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace rqm
{

    /* converting rational numbers to floating-point.

       Strategy

       we have a rational number of infinite precision expressed as nom / denom,
       and we want to convert this with correct rounding to a floating-point number.
       To do so, we will go set bit by set bit by comparing nom/denom against 1<<posexp/1<<negexp.

       nom / denom >= 1<<posexp / 1<<negexp is the same as nom<<negexp >= denom<<posexp.

       once we've found a bit, we subtract it off. this is done by:

       nom' / denom' = nom / denom - 1<<posexp/1<<negexp
       = nom<<negexp - denom<<posexp / denom<<negexp

       i.e if exp is positive:
       nom' = nom - denom<<posexp
       denom' = denom
       and if exp is negative:
       nom' = nom<<negexp - denom
       denom = denom<<negexp


       This is a pretty solid way to figure out the mantissa, but we also need a way to discover the exponent, without scanning too many possible exponent positions.

       if we start with our previous formula,
       nom<<negexp >= denom<<posexp.

       take the log2 of this, and we get log2(nom) + negexp >= log2(denom) + posexp, and rearranging the terms:
       log2(nom) - log2(denom) >= posexp - negexp
       log2(nom) - log2(denom) >= exp

       our n_bits query is a fast quantized log2, which gives a rough indication. however, it may be two positions off due to rounding effects.



     */

    static signum_t compare_with_pow2(const rnum &v, int32_t exponent)
    {
        int32_t posexp = std::max<int32_t>(exponent, 0);
        int32_t negexp = std::max<int32_t>(-exponent, 0);

        if(posexp > 0)
        {
            return compare(v.nom(), v.denom() << posexp);
        } else if(negexp > 0)
        {
            return compare(v.nom() << negexp, v.denom());
        } else
        {
            return compare(v.nom(), v.denom());
        }
    }

    static rnum subtract_pow2(const rnum &v, int32_t exponent)
    {
        int32_t posexp = std::max<int32_t>(exponent, 0);
        int32_t negexp = std::max<int32_t>(-exponent, 0);

        if(posexp > 0)
        {
            return rnum(v.nom() - (v.denom() << posexp), v.denom());
        } else if(negexp > 0)
        {
            return rnum((v.nom() << negexp) - v.denom(), v.denom() << negexp);
        } else
        {
            return rnum(v.nom() - v.denom(), v.denom());
        }
    }

    template<uint32_t n_exponent_bits, uint32_t n_mantissa_bits>
    uint64_t compose_float(uint64_t sign, uint64_t exponent, uint64_t mantissa)
    {
        constexpr uint32_t sign_pos = n_exponent_bits + n_mantissa_bits;
        constexpr uint32_t exponent_pos = n_mantissa_bits;
        return ((sign & 1) << sign_pos) | ((exponent & ((1ull << n_exponent_bits) - 1)) << exponent_pos) | (mantissa & ((1ull << n_mantissa_bits) - 1));
    }

    template<uint32_t n_exponent_bits, uint32_t n_mantissa_bits>
    uint64_t convert_rnum_to_floating_point(const rnum &_v)
    {
        constexpr int32_t max_exponent = (1ull << n_exponent_bits) - 1;
        constexpr int32_t exp_bias = (1ull << (n_exponent_bits - 1)) - 1;
        if(_v.signum() == 0) return compose_float<n_exponent_bits, n_mantissa_bits>(0, 0, 0);
        bool sign = _v.signum() < 0;
        rnum v = abs(_v);

        int32_t exponent_estimate = v.nom().n_bits() - v.denom().n_bits() + 2; // estimate the first set bit. add two, as n_bits is an integer approximation to log2 and may be off by 1.
        int32_t exponent;
        for(exponent = exponent_estimate;; --exponent)
        {
            if(compare_with_pow2(v, exponent) >= 0)
            {
                // bingo
                v = subtract_pow2(v, exponent); // and subtract
                break;
            }
        }
        if(exponent + exp_bias >= max_exponent)
        {
            // infinity
            return compose_float<n_exponent_bits, n_mantissa_bits>(sign, max_exponent, 0);
        }

        if(exponent + exp_bias <= 0)
        {
            exponent = -exp_bias; // we're in denormal land. set the exponent to biased zero, and proceed.
        }

        uint64_t mantissa = 0;
        for(int32_t pos = n_mantissa_bits - 1; pos >= 0; --pos)
        {
            int32_t e = exponent + pos - n_mantissa_bits;
            signum_t comparison = compare_with_pow2(v, e);
            if(comparison >= 0)
            {
                mantissa |= (1ull << pos);
                v = subtract_pow2(v, e);
            }
        }

        /* round to nearest even. */
        signum_t half = compare_with_pow2(v, exponent - n_mantissa_bits - 1);
        if(half > 0)
        {
            /* if > 0.5ulp */
            ++mantissa;
        } else if(half == 0 && (mantissa & 1) == 1)
        {
            /* if == 0.5ulp, and the last bit of the mantissa is odd - round up to even */
            ++mantissa;
        }
        constexpr uint64_t beyond_mantissa_bit = 1ull << n_mantissa_bits;
        if(mantissa & beyond_mantissa_bit)
        {
            // we've rounded up past the max mantissa, increment exponent to adjust
            ++exponent;
            mantissa &= beyond_mantissa_bit - 1;
        }

        if(exponent + exp_bias >= max_exponent)
        {
            // oops, we've overflowed into infinity. fix that up
            return compose_float<n_exponent_bits, n_mantissa_bits>(sign, max_exponent, 0);
        }

        return compose_float<n_exponent_bits, n_mantissa_bits>(sign, exponent + exp_bias, mantissa);
    }
} // namespace rqm

#endif // RQM_CONVERT_RNUM_TO_FLOATING_POINT_H
