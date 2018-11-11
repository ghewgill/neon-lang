#include "number.h"

#include <iso646.h>

class Initialiser {
public:
    Initialiser() {
        mpfr_set_default_prec(128);
    }
} initialiser;

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
    Number r;
    mpfr_fmod(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_pow(Number x, Number y)
{
    Number r;
    mpfr_pow(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_negate(Number x)
{
    Number r;
    mpfr_neg(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_abs(Number x)
{
    Number r;
    mpfr_abs(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_sign(Number x)
{
    Number r;
    int s = mpfr_sgn(x.x);
    if (s > 0) {
        mpfr_set_si(r.x, 1, MPFR_RNDN);
    } else if (s < 0) {
        mpfr_set_si(r.x, -1, MPFR_RNDN);
    } else {
        mpfr_set_si(r.x, 0, MPFR_RNDN);
    }
    return r;
}

Number number_ceil(Number x)
{
    Number r;
    mpfr_ceil(r.x, x.x);
    return r;
}

Number number_floor(Number x)
{
    Number r;
    mpfr_floor(r.x, x.x);
    return r;
}

Number number_trunc(Number x)
{
    Number r;
    mpfr_trunc(r.x, x.x);
    return r;
}

Number number_exp(Number x)
{
    Number r;
    mpfr_exp(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_log(Number x)
{
    Number r;
    mpfr_log(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_sqrt(Number x)
{
    Number r;
    mpfr_sqrt(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_acos(Number x)
{
    Number r;
    mpfr_acos(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_asin(Number x)
{
    Number r;
    mpfr_asin(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_atan(Number x)
{
    Number r;
    mpfr_atan(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_cos(Number x)
{
    Number r;
    mpfr_cos(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_sin(Number x)
{
    Number r;
    mpfr_sin(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_tan(Number x)
{
    Number r;
    mpfr_tan(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_acosh(Number x)
{
    Number r;
    mpfr_acosh(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_asinh(Number x)
{
    Number r;
    mpfr_asinh(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_atanh(Number x)
{
    Number r;
    mpfr_atanh(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_atan2(Number y, Number x)
{
    Number r;
    mpfr_atan2(r.x, y.x, x.x, MPFR_RNDN);
    return r;
}

Number number_cbrt(Number x)
{
    Number r;
    mpfr_cbrt(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_cosh(Number x)
{
    Number r;
    mpfr_cosh(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_erf(Number x)
{
    Number r;
    mpfr_erf(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_erfc(Number x)
{
    Number r;
    mpfr_erfc(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_exp2(Number x)
{
    Number r;
    mpfr_exp2(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_expm1(Number x)
{
    Number r;
    mpfr_expm1(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_frexp(Number x, int *exp)
{
    return Number();//bid128_frexp(x.x, exp);
}

Number number_hypot(Number x, Number y)
{
    Number r;
    mpfr_hypot(r.x, x.x, y.x, MPFR_RNDN);
    return r;
}

Number number_ldexp(Number x, int exp)
{
    return Number();//bid128_ldexp(x.x, exp);
}

Number number_lgamma(Number x)
{
    Number r;
    int sign;
    mpfr_lgamma(r.x, &sign, x.x, MPFR_RNDN);
    return r;
}

Number number_log10(Number x)
{
    Number r;
    mpfr_log10(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_log1p(Number x)
{
    Number r;
    mpfr_log1p(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_log2(Number x)
{
    Number r;
    mpfr_log2(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_nearbyint(Number x)
{
    return Number();//bid128_nearbyint(x.x);
}

Number number_sinh(Number x)
{
    Number r;
    mpfr_sinh(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_tanh(Number x)
{
    Number r;
    mpfr_tanh(r.x, x.x, MPFR_RNDN);
    return r;
}

Number number_tgamma(Number x)
{
    return Number();//bid128_tgamma(x.x);
}

bool number_is_zero(Number x)
{
    return mpfr_zero_p(x.x);
}

bool number_is_negative(Number x)
{
    return mpfr_sgn(x.x) < 0;
}

bool number_is_equal(Number x, Number y)
{
    return mpfr_equal_p(x.x, y.x);
}

bool number_is_not_equal(Number x, Number y)
{
    return not mpfr_equal_p(x.x, y.x);
}

bool number_is_less(Number x, Number y)
{
    return mpfr_less_p(x.x, y.x);
}

bool number_is_greater(Number x, Number y)
{
    return mpfr_greater_p(x.x, y.x);
}

bool number_is_less_equal(Number x, Number y)
{
    return mpfr_lessequal_p(x.x, y.x);
}

bool number_is_greater_equal(Number x, Number y)
{
    return mpfr_greaterequal_p(x.x, y.x);
}

bool number_is_integer(Number x)
{
    return mpfr_integer_p(x.x);
}

bool number_is_odd(Number x)
{
    return false;//not bid128_isZero(bid128_fmod(x.x, bid128_from_uint32(2)));
}

bool number_is_nan(Number x)
{
    return mpfr_nan_p(x.x);
}

std::string number_to_string(Number x)
{
    mpfr_exp_t exp;
    const int DECIMAL_DIGITS = 38;
    char buf[1+DECIMAL_DIGITS+1];
    mpfr_get_str(buf, &exp, 10, DECIMAL_DIGITS, x.x, MPFR_RNDN);
    std::string r = buf;
    if (not r.empty() && r[0] == '@') {
        return r;
    }
    std::string sign = not r.empty() && r[0] == '-' ? "-" : "";
    std::string m = sign.empty() ? r : r.substr(1);
    if (m.empty()) {
        return "0";
    } else if (exp <= 0) {
        r = sign + "0." + std::string(-exp, '0') + m;
    } else if (static_cast<size_t>(exp) < m.length()) {
        r = sign + m.substr(0, exp) + "." + m.substr(exp);
    } else {
        return sign + m + std::string(exp - m.length(), '0');
    }
    std::string::size_type i = r.length() - 1;
    while (r[i] == '0') {
        i--;
    }
    if (r[i] == '.') {
        i--;
    }
    return r.substr(0, i+1);
}

uint8_t number_to_uint8(Number x)
{
    return mpfr_get_ui(x.x, MPFR_RNDN);
}

int8_t number_to_sint8(Number x)
{
    return mpfr_get_si(x.x, MPFR_RNDN);
}

uint16_t number_to_uint16(Number x)
{
    return mpfr_get_ui(x.x, MPFR_RNDN);
}

int16_t number_to_sint16(Number x)
{
    return mpfr_get_si(x.x, MPFR_RNDN);
}

uint32_t number_to_uint32(Number x)
{
    return mpfr_get_ui(x.x, MPFR_RNDN);
}

int32_t number_to_sint32(Number x)
{
    return mpfr_get_si(x.x, MPFR_RNDN);
}

uint64_t number_to_uint64(Number x)
{
    return mpfr_get_uj(x.x, MPFR_RNDN);
}

int64_t number_to_sint64(Number x)
{
    return mpfr_get_sj(x.x, MPFR_RNDN);
}

float number_to_float(Number x)
{
    return mpfr_get_flt(x.x, MPFR_RNDN);
}

double number_to_double(Number x)
{
    return mpfr_get_d(x.x, MPFR_RNDN);
}

Number number_from_string(const std::string &s)
{
    Number r;
    mpfr_set_str(r.x, s.c_str(), 10, MPFR_RNDN);
    return r;
}

Number number_from_uint8(uint8_t x)
{
    Number r;
    mpfr_set_ui(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_sint8(int8_t x)
{
    Number r;
    mpfr_set_si(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_uint16(uint16_t x)
{
    Number r;
    mpfr_set_ui(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_sint16(int16_t x)
{
    Number r;
    mpfr_set_si(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_uint32(uint32_t x)
{
    Number r;
    mpfr_set_ui(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_sint32(int32_t x)
{
    Number r;
    mpfr_set_si(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_uint64(uint64_t x)
{
    Number r;
    mpfr_set_uj(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_sint64(int64_t x)
{
    Number r;
    mpfr_set_sj(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_float(float x)
{
    Number r;
    mpfr_set_flt(r.x, x, MPFR_RNDN);
    return r;
}

Number number_from_double(double x)
{
    Number r;
    mpfr_set_d(r.x, x, MPFR_RNDN);
    return r;
}
