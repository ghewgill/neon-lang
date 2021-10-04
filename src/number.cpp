#include "number.h"

#include <assert.h>
#include <iso646.h>

const mpz_class &Number::get_mpz()
{
    assert(rep == Rep::MPZ);
    return mpz;
}

dec64 Number::get_bid()
{
    if (rep == Rep::BID) {
        return bid;
    }
    assert(rep == Rep::MPZ);
    dec64_string_state state = dec64_string_begin();
    dec64 r = dec64_from_string(state, mpz.get_str().c_str());
    dec64_string_end(state);
    return r;
}

int64 sint64_from_dec64(dec64 x)
{
    int64 cof = dec64_coefficient(x);
    int64 exp = dec64_exponent(x);
    if (exp > 0) {
        while (exp--) {
            cof *= 10;
        }
    } else if (exp < 0) {
        while (exp++) {
            cof /= 10;
        }
    }
    return cof;
}

uint64 uint64_from_dec64(dec64 x)
{
    int64 cof = dec64_coefficient(x);
    if (cof < 0) {
        return 0;
    }
    uint64 ucof = cof;
    int64 exp = dec64_exponent(x);
    if (exp > 0) {
        while (exp--) {
            ucof *= 10;
        }
    } else if (exp < 0) {
        while (exp++) {
            ucof /= 10;
        }
    }
    return ucof;
}

Number number_add(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return mpz_class(x.get_mpz() + y.get_mpz());
    }
    return dec64_add(x.get_bid(), y.get_bid());
}

Number number_subtract(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return mpz_class(x.get_mpz() - y.get_mpz());
    }
    return dec64_subtract(x.get_bid(), y.get_bid());
}

Number number_multiply(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return mpz_class(x.get_mpz() * y.get_mpz());
    }
    return dec64_multiply(x.get_bid(), y.get_bid());
}

Number number_divide(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        if (mpz_divisible_p(x.get_mpz().get_mpz_t(), y.get_mpz().get_mpz_t())) {
            return mpz_class(x.get_mpz() / y.get_mpz());
        }
    }
    return dec64_divide(x.get_bid(), y.get_bid());
}

Number number_modulo(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        mpz_class r;
        mpz_fdiv_r(r.get_mpz_t(), x.get_mpz().get_mpz_t(), y.get_mpz().get_mpz_t());
        return r;
    }
    return dec64_modulo(x.get_bid(), y.get_bid());
/*
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
*/
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
    return dec64_raise(x.get_bid(), y.get_bid());
}

Number number_powmod(Number b, Number e, Number m)
{
    mpz_class r;
    mpz_powm(r.get_mpz_t(), b.get_mpz().get_mpz_t(), e.get_mpz().get_mpz_t(), m.get_mpz().get_mpz_t());
    return r;
}

Number number_negate(Number x)
{
    if (x.rep == Rep::MPZ) {
        return mpz_class(-x.get_mpz());
    }
    return dec64_neg(x.get_bid());
}

Number number_abs(Number x)
{
    if (x.rep == Rep::MPZ) {
        return mpz_class(abs(x.get_mpz()));
    }
    return dec64_abs(x.get_bid());
}

Number number_sign(Number x)
{
    if (x.rep == Rep::MPZ) {
        return mpz_class(sgn(x.get_mpz()));
    }
    return dec64_signum(x.get_bid());
}

Number number_ceil(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x;
    }
    return dec64_ceiling(x.get_bid());
}

Number number_floor(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x;
    }
    return dec64_floor(x.get_bid());
}

Number number_trunc(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x;
    }
    return dec64_is_less(x.get_bid(), DEC64_ZERO) == DEC64_TRUE ? dec64_ceiling(x.get_bid()) : dec64_floor(x.get_bid());
}

Number number_exp(Number x)
{
    return dec64_exp(x.get_bid());
}

Number number_log(Number x)
{
    return dec64_log(x.get_bid());
}

Number number_sqrt(Number x)
{
    // TODO: mpz sqrt
    return dec64_sqrt(x.get_bid());
}

Number number_acos(Number x)
{
    return dec64_acos(x.get_bid());
}

Number number_asin(Number x)
{
    return dec64_asin(x.get_bid());
}

Number number_atan(Number x)
{
    return dec64_atan(x.get_bid());
}

Number number_cos(Number x)
{
    return dec64_cos(x.get_bid());
}

Number number_sin(Number x)
{
    return dec64_sin(x.get_bid());
}

Number number_tan(Number x)
{
    return dec64_tan(x.get_bid());
}

