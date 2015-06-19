#include "number.h"

#include <iso646.h>

Number number_add(Number x, Number y)
{
    return bid128_add(x.x, y.x);
}

Number number_subtract(Number x, Number y)
{
    return bid128_sub(x.x, y.x);
}

Number number_multiply(Number x, Number y)
{
    return bid128_mul(x.x, y.x);
}

Number number_divide(Number x, Number y)
{
    return bid128_div(x.x, y.x);
}

Number number_modulo(Number x, Number y)
{
    BID_UINT128 m = bid128_abs(y.x);
    if (bid128_isSigned(x.x)) {
        Number q = number_ceil(bid128_div(bid128_abs(x.x), m));
        x.x = bid128_add(x.x, bid128_mul(m, q.x));
    }
    BID_UINT128 r = bid128_fmod(x.x, m);
    if (bid128_isSigned(y.x) && not bid128_isZero(r)) {
        r = bid128_sub(r, m);
    }
    return r;
}

Number number_pow(Number x, Number y)
{
    return bid128_pow(x.x, y.x);
}

Number number_negate(Number x)
{
    return bid128_negate(x.x);
}

Number number_abs(Number x)
{
    return bid128_abs(x.x);
}

Number number_sign(Number x)
{
    return bid128_copySign(bid128_from_uint32(1), x.x);
}

Number number_ceil(Number x)
{
    // TODO: find proper ceil function to handle full range of decimal128
    return bid128_from_int64(bid128_to_int64_ceil(x.x));
}

Number number_floor(Number x)
{
    // TODO: find proper floor function to handle full range of decimal128
    return bid128_from_int64(bid128_to_int64_floor(x.x));
}

Number number_exp(Number x)
{
    return bid128_exp(x.x);
}

Number number_log(Number x)
{
    return bid128_log(x.x);
}

Number number_sqrt(Number x)
{
    return bid128_sqrt(x.x);
}

Number number_acos(Number x)
{
    return bid128_acos(x.x);
}

Number number_asin(Number x)
{
    return bid128_asin(x.x);
}

Number number_atan(Number x)
{
    return bid128_atan(x.x);
}

Number number_cos(Number x)
{
    return bid128_cos(x.x);
}

Number number_sin(Number x)
{
    return bid128_sin(x.x);
}

Number number_tan(Number x)
{
    return bid128_tan(x.x);
}

Number number_acosh(Number x)
{
    return bid128_acosh(x.x);
}

Number number_asinh(Number x)
{
    return bid128_asinh(x.x);
}

Number number_atanh(Number x)
{
    return bid128_atanh(x.x);
}

Number number_atan2(Number y, Number x)
{
    return bid128_atan2(y.x, x.x);
}

Number number_cbrt(Number x)
{
    return bid128_cbrt(x.x);
}

Number number_cosh(Number x)
{
    return bid128_cosh(x.x);
}

Number number_erf(Number x)
{
    return bid128_erf(x.x);
}

Number number_erfc(Number x)
{
    return bid128_erfc(x.x);
}

Number number_exp2(Number x)
{
    return bid128_exp2(x.x);
}

Number number_expm1(Number x)
{
    return bid128_expm1(x.x);
}

Number number_frexp(Number x, int *exp)
{
    return bid128_frexp(x.x, exp);
}

Number number_hypot(Number x, Number y)
{
    return bid128_hypot(x.x, y.x);
}

Number number_ldexp(Number x, int exp)
{
    return bid128_ldexp(x.x, exp);
}

Number number_lgamma(Number x)
{
    return bid128_lgamma(x.x);
}

Number number_log10(Number x)
{
    return bid128_log10(x.x);
}

Number number_log1p(Number x)
{
    return bid128_log1p(x.x);
}

Number number_log2(Number x)
{
    return bid128_log2(x.x);
}

Number number_nearbyint(Number x)
{
    return bid128_nearbyint(x.x);
}

Number number_sinh(Number x)
{
    return bid128_sinh(x.x);
}

Number number_tanh(Number x)
{
    return bid128_tanh(x.x);
}

Number number_tgamma(Number x)
{
    return bid128_tgamma(x.x);
}

bool number_is_zero(Number x)
{
    return bid128_isZero(x.x) != 0;
}

bool number_is_negative(Number x)
{
    return bid128_isSigned(x.x) != 0;
}

bool number_is_equal(Number x, Number y)
{
    return bid128_quiet_equal(x.x, y.x) != 0;
}

bool number_is_not_equal(Number x, Number y)
{
    return bid128_quiet_not_equal(x.x, y.x) != 0;
}

