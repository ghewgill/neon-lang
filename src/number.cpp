#include "number.h"

#include <assert.h>
#include <iso646.h>

BID_UINT128 Number::get_bid()
{
    return bid;
}

Number number_add(Number x, Number y)
{
    return bid128_add(x.get_bid(), y.get_bid());
}

Number number_subtract(Number x, Number y)
{
    return bid128_sub(x.get_bid(), y.get_bid());
}

Number number_multiply(Number x, Number y)
{
    return bid128_mul(x.get_bid(), y.get_bid());
}

Number number_divide(Number x, Number y)
{
    return bid128_div(x.get_bid(), y.get_bid());
}

Number number_modulo(Number x, Number y)
{
    BID_UINT128 m = bid128_abs(y.get_bid());
    if (bid128_isSigned(x.get_bid())) {
        Number q = number_ceil(bid128_div(bid128_abs(x.get_bid()), m));
        x = bid128_add(x.get_bid(), bid128_mul(m, q.get_bid()));
    }
    BID_UINT128 r = bid128_fmod(x.get_bid(), m);
    if (bid128_isSigned(y.get_bid()) && not bid128_isZero(r)) {
        r = bid128_sub(r, m);
    }
    return r;
}

Number number_pow(Number x, Number y)
{
    if (number_is_integer(y) && not number_is_negative(y)) {
        uint32_t iy = number_to_uint32(y);
        Number r = number_from_uint32(1);
        while (iy != 0) {
            if (iy & 1) {
                r = number_multiply(r, x);
            }
            x = number_multiply(x, x);
            iy >>= 1;
        }
        return r;
    }
    return bid128_pow(x.get_bid(), y.get_bid());
}

Number number_powmod(Number /*b*/, Number /*e*/, Number /*m*/)
{
    return Number();
}

Number number_negate(Number x)
{
    return bid128_negate(x.get_bid());
}

Number number_abs(Number x)
{
    return bid128_abs(x.get_bid());
}

Number number_sign(Number x)
{
    if (bid128_isZero(x.get_bid())) {
        return bid128_from_uint32(0);
    }
    return bid128_copySign(bid128_from_uint32(1), x.get_bid());
}

Number number_ceil(Number x)
{
    return bid128_round_integral_positive(x.get_bid());
}

Number number_floor(Number x)
{
    return bid128_round_integral_negative(x.get_bid());
}

Number number_trunc(Number x)
{
    return bid128_round_integral_zero(x.get_bid());
}

Number number_exp(Number x)
{
    return bid128_exp(x.get_bid());
}

Number number_log(Number x)
{
    return bid128_log(x.get_bid());
}

Number number_sqrt(Number x)
{
    return bid128_sqrt(x.get_bid());
}

Number number_acos(Number x)
{
    return bid128_acos(x.get_bid());
}

Number number_asin(Number x)
{
    return bid128_asin(x.get_bid());
}

Number number_atan(Number x)
{
    return bid128_atan(x.get_bid());
}

Number number_cos(Number x)
{
    return bid128_cos(x.get_bid());
}

Number number_sin(Number x)
{
    return bid128_sin(x.get_bid());
}

Number number_tan(Number x)
{
    return bid128_tan(x.get_bid());
}

Number number_acosh(Number x)
{
    return bid128_acosh(x.get_bid());
}

Number number_asinh(Number x)
{
    return bid128_asinh(x.get_bid());
}

Number number_atanh(Number x)
{
    return bid128_atanh(x.get_bid());
}

Number number_atan2(Number y, Number x)
{
    return bid128_atan2(y.get_bid(), x.get_bid());
}

Number number_cbrt(Number x)
{
    return bid128_cbrt(x.get_bid());
}

Number number_cosh(Number x)
{
    return bid128_cosh(x.get_bid());
}

Number number_erf(Number x)
{
    return bid128_erf(x.get_bid());
}

Number number_erfc(Number x)
{
    return bid128_erfc(x.get_bid());
}

Number number_exp2(Number x)
{
    return bid128_exp2(x.get_bid());
}

Number number_expm1(Number x)
{
    return bid128_expm1(x.get_bid());
}

Number number_frexp(Number x, int *exp)
{
    return bid128_frexp(x.get_bid(), exp);
}

Number number_hypot(Number x, Number y)
{
    return bid128_hypot(x.get_bid(), y.get_bid());
}

Number number_ldexp(Number x, int exp)
{
    return bid128_ldexp(x.get_bid(), exp);
}

Number number_lgamma(Number x)
{
    return bid128_lgamma(x.get_bid());
}

Number number_log10(Number x)
{
    return bid128_log10(x.get_bid());
}

Number number_log1p(Number x)
{
    return bid128_log1p(x.get_bid());
}

Number number_log2(Number x)
{
    return bid128_log2(x.get_bid());
}

Number number_nearbyint(Number x)
{
    return bid128_nearbyint(x.get_bid());
}

