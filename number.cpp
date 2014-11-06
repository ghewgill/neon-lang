#include "number.h"

#if defined(NUMBER_DOUBLE)

#include <math.h>

Number number_add(Number x, Number y)
{
    return x.x + y.x;
}

Number number_subtract(Number x, Number y)
{
    return x.x - y.x;
}

Number number_multiply(Number x, Number y)
{
    return x.x * y.x;
}

Number number_divide(Number x, Number y)
{
    return x.x / y.x;
}

Number number_modulo(Number x, Number y)
{
    return fmod(x.x, y.x);
}

Number number_pow(Number x, Number y)
{
    return pow(x.x, y.x);
}

Number number_negate(Number x)
{
    return - x.x;
}

Number number_abs(Number x)
{
    return fabs(x.x);
}

Number number_ceil(Number x)
{
    return ceil(x.x);
}

Number number_floor(Number x)
{
    return floor(x.x);
}

Number number_exp(Number x)
{
    return exp(x.x);
}

Number number_log(Number x)
{
    return log(x.x);
}

Number number_sqrt(Number x)
{
    return sqrt(x.x);
}

Number number_acos(Number x)
{
    return acos(x.x);
}

Number number_asin(Number x)
{
    return asin(x.x);
}

Number number_atan(Number x)
{
    return atan(x.x);
}

Number number_cos(Number x)
{
    return cos(x.x);
}

Number number_sin(Number x)
{
    return sin(x.x);
}

Number number_tan(Number x)
{
    return tan(x.x);
}

bool number_is_zero(Number x)
{
    return x.x == 0;
}

bool number_is_equal(Number x, Number y)
{
    return x.x == y.x;
}

bool number_is_not_equal(Number x, Number y)
{
    return x.x != y.x;
}

bool number_is_less(Number x, Number y)
{
    return x.x < y.x;
}

bool number_is_greater(Number x, Number y)
{
    return x.x > y.x;
}

bool number_is_less_equal(Number x, Number y)
{
    return x.x <= y.x;
}

bool number_is_greater_equal(Number x, Number y)
{
    return x.x >= y.x;
}

bool number_is_integer(Number x)
{
    return x.x == trunc(x.x);
}

std::string number_to_string(Number x)
{
    std::string r = std::to_string(x.x);
    
    // TODO: This hack converts a canonical string like 42.00000 to just 42
    const auto d = r.find('.');
    if (d != std::string::npos) {
        auto i = d + 1;
        while (i < r.length() && r.at(i) == '0') {
            i++;
        }
        if (i >= r.length()) {
            r = r.substr(0, d);
        }
    }

    return r;
}

uint32_t number_to_uint32(Number x)
{
    return trunc(x.x);
}

Number number_from_string(const std::string &s)
{
    return stod(s);
}

Number number_from_uint32(uint32_t x)
{
    return x;
}

Number number_from_uint64(uint64_t x)
{
    return x;
}

#elif defined(NUMBER_DECIMAL)

Number number_add(Number x, Number y)
{
    return bid64_add(x.x, y.x);
}

Number number_subtract(Number x, Number y)
{
    return bid64_sub(x.x, y.x);
}

Number number_multiply(Number x, Number y)
{
    return bid64_mul(x.x, y.x);
}

Number number_divide(Number x, Number y)
{
    // TODO: division by zero
    return bid64_div(x.x, y.x);
}

Number number_modulo(Number x, Number y)
{
    // TODO: division by zero
    return bid64_rem(x.x, y.x);
}

Number number_pow(Number x, Number y)
{
    return bid64_pow(x.x, y.x);
}

Number number_negate(Number x)
{
    return bid64_negate(x.x);
}

Number number_abs(Number x)
{
    return bid64_abs(x.x);
}

Number number_ceil(Number x)
{
    // TODO: find proper ceil function to handle full range of decimal64
    return bid64_from_int64(bid64_to_int64_ceil(x.x));
}

Number number_floor(Number x)
{
    // TODO: find proper floor function to handle full range of decimal64
    return bid64_from_int64(bid64_to_int64_floor(x.x));
}

Number number_exp(Number x)
{
    return bid64_exp(x.x);
}

Number number_log(Number x)
{
    return bid64_log(x.x);
}

Number number_sqrt(Number x)
{
    return bid64_sqrt(x.x);
}

Number number_acos(Number x)
{
    return bid64_acos(x.x);
}

Number number_asin(Number x)
{
    return bid64_asin(x.x);
}

Number number_atan(Number x)
{
    return bid64_atan(x.x);
}

Number number_cos(Number x)
{
    return bid64_cos(x.x);
}

Number number_sin(Number x)
{
    return bid64_sin(x.x);
}

Number number_tan(Number x)
{
    return bid64_tan(x.x);
}

bool number_is_zero(Number x)
{
    return bid64_isZero(x.x) != 0;
}

bool number_is_equal(Number x, Number y)
{
    return bid64_quiet_equal(x.x, y.x) != 0;
}

bool number_is_not_equal(Number x, Number y)
{
    return bid64_quiet_not_equal(x.x, y.x) != 0;
}

bool number_is_less(Number x, Number y)
{
    return bid64_quiet_less(x.x, y.x) != 0;
}

bool number_is_greater(Number x, Number y)
{
    return bid64_quiet_greater(x.x, y.x) != 0;
}

bool number_is_less_equal(Number x, Number y)
{
    return bid64_quiet_less_equal(x.x, y.x) != 0;
}

bool number_is_greater_equal(Number x, Number y)
{
    return bid64_quiet_greater_equal(x.x, y.x) != 0;
}

bool number_is_integer(Number x)
{
    BID_UINT64 i = bid64_round_integral_zero(x.x);
    return bid64_quiet_equal(x.x, i) != 0;
}

std::string number_to_string(Number x)
{
    char buf[40];
    bid64_to_string(buf, x.x);
    std::string sbuf(buf);

    // TODO: This hack converts a canonical string like +42E+0 to just 42
    if (sbuf.length() >= 3 && sbuf.substr(sbuf.length()-3) == "E+0") {
        sbuf = sbuf.substr(0, sbuf.length()-3);
    }
    if (sbuf.at(0) == '+') {
        sbuf = sbuf.substr(1);
    }

    // TODO: This hack converts xxxx0000E-4 to just xxxx.
    const char *p = sbuf.data() + sbuf.length();
    while (p > sbuf.data() && isdigit(p[-1])) {
        p--;
    }
    if (p > sbuf.data() && p[-1] == '-') {
        p--;
        if (p > sbuf.data() && p[-1] == 'E') {
            p--;
            int z = std::stoi(p+2);
            while (z > 0 && p > sbuf.data() && p[-1] == '0') {
                p--;
                z--;
            }
            if (z == 0) {
                sbuf = sbuf.substr(0, p-sbuf.data());
            }
        }
    }

    // TODO: This hack converts 0Exxxx to just 0.
    if (sbuf.substr(0, 2) == "0E") {
        sbuf = "0";
    }

    return sbuf;
}

uint32_t number_to_uint32(Number x)
{
    return bid64_to_uint32_int(x.x);
}

Number number_from_string(const std::string &s)
{
    return bid64_from_string(const_cast<char *>(s.c_str()));
}

Number number_from_uint32(uint32_t x)
{
    return bid64_from_uint32(x);
}

Number number_from_uint64(uint64_t x)
{
    return bid64_from_uint64(x);
}

#endif
