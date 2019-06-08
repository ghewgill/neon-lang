#include "number.h"

#include <iso646.h>

#include "rtl_exec.h"

namespace rtl {

namespace ne_math {

Number abs(Number x)
{
    return number_abs(x);
}

Number acos(Number x)
{
    return number_acos(x);
}

Number acosh(Number x)
{
    return number_acosh(x);
}

Number asin(Number x)
{
    return number_asin(x);
}

Number asinh(Number x)
{
    return number_asinh(x);
}

Number atan(Number x)
{
    return number_atan(x);
}

Number atanh(Number x)
{
    return number_atanh(x);
}

Number atan2(Number y, Number x)
{
    return number_atan2(y, x);
}

Number cbrt(Number x)
{
    return number_cbrt(x);
}

Number ceil(Number x)
{
    return number_ceil(x);
}

Number cos(Number x)
{
    return number_cos(x);
}

Number cosh(Number x)
{
    return number_cosh(x);
}

Number erf(Number x)
{
    return number_erf(x);
}

Number erfc(Number x)
{
    return number_erfc(x);
}

Number exp(Number x)
{
    return number_exp(x);
}

Number exp2(Number x)
{
    return number_exp2(x);
}

Number expm1(Number x)
{
    return number_expm1(x);
}

Number floor(Number x)
{
    return number_floor(x);
}

Number frexp(Number x, Number *exp)
{
    int iexp;
    Number r = number_frexp(x, &iexp);
    *exp = number_from_sint32(iexp);
    return r;
}

Number hypot(Number x, Number y)
{
    return number_hypot(x, y);
}

Number intdiv(Number x, Number y)
{
    return number_trunc(number_divide(x, y));
}

Number ldexp(Number x, Number exp)
{
    if (not number_is_integer(exp)) {
        // TODO: more specific exception?
        throw RtlException(ne_global::Exception_ValueRangeException, utf8string(number_to_string(exp)));
    }
    return number_ldexp(x, number_to_sint32(exp));
}

Number lgamma(Number x)
{
    return number_lgamma(x);
}

Number log(Number x)
{
    return number_log(x);
}

Number log10(Number x)
{
    return number_log10(x);
}

Number log1p(Number x)
{
    return number_log1p(x);
}

Number log2(Number x)
{
    return number_log2(x);
}

Number nearbyint(Number x)
{
    return number_nearbyint(x);
}

Number sign(Number x)
{
    return number_sign(x);
}

Number sin(Number x)
{
    return number_sin(x);
}

Number sinh(Number x)
{
    return number_sinh(x);
}

Number sqrt(Number x)
{
    if (number_is_negative(x)) {
        throw RtlException(ne_global::Exception_ValueRangeException, utf8string(number_to_string(x)));
    }
    return number_sqrt(x);
}

Number tan(Number x)
{
    return number_tan(x);
}

Number tanh(Number x)
{
    return number_tanh(x);
}

Number tgamma(Number x)
{
    return number_tgamma(x);
}

Number trunc(Number x)
{
    return number_trunc(x);
}

} // namespace ne_math

} // namespace rtl