Number number_sinh(Number x)
{
    return bid128_sinh(x.get_bid());
}

Number number_tanh(Number x)
{
    return bid128_tanh(x.get_bid());
}

Number number_tgamma(Number x)
{
    return bid128_tgamma(x.get_bid());
}

bool number_is_zero(Number x)
{
    return bid128_isZero(x.get_bid()) != 0;
}

bool number_is_negative(Number x)
{
    return bid128_isSigned(x.get_bid()) != 0;
}

bool number_is_equal(Number x, Number y)
{
    return bid128_quiet_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_not_equal(Number x, Number y)
{
    return bid128_quiet_not_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_less(Number x, Number y)
{
    return bid128_quiet_less(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_greater(Number x, Number y)
{
    return bid128_quiet_greater(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_less_equal(Number x, Number y)
{
    return bid128_quiet_less_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_greater_equal(Number x, Number y)
{
    return bid128_quiet_greater_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_integer(Number x)
{
    BID_UINT128 i = bid128_round_integral_zero(x.get_bid());
    return bid128_quiet_equal(x.get_bid(), i) != 0;
}

bool number_is_odd(Number x)
{
    return not bid128_isZero(bid128_fmod(x.get_bid(), bid128_from_uint32(2)));
}

bool number_is_finite(Number x)
{
    return bid128_isFinite(x.get_bid());
}

bool number_is_nan(Number x)
{
    return bid128_isNaN(x.get_bid()) != 0;
}

std::string number_to_string(Number x, Format format)
{
    const int PRECISION = 34;

    char buf[50];
    bid128_to_string(buf, x.get_bid());
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
        if (exponent > 0 && (format == Format::full || sbuf.length() + exponent <= PRECISION+1)) {
            sbuf.append(exponent, '0');
        } else if (exponent < 0 && -exponent < static_cast<int>(sbuf.length()-1)) {
            sbuf = sbuf.substr(0, sbuf.length()+exponent) + "." + sbuf.substr(sbuf.length()+exponent);
        } else if (exponent < 0 && -exponent == static_cast<int>(sbuf.length()-1)) {
            sbuf = sbuf.substr(0, 1) + "0." + sbuf.substr(1);
        } else if (exponent < 0 && (format == Format::full || sbuf.length() - exponent <= PRECISION+2)) {
            sbuf.insert(1, "0." + std::string(-exponent-(sbuf.length()-1), '0'));
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
    return bid128_to_uint8_int(x.get_bid());
}

int8_t number_to_sint8(Number x)
{
    return bid128_to_int8_int(x.get_bid());
}

uint16_t number_to_uint16(Number x)
{
    return bid128_to_uint16_int(x.get_bid());
}

int16_t number_to_sint16(Number x)
{
    return bid128_to_int16_int(x.get_bid());
}

uint32_t number_to_uint32(Number x)
{
    return bid128_to_uint32_int(x.get_bid());
}

int32_t number_to_sint32(Number x)
{
    return bid128_to_int32_int(x.get_bid());
}

uint64_t number_to_uint64(Number x)
{
    return bid128_to_uint64_int(x.get_bid());
}

int64_t number_to_sint64(Number x)
{
    return bid128_to_int64_int(x.get_bid());
}

float number_to_float(Number x)
{
    return bid128_to_binary32(x.get_bid());
}

double number_to_double(Number x)
{
    return bid128_to_binary64(x.get_bid());
}

Number number_from_string(const std::string &s)
{
    std::string::size_type i = 0;
    std::string::size_type skip = 0;
    if (s[i] == '+') {
        i++;
        skip = i;
    } else if (s[i] == '-') {
        i++;
    }
    std::string::size_type next = s.find_first_not_of("0123456789", i);
    // Number must start with a digit.
    if (next == i) {
        return bid128_nan(NULL);
    }
    // Check exact allowed formats because bid128_from_string
    // is a bit too permissive.
    if (next != std::string::npos && s[next] == '.') {
        i = next + 1;
        if (i >= s.length()) {
            return bid128_nan(NULL);
        }
        next = s.find_first_not_of("0123456789", i);
        // Must have one or more digits after decimal point.
        if (next == i) {
            return bid128_nan(NULL);
        }
    }
    // Check for exponential notation.
    if (next != std::string::npos && (s[next] == 'e' || s[next] == 'E')) {
        i = next + 1;
        // e may be followed by a + or -
        if (i < s.length() && (s[i] == '+' || s[i] == '-')) {
            i++;
        }
        if (i >= s.length()) {
            return bid128_nan(NULL);
        }
        // Must have one or more digits in exponent.
        next = s.find_first_not_of("0123456789", i);
        if (next == i) {
            return bid128_nan(NULL);
        }
    }
    // Reject if there is any trailing junk.
    if (next != std::string::npos) {
        return bid128_nan(NULL);
    }
    return bid128_from_string(const_cast<char *>(s.c_str() + skip));
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
