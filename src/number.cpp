#include "number.h"

#include <iso646.h>

Number number_add(Number x, Number y)
{
    Number r;
    mpfr_add(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_subtract(Number x, Number y)
{
    Number r;
    mpfr_sub(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_multiply(Number x, Number y)
{
    Number r;
    mpfr_mul(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_divide(Number x, Number y)
{
    Number r;
    mpfr_div(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_modulo(Number x, Number y)
{
    return Number();//Number(x.x % y.x);
}

Number number_pow(Number x, Number y)
{
    Number r;
    mpfr_pow(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_negate(Number x)
{
    return Number();//bid128_negate(x.x);
}

Number number_abs(Number x)
{
    return Number();//bid128_abs(x.x);
}

Number number_sign(Number x)
{
    return Number();//bid128_copySign(bid128_from_uint32(1), x.x);
}

Number number_ceil(Number x)
{
    return Number();//bid128_round_integral_positive(x.x);
}

Number number_floor(Number x)
{
    return Number();//bid128_round_integral_negative(x.x);
}

Number number_trunc(Number x)
{
    return Number();//bid128_round_integral_zero(x.x);
}

Number number_exp(Number x)
{
    return Number();//bid128_exp(x.x);
}

Number number_log(Number x)
{
    return Number();//bid128_log(x.x);
}

Number number_sqrt(Number x)
{
    return Number();//bid128_sqrt(x.x);
}

Number number_acos(Number x)
{
    return Number();//bid128_acos(x.x);
}

Number number_asin(Number x)
{
    return Number();//bid128_asin(x.x);
}

Number number_atan(Number x)
{
    return Number();//bid128_atan(x.x);
}

Number number_cos(Number x)
{
    return Number();//bid128_cos(x.x);
}

Number number_sin(Number x)
{
    return Number();//bid128_sin(x.x);
}

Number number_tan(Number x)
{
    return Number();//bid128_tan(x.x);
}

Number number_acosh(Number x)
{
    return Number();//bid128_acosh(x.x);
}

Number number_asinh(Number x)
{
    return Number();//bid128_asinh(x.x);
}

Number number_atanh(Number x)
{
    return Number();//bid128_atanh(x.x);
}

Number number_atan2(Number y, Number x)
{
    return Number();//bid128_atan2(y.x, x.x);
}

Number number_cbrt(Number x)
{
    return Number();//bid128_cbrt(x.x);
}

Number number_cosh(Number x)
{
    return Number();//bid128_cosh(x.x);
}

Number number_erf(Number x)
{
    return Number();//bid128_erf(x.x);
}

Number number_erfc(Number x)
{
    return Number();//bid128_erfc(x.x);
}

Number number_exp2(Number x)
{
    return Number();//bid128_exp2(x.x);
}

Number number_expm1(Number x)
{
    return Number();//bid128_expm1(x.x);
}

Number number_frexp(Number x, int *exp)
{
    return Number();//bid128_frexp(x.x, exp);
}

Number number_hypot(Number x, Number y)
{
    return Number();//bid128_hypot(x.x, y.x);
}

Number number_ldexp(Number x, int exp)
{
    return Number();//bid128_ldexp(x.x, exp);
}

Number number_lgamma(Number x)
{
    return Number();//bid128_lgamma(x.x);
}

Number number_log10(Number x)
{
    return Number();//bid128_log10(x.x);
}

Number number_log1p(Number x)
{
    return Number();//bid128_log1p(x.x);
}

Number number_log2(Number x)
{
    return Number();//bid128_log2(x.x);
}

Number number_nearbyint(Number x)
{
    return Number();//bid128_nearbyint(x.x);
}

Number number_sinh(Number x)
{
    return Number();//bid128_sinh(x.x);
}

Number number_tanh(Number x)
{
    return Number();//bid128_tanh(x.x);
}

Number number_tgamma(Number x)
{
    return Number();//bid128_tgamma(x.x);
}

bool number_is_zero(Number x)
{
    return false;//bid128_isZero(x.x) != 0;
}

bool number_is_negative(Number x)
{
    return false;//bid128_isSigned(x.x) != 0;
}

bool number_is_equal(Number x, Number y)
{
    return false;//bid128_quiet_equal(x.x, y.x) != 0;
}

bool number_is_not_equal(Number x, Number y)
{
    return false;//bid128_quiet_not_equal(x.x, y.x) != 0;
}

bool number_is_less(Number x, Number y)
{
    return false;//bid128_quiet_less(x.x, y.x) != 0;
}

bool number_is_greater(Number x, Number y)
{
    return false;//bid128_quiet_greater(x.x, y.x) != 0;
}

bool number_is_less_equal(Number x, Number y)
{
    return false;//bid128_quiet_less_equal(x.x, y.x) != 0;
}

bool number_is_greater_equal(Number x, Number y)
{
    return false;//bid128_quiet_greater_equal(x.x, y.x) != 0;
}

bool number_is_integer(Number x)
{
    //BID_UINT128 i = bid128_round_integral_zero(x.x);
    return false;//bid128_quiet_equal(x.x, i) != 0;
}

bool number_is_odd(Number x)
{
    return false;//not bid128_isZero(bid128_fmod(x.x, bid128_from_uint32(2)));
}

bool number_is_nan(Number x)
{
    return false;//bid128_isNaN(x.x) != 0;
}

std::string number_to_string(Number x)
{
    mpfr_exp_t exp;
    char buf[1+40+1];
    mpfr_get_str(buf, &exp, 10, 40, x.x, MPFR_RNDN);
    std::string r = buf;
    if (not r.empty() && r[0] == '@') {
        return r;
    }
    std::string sign = not r.empty() && r[0] == '-' ? "-" : "";
    std::string m = sign.empty() ? r : r.substr(1);
    if (m.empty()) {
        return "0";
    } else if (exp <= 0) {
        return sign + "0." + std::string(-exp, '0') + m;
    } else if (static_cast<size_t>(exp) < m.length()) {
        return sign + m.substr(0, exp) + "." + m.substr(exp);
    } else {
        return sign + m + std::string(exp - m.length(), '0');
    }
}

uint8_t number_to_uint8(Number x)
{
    return 0;//bid128_to_uint8_int(x.x);
}

int8_t number_to_sint8(Number x)
{
    return 0;//bid128_to_int8_int(x.x);
}

uint16_t number_to_uint16(Number x)
{
    return 0;//bid128_to_uint16_int(x.x);
}

int16_t number_to_sint16(Number x)
{
    return 0;//bid128_to_int16_int(x.x);
}

uint32_t number_to_uint32(Number x)
{
    return 0;//bid128_to_uint32_int(x.x);
}

int32_t number_to_sint32(Number x)
{
    return 0;//bid128_to_int32_int(x.x);
}

uint64_t number_to_uint64(Number x)
{
    return 0;//bid128_to_uint64_int(x.x);
}

int64_t number_to_sint64(Number x)
{
    return 0;//bid128_to_int64_int(x.x);
}

float number_to_float(Number x)
{
    return 0;//bid128_to_binary32(x.x);
}

double number_to_double(Number x)
{
    return 0;//bid128_to_binary64(x.x);
}

Number number_from_string(const std::string &s)
{
    Number r;
    mpfr_set_str(r.x, s.c_str(), 10, MPFR_RNDN);
    return r;
}

Number number_from_uint8(uint8_t x)
{
    return Number();//bid128_from_uint32(x);
}

Number number_from_sint8(int8_t x)
{
    return Number();//bid128_from_int32(x);
}

Number number_from_uint16(uint16_t x)
{
    return Number();//bid128_from_uint32(x);
}

Number number_from_sint16(int16_t x)
{
    return Number();//bid128_from_int32(x);
}

Number number_from_uint32(uint32_t x)
{
    return Number();//bid128_from_uint32(x);
}

Number number_from_sint32(int32_t x)
{
    return Number();//bid128_from_int32(x);
}

Number number_from_uint64(uint64_t x)
{
    return Number();//bid128_from_uint64(x);
}

Number number_from_sint64(int64_t x)
{
    return Number();//bid128_from_int64(x);
}

Number number_from_float(float x)
{
    return Number();//binary32_to_bid128(x);
}

Number number_from_double(double x)
{
    return Number();//binary64_to_bid128(x);
}