Number number_acosh(Number x)
{
    return /*dec64_acosh*/(x.get_bid());
}

Number number_asinh(Number x)
{
    return /*dec64_asinh*/(x.get_bid());
}

Number number_atanh(Number x)
{
    return /*dec64_atanh*/(x.get_bid());
}

Number number_atan2(Number y, Number x)
{
    return dec64_atan2(y.get_bid(), x.get_bid());
}

Number number_cbrt(Number x)
{
    // TODO: mpz
    return /*dec64_cbrt*/(x.get_bid());
}

Number number_cosh(Number x)
{
    return /*dec64_cosh*/(x.get_bid());
}

Number number_erf(Number x)
{
    return /*dec64_erf*/(x.get_bid());
}

Number number_erfc(Number x)
{
    return /*dec64_erfc*/(x.get_bid());
}

Number number_exp2(Number x)
{
    // TODO: mpz
    return /*dec64_exp2*/(x.get_bid());
}

Number number_expm1(Number x)
{
    return /*dec64_expm1*/(x.get_bid());
}

Number number_frexp(Number /*x*/, int * /* exp*/)
{
    return DEC64_ZERO; //dec64_frexp(x.get_bid(), exp);
}

Number number_hypot(Number /*x*/, Number /*y*/)
{
    return DEC64_ZERO; //dec64_hypot(x.get_bid(), y.get_bid());
}

Number number_ldexp(Number /*x*/, int /*exp*/)
{
    return DEC64_ZERO; //dec64_ldexp(x.get_bid(), exp);
}

Number number_lgamma(Number x)
{
    return /*dec64_lgamma*/(x.get_bid());
}

Number number_log10(Number x)
{
    return /*dec64_log10*/(x.get_bid());
}

Number number_log1p(Number x)
{
    return /*dec64_log1p*/(x.get_bid());
}

Number number_log2(Number x)
{
    return /*dec64_log2*/(x.get_bid());
}

Number number_nearbyint(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x;
    }
    return DEC64_ZERO; //dec64_nearbyint(x.get_bid());
}

Number number_sinh(Number x)
{
    return /*dec64_sinh*/(x.get_bid());
}

Number number_tanh(Number x)
{
    return /*dec64_tanh*/(x.get_bid());
}

Number number_tgamma(Number x)
{
    return /*dec64_tgamma*/(x.get_bid());
}

bool number_is_zero(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz() == 0;
    }
    return dec64_is_zero(x.get_bid()) != DEC64_FALSE;
}

bool number_is_negative(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz() < 0;
    }
    return dec64_is_less(x.get_bid(), DEC64_ZERO) != DEC64_FALSE;
}

bool number_is_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() == y.get_mpz();
    }
    return dec64_is_equal(x.get_bid(), y.get_bid()) != DEC64_FALSE;
}

bool number_is_not_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() != y.get_mpz();
    }
    return dec64_is_equal(x.get_bid(), y.get_bid()) == DEC64_FALSE;
}

bool number_is_less(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() < y.get_mpz();
    }
    return dec64_is_less(x.get_bid(), y.get_bid()) != DEC64_FALSE;
}

bool number_is_greater(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() > y.get_mpz();
    }
    return dec64_is_less(y.get_bid(), x.get_bid()) != DEC64_FALSE;
}

bool number_is_less_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() <= y.get_mpz();
    }
    return dec64_is_less(x.get_bid(), y.get_bid()) != DEC64_FALSE || dec64_is_equal(x.get_bid(), y.get_bid()) != DEC64_FALSE;
}

bool number_is_greater_equal(Number x, Number y)
{
    if (x.rep == Rep::MPZ && y.rep == Rep::MPZ) {
        return x.get_mpz() >= y.get_mpz();
    }
    return dec64_is_less(y.get_bid(), x.get_bid()) != DEC64_FALSE || dec64_is_equal(x.get_bid(), y.get_bid()) != DEC64_FALSE;
}

bool number_is_integer(Number x)
{
    if (x.rep == Rep::MPZ) {
        return true;
    }
    return dec64_is_integer(x.get_bid()) != DEC64_FALSE;
}

bool number_is_odd(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz() % 2 != 0;
    }
    return dec64_is_zero(dec64_modulo(x.get_bid(), DEC64_TWO)) == DEC64_FALSE;
}

bool number_is_finite(Number x)
{
    if (x.rep == Rep::MPZ) {
        return true;
    }
    return true;
}

