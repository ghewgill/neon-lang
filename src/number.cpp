#include "number.h"

#include <assert.h>
#include <iso646.h>

const mpz_class &Number::get_mpz()
{
    assert(rep == Rep::MPZ);
    return mpz;
}

BID_UINT128 Number::get_bid()
{
    if (rep == Rep::BID) {
        return bid;
    }
    assert(rep == Rep::MPZ);
    return bid128_from_string(const_cast<char *>(mpz.get_str().c_str()));
}

Number number_add(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return mpz_class(x.get_mpz() + y.get_mpz());
    }
    return bid128_add(x.get_bid(), y.get_bid());
}

Number number_subtract(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return mpz_class(x.get_mpz() - y.get_mpz());
    }
    return bid128_sub(x.get_bid(), y.get_bid());
}

Number number_multiply(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return mpz_class(x.get_mpz() * y.get_mpz());
    }
    return bid128_mul(x.get_bid(), y.get_bid());
}

Number number_divide(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        if (mpz_divisible_p(x.get_mpz().get_mpz_t(), y.get_mpz().get_mpz_t())) {
            return mpz_class(x.get_mpz() / y.get_mpz());
        }
    }
    return bid128_div(x.get_bid(), y.get_bid());
}

Number number_modulo(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        mpz_class r;
        mpz_fdiv_r(r.get_mpz_t(), x.get_mpz().get_mpz_t(), y.get_mpz().get_mpz_t());
        return r;
    }
    BID_UINT128 m = bid128_abs(y.get_bid());
    if (bid128_isSigned(x.get_bid())) {
        Number q = number_ceil(bid128_div(bid128_abs(x.get_bid()), m));
        x.get_bid() = bid128_add(x.get_bid(), bid128_mul(m, q.get_bid()));
    }
    BID_UINT128 r = bid128_fmod(x.get_bid(), m);
    if (bid128_isSigned(y.get_bid()) && not bid128_isZero(r)) {
        r = bid128_sub(r, m);
    }
    return r;
}

Number number_pow(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        mpz_class r;
        mpz_pow_ui(r.get_mpz_t(), x.get_mpz().get_mpz_t(), y.get_mpz().get_ui());
        return r;
    }
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

Number number_negate(Number x)
{
    if (x.rep == Rep::MPZ) {
        return mpz_class(-x.get_mpz());
    }
    return bid128_negate(x.get_bid());
}

Number number_abs(Number x)
{
    if (x.rep == Rep::MPZ) {
        return mpz_class(abs(x.get_mpz()));
    }
    return bid128_abs(x.get_bid());
}

Number number_sign(Number x)
{
    if (x.rep == Rep::MPZ) {
        return mpz_class(sgn(x.get_mpz()));
    }
    return bid128_copySign(bid128_from_uint32(1), x.get_bid());
}

Number number_ceil(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x;
    }
    return bid128_round_integral_positive(x.get_bid());
}

Number number_floor(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x;
    }
    return bid128_round_integral_negative(x.get_bid());
}

Number number_trunc(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x;
    }
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
    // TODO: mpz sqrt
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
    // TODO: mpz
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
    // TODO: mpz
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
    if (x.rep == Rep::MPZ) {
        return x;
    }
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
    if (x.rep == Rep::MPZ) {
        return x.get_mpz() == 0;
    }
    return bid128_isZero(x.get_bid()) != 0;
}

bool number_is_negative(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz() < 0;
    }
    return bid128_isSigned(x.get_bid()) != 0;
}

bool number_is_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() == y.get_mpz();
    }
    return bid128_quiet_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_not_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() != y.get_mpz();
    }
    return bid128_quiet_not_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_less(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() < y.get_mpz();
    }
    return bid128_quiet_less(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_greater(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() > y.get_mpz();
    }
    return bid128_quiet_greater(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_less_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() <= y.get_mpz();
    }
    return bid128_quiet_less_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_greater_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() >= y.get_mpz();
    }
    return bid128_quiet_greater_equal(x.get_bid(), y.get_bid()) != 0;
}

bool number_is_integer(Number x)
{
    if (x.rep == Rep::MPZ) {
        return true;
    }
    BID_UINT128 i = bid128_round_integral_zero(x.get_bid());
    return bid128_quiet_equal(x.get_bid(), i) != 0;
}

bool number_is_odd(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz() % 2 != 0;
    }
    return not bid128_isZero(bid128_fmod(x.get_bid(), bid128_from_uint32(2)));
}

bool number_is_nan(Number x)
{
    if (x.rep == Rep::MPZ) {
        return false;
    }
    return bid128_isNaN(x.get_bid()) != 0;
}