bool number_is_less(Number x, Number y)
{
    return bid128_quiet_less(x.x, y.x) != 0;
}

bool number_is_greater(Number x, Number y)
{
    return bid128_quiet_greater(x.x, y.x) != 0;
}

bool number_is_less_equal(Number x, Number y)
{
    return bid128_quiet_less_equal(x.x, y.x) != 0;
}

bool number_is_greater_equal(Number x, Number y)
{
    return bid128_quiet_greater_equal(x.x, y.x) != 0;
}

bool number_is_integer(Number x)
{
    BID_UINT128 i = bid128_round_integral_zero(x.x);
    return bid128_quiet_equal(x.x, i) != 0;
}

bool number_is_nan(Number x)
{
    return bid128_isNaN(x.x) != 0;
}

std::string number_to_string(Number x)
{
    const int PRECISION = 34;

    char buf[50];
    bid128_to_string(buf, x.x);
    std::string sbuf(buf);
    const std::string::size_type E = sbuf.find('E');
    if (E == std::string::npos) {
        // Inf or NaN
        return sbuf;
    }
    int exponent = std::stoi(sbuf.substr(E+1));
    sbuf = sbuf.substr(0, E);
    const std::string::size_type last_significant_digit = sbuf.find_last_not_of('0');
    if (last_significant_digit == 0) {
        return "0";
    }
    const int trailing_zeros = static_cast<int>(sbuf.length() - (last_significant_digit + 1));
    exponent += trailing_zeros;
    sbuf = sbuf.substr(0, last_significant_digit + 1);
    if (exponent != 0) {
        if (exponent > 0 && sbuf.length() + exponent <= PRECISION+1) {
            sbuf.append(exponent, '0');
        } else if (exponent < 0 && -exponent < static_cast<int>(sbuf.length()-1)) {
            sbuf = sbuf.substr(0, sbuf.length()+exponent) + "." + sbuf.substr(sbuf.length()+exponent);
        } else if (exponent < 0 && -exponent == static_cast<int>(sbuf.length()-1)) {
            sbuf = sbuf.substr(0, 1) + "0." + sbuf.substr(1);
        } else if (exponent < 0 && sbuf.length() - exponent <= PRECISION+2) {
            sbuf.insert(1, "0." + std::string(-exponent-1, '0'));
        } else {
            exponent += static_cast<int>(sbuf.length() - 2);
            if (sbuf.length() >= 3) {
                sbuf.insert(2, 1, '.');
            }
            sbuf.push_back('e');
            sbuf.append(std::to_string(exponent));
        }
    }
    if (sbuf.at(0) == '+') {
        sbuf = sbuf.substr(1);
    }
    return sbuf;
}

uint8_t number_to_uint8(Number x)
{
    return bid128_to_uint8_int(x.x);
}

int8_t number_to_sint8(Number x)
{
    return bid128_to_int8_int(x.x);
}

uint16_t number_to_uint16(Number x)
{
    return bid128_to_uint16_int(x.x);
}

int16_t number_to_sint16(Number x)
{
    return bid128_to_int16_int(x.x);
}

uint32_t number_to_uint32(Number x)
{
    return bid128_to_uint32_int(x.x);
}

int32_t number_to_sint32(Number x)
{
    return bid128_to_int32_int(x.x);
}

uint64_t number_to_uint64(Number x)
{
    return bid128_to_uint64_int(x.x);
}

int64_t number_to_sint64(Number x)
{
    return bid128_to_int64_int(x.x);
}

float number_to_float(Number x)
{
    return bid128_to_binary32(x.x);
}

double number_to_double(Number x)
{
    return bid128_to_binary64(x.x);
}

Number number_from_string(const std::string &s)
{
    return bid128_from_string(const_cast<char *>(s.c_str()));
}

Number number_from_uint8(uint8_t x)
{
    return bid128_from_uint32(x);
}

Number number_from_sint8(int8_t x)
{
    return bid128_from_int32(x);
}

Number number_from_uint16(uint16_t x)
{
    return bid128_from_uint32(x);
}

Number number_from_sint16(int16_t x)
{
    return bid128_from_int32(x);
}

Number number_from_uint32(uint32_t x)
{
    return bid128_from_uint32(x);
}

Number number_from_sint32(int32_t x)
{
    return bid128_from_int32(x);
}

Number number_from_uint64(uint64_t x)
{
    return bid128_from_uint64(x);
}

Number number_from_sint64(int64_t x)
{
    return bid128_from_int64(x);
}

Number number_from_float(float x)
{
    return binary32_to_bid128(x);
}

Number number_from_double(double x)
{
    return binary64_to_bid128(x);
}