bool number_is_nan(Number x)
{
    if (x.rep == Rep::MPZ) {
        return false;
    }
    return dec64_is_nan(x.get_bid()) != DEC64_FALSE;
}

std::string number_to_string(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_str();
    }

    const int PRECISION = 34;

    char buf[50];
    dec64_to_string(dec64_default_state(), x.get_bid(), buf);
    std::string sbuf(buf);
    const std::string::size_type E = sbuf.find('E');
    if (E == std::string::npos) {
        // Inf or NaN
        if (sbuf.back() == '.') {
            sbuf = sbuf.substr(0, sbuf.length()-1);
        }
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
    return uint64_from_dec64(x.get_bid());
}

int8_t number_to_sint8(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<int8_t>(x.get_mpz().get_si());
    }
    return sint64_from_dec64(x.get_bid());
}

uint16_t number_to_uint16(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<uint16_t>(x.get_mpz().get_ui());
    }
    return uint64_from_dec64(x.get_bid());
}

int16_t number_to_sint16(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<int16_t>(x.get_mpz().get_si());
    }
    return sint64_from_dec64(x.get_bid());
}

uint32_t number_to_uint32(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_ui();
    }
    return uint64_from_dec64(x.get_bid());
}

int32_t number_to_sint32(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_si();
    }
    return sint64_from_dec64(x.get_bid());
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
    return uint64_from_dec64(x.get_bid());
}

int64_t number_to_sint64(Number x)
{
    if (x.rep == Rep::MPZ) {
        size_t sls = sizeof(signed long);
        if (sls >= 8) {
            return x.get_mpz().get_si();
        } else {
            bool negative = false;
            if (number_is_negative(x)) {
                negative = true;
                x = number_negate(x);
            }
            uint64_t ur = static_cast<uint64_t>(x.get_mpz().get_ui() & 0xFFFFFFFF) + (static_cast<uint64_t>(mpz_class(x.get_mpz() >> 32).get_ui() & 0xFFFFFFFF) << 32);
            if (ur > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
                if (negative) {
                    return std::numeric_limits<int64_t>::min();
                } else {
                    return std::numeric_limits<int64_t>::max();
                }
            }
            int64_t sr = static_cast<int64_t>(ur);
            if (negative) {
                sr = -sr;
            }
            return sr;
        }
    }
    return sint64_from_dec64(x.get_bid());
}

float number_to_float(Number x)
{
    if (x.rep == Rep::MPZ) {
        return static_cast<float>(x.get_mpz().get_d());
    }
    abort();
    return DEC64_ZERO; //bid128_to_binary32(x.get_bid());
}

double number_to_double(Number x)
{
    if (x.rep == Rep::MPZ) {
        return x.get_mpz().get_d();
    }
    abort();
    return DEC64_ZERO; //bid128_to_binary64(x.get_bid());
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
        return DEC64_NAN;
    }
    // If all digits, then do a large integer conversion.
    if (next == std::string::npos) {
        try {
            return mpz_class(s.substr(skip), 10);
        } catch (std::invalid_argument &) {
            return DEC64_NAN;
        }
    }
    // Check exact allowed formats because bid128_from_string
    // is a bit too permissive.
    if (s[next] == '.') {
        i = next + 1;
        if (i >= s.length()) {
            return DEC64_NAN;
        }
        next = s.find_first_not_of("0123456789", i);
        // Must have one or more digits after decimal point.
        if (next == i) {
            return DEC64_NAN;
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
            return DEC64_NAN;
        }
        // Must have one or more digits in exponent.
        next = s.find_first_not_of("0123456789", i);
        if (next == i) {
            return DEC64_NAN;
        }
    }
    // Reject if there is any trailing junk.
    if (next != std::string::npos) {
        return DEC64_NAN;
    }
    return dec64_from_string(dec64_default_state(), s.c_str() + skip);
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
    } else if (x == std::numeric_limits<int64_t>::min()) {
        mpz_class hi;
        mpz_mul_2exp(hi.get_mpz_t(), mpz_class(0x80000000).get_mpz_t(), 32);
        return mpz_class(-hi);
    } else {
        mpz_class hi;
        mpz_mul_2exp(hi.get_mpz_t(), mpz_class(static_cast<uint32_t>((-x) >> 32)).get_mpz_t(), 32);
        return mpz_class(-(static_cast<uint32_t>((-x) & 0xFFFFFFFF) + hi));
    }
}

Number number_from_float(float x)
{
    return dec64_from_double(x);
}

Number number_from_double(double x)
{
    return dec64_from_double(x);
}
