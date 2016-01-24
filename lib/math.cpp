#include "number.h"

#include <iso646.h>

#include "rtl_exec.h"

namespace rtl {

Number math$abs(Number x)
{
    return number_abs(x);
}

Number math$acos(Number x)
{
    return number_acos(x);
}

Number math$acosh(Number x)
{
    return number_acosh(x);
}

Number math$asin(Number x)
{
    return number_asin(x);
}

Number math$asinh(Number x)
{
    return number_asinh(x);
}

Number math$atan(Number x)
{
    return number_atan(x);
}

Number math$atanh(Number x)
{
    return number_atanh(x);
}

Number math$atan2(Number y, Number x)
{
    return number_atan2(y, x);
}

Number math$cbrt(Number x)
{
    return number_cbrt(x);
}

Number math$ceil(Number x)
{
    return number_ceil(x);
}

Number math$cos(Number x)
{
    return number_cos(x);
}

Number math$cosh(Number x)
{
    return number_cosh(x);
}

Number math$erf(Number x)
{
    return number_erf(x);
}

Number math$erfc(Number x)
{
    return number_erfc(x);
}

Number math$exp(Number x)
{
    return number_exp(x);
}

Number math$exp2(Number x)
{
    return number_exp2(x);
}

Number math$expm1(Number x)
{
    return number_expm1(x);
}

Number math$floor(Number x)
{
    return number_floor(x);
}

Number math$frexp(Number x, Number *exp)
{
    int iexp;
    Number r = number_frexp(x, &iexp);
    *exp = number_from_sint32(iexp);
    return r;
}

Number math$hypot(Number x, Number y)
{
    return number_hypot(x, y);
}

Number math$ldexp(Number x, Number exp)
{
    if (not number_is_integer(exp)) {
        // TODO: more specific exception?
        throw RtlException(Exception_global$ValueRangeException, number_to_string(exp));
    }
    return number_ldexp(x, number_to_sint32(exp));
}

Number math$lgamma(Number x)
{
    return number_lgamma(x);
}

Number math$log(Number x)
{
    return number_log(x);
}

Number math$log10(Number x)
{
    return number_log10(x);
}

Number math$log1p(Number x)
{
    return number_log1p(x);
}

Number math$log2(Number x)
{
    return number_log2(x);
}

Number math$nearbyint(Number x)
{
    return number_nearbyint(x);
}

Number math$sign(Number x)
{
    return number_sign(x);
}

Number math$sin(Number x)
{
    return number_sin(x);
}

Number math$sinh(Number x)
{
    return number_sinh(x);
}

Number math$sqrt(Number x)
{
    return number_sqrt(x);
}

Number math$tan(Number x)
{
    return number_tan(x);
}

Number math$tanh(Number x)
{
    return number_tanh(x);
}

Number math$tgamma(Number x)
{
    return number_tgamma(x);
}

Number math$trunc(Number x)
{
    return number_trunc(x);
}

} // namespace rtl