std::string number_to_string(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_str();
    }

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
        if (exponent > 0 && sbuf.length() + exponent <= PRECISION+1) {
            sbuf.append(exponent, '0');
        } else if (exponent < 0 && -exponent < static_cast<int>(sbuf.length()-1)) {
            sbuf = sbuf.substr(0, sbuf.length()+exponent) + "." + sbuf.substr(sbuf.length()+exponent);
        } else if (exponent < 0 && -exponent == static_cast<int>(sbuf.length()-1)) {
            sbuf = sbuf.substr(0, 1) + "0." + sbuf.substr(1);
        } else if (exponent < 0 && sbuf.length() - exponent <= PRECISION+2) {
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
    if (x.rep == Rep::MPZ) {
        return static_cast<uint8_t>(x.get_mpz().get_ui());
    }
    return bid128_to_uint8_int(x.get_bid());
}

int8_t number_to_sint8(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<int8_t>(x.get_mpz().get_si());
    }
    return bid128_to_int8_int(x.get_bid());
}

uint16_t number_to_uint16(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<uint16_t>(x.get_mpz().get_ui());
    }
    return bid128_to_uint16_int(x.get_bid());
}

int16_t number_to_sint16(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<int16_t>(x.get_mpz().get_si());
    }
    return bid128_to_int16_int(x.get_bid());
}

uint32_t number_to_uint32(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_ui();
    }
    return bid128_to_uint32_int(x.get_bid());
}

int32_t number_to_sint32(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_si();
    }
    return bid128_to_int32_int(x.get_bid());
}

uint64_t number_to_uint64(Number x)
{
    if (x.rep == Rep::MPZ) {
        size_t uls = sizeof(unsigned long);
        if (uls >= 8) {
            return x.get_mpz().get_ui();
        } else {
            return static_cast<uint64_t>(x.get_mpz().get_ui() & 0xFFFFFFFF) + (static_cast<uint64_t>(mpz_class(x.get_mpz() >> 32).get_ui() & 0xFFFFFFFF) << 32);
        }
    }
    return bid128_to_uint64_int(x.get_bid());
}

int64_t number_to_sint64(Number x)
{
    if (x.rep == Rep::MPZ) {
        size_t sls = sizeof(signed long);
        if (sls >= 8) {
            return x.get_mpz().get_si();
        } else {
            return static_cast<int64_t>(x.get_mpz().get_si() & 0xFFFFFFFF) + (static_cast<int64_t>(mpz_class(x.get_mpz() >> 32).get_si() & 0xFFFFFFFF) << 32);
        }
    }
    return bid128_to_int64_int(x.get_bid());
}

float number_to_float(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<float>(x.get_mpz().get_d());
    }
    return bid128_to_binary32(x.get_bid());
}

double number_to_double(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_d();
    }
    return bid128_to_binary64(x.get_bid());
}

Number number_from_string(const std::string &s)
{
    std::string::size_type i = 0;
    if (s[i] == '-') {
        i++;
    }
    if (s.find_first_not_of("0123456789", i) == std::string::npos) {
        return mpz_class(s, 10);
    }
    return bid128_from_string(const_cast<char *>(s.c_str()));
}

Number number_from_uint8(uint8_t x)
{
    return mpz_class(x);
}

Number number_from_sint8(int8_t x)
{
    return mpz_class(x);
}

Number number_from_uint16(uint16_t x)
{
    return mpz_class(x);
}

Number number_from_sint16(int16_t x)
{
    return mpz_class(x);
}

Number number_from_uint32(uint32_t x)
{
    return mpz_class(x);
}

Number number_from_sint32(int32_t x)
{
    return mpz_class(x);
}

Number number_from_uint64(uint64_t x)
{
    mpz_class hi;
    mpz_mul_2exp(hi.get_mpz_t(), mpz_class(static_cast<uint32_t>(x >> 32)).get_mpz_t(), 32);
    return mpz_class(static_cast<uint32_t>(x & 0xFFFFFFFF) + hi);
}

Number number_from_sint64(int64_t x)
{
    if (x >= 0) {
        mpz_class hi;
        mpz_mul_2exp(hi.get_mpz_t(), mpz_class(static_cast<uint32_t>(x >> 32)).get_mpz_t(), 32);
        return mpz_class(static_cast<uint32_t>(x & 0xFFFFFFFF) + hi);
    } else {
        mpz_class hi;
        mpz_mul_2exp(hi.get_mpz_t(), mpz_class(static_cast<uint32_t>((-x) >> 32)).get_mpz_t(), 32);
        return mpz_class(-(static_cast<uint32_t>((-x) & 0xFFFFFFFF) + hi));
    }
}

Number number_from_float(float x)
{
    return binary32_to_bid128(x);
}

Number number_from_double(double x)
{
    return binary64_to_bid128(x);
}
