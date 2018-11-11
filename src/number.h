#ifndef NUMBER_H
#define NUMBER_H

#include <stdint.h>
#include <string>

// The Number type is defined as a struct below (containing a member
// of the actual numeric type) so that the rest of the code doesn't
// accidentally use regular arithmetic operations on these numeric
// values.

#include <mpfr.h>

struct Number {
    Number() { mpfr_init(x); }
    Number(const Number &rhs) { mpfr_init_set(x, rhs.x, MPFR_RNDN); }
    ~Number() { mpfr_clear(x); }
    Number &operator=(const Number &rhs) {
        if (&rhs == this) {
            return *this;
        }
        mpfr_set(x, rhs.x, MPFR_RNDN);
        return *this;
    }
private:
    mpfr_t x;
    //Number(const mpfr_t &x) { mpfr_init_set(this->x, x, MPFR_RNDN); }

    friend Number number_add(Number x, Number y);
    friend Number number_subtract(Number x, Number y);
    friend Number number_multiply(Number x, Number y);
    friend Number number_divide(Number x, Number y);
    friend Number number_modulo(Number x, Number y);
    friend Number number_pow(Number x, Number y);
    friend Number number_negate(Number x);
    friend Number number_abs(Number x);
    friend Number number_sign(Number x);
    friend Number number_ceil(Number x);
    friend Number number_floor(Number x);
    friend Number number_trunc(Number x);
    friend Number number_exp(Number x);
    friend Number number_log(Number x);
    friend Number number_sqrt(Number x);
    friend Number number_acos(Number x);
    friend Number number_asin(Number x);
    friend Number number_atan(Number x);
    friend Number number_cos(Number x);
    friend Number number_sin(Number x);
    friend Number number_tan(Number x);
    friend Number number_acosh(Number x);
    friend Number number_asinh(Number x);
    friend Number number_atanh(Number x);
    friend Number number_atan2(Number y, Number x);
    friend Number number_cbrt(Number x);
    friend Number number_cosh(Number x);
    friend Number number_erf(Number x);
    friend Number number_erfc(Number x);
    friend Number number_exp2(Number x);
    friend Number number_expm1(Number x);
    friend Number number_frexp(Number x, int *exp);
    friend Number number_hypot(Number x, Number y);
    friend Number number_ldexp(Number x, int exp);
    friend Number number_lgamma(Number x);
    friend Number number_log10(Number x);
    friend Number number_log1p(Number x);
    friend Number number_log2(Number x);
    friend Number number_nearbyint(Number x);
    friend Number number_sinh(Number x);
    friend Number number_tanh(Number x);
    friend Number number_tgamma(Number x);
    friend bool number_is_zero(Number x);
    friend bool number_is_negative(Number x);
    friend bool number_is_equal(Number x, Number y);
    friend bool number_is_not_equal(Number x, Number y);
    friend bool number_is_less(Number x, Number y);
    friend bool number_is_greater(Number x, Number y);
    friend bool number_is_less_equal(Number x, Number y);
    friend bool number_is_greater_equal(Number x, Number y);
    friend bool number_is_integer(Number x);
    friend bool number_is_odd(Number x);
    friend bool number_is_nan(Number x);
    friend std::string number_to_string(Number x);
    friend uint8_t number_to_uint8(Number x);
    friend int8_t number_to_sint8(Number x);
    friend uint16_t number_to_uint16(Number x);
    friend int16_t number_to_sint16(Number x);
    friend uint32_t number_to_uint32(Number x);
    friend int32_t number_to_sint32(Number x);
    friend uint64_t number_to_uint64(Number x);
    friend int64_t number_to_sint64(Number x);
    friend float number_to_float(Number x);
    friend double number_to_double(Number x);
    friend Number number_from_string(const std::string &s);
    friend Number number_from_uint8(uint8_t x);
    friend Number number_from_sint8(int8_t x);
    friend Number number_from_uint16(uint16_t x);
    friend Number number_from_sint16(int16_t x);
    friend Number number_from_uint32(uint32_t x);
    friend Number number_from_sint32(int32_t x);
    friend Number number_from_uint64(uint64_t x);
    friend Number number_from_sint64(int64_t x);
    friend Number number_from_float(float x);
    friend Number number_from_double(double x);
};

Number number_add(Number x, Number y);
Number number_subtract(Number x, Number y);
Number number_multiply(Number x, Number y);
Number number_divide(Number x, Number y);
Number number_modulo(Number x, Number y);
Number number_pow(Number x, Number y);
Number number_negate(Number x);
Number number_abs(Number x);
Number number_sign(Number x);
Number number_ceil(Number x);
Number number_floor(Number x);
Number number_trunc(Number x);
Number number_exp(Number x);
Number number_log(Number x);
Number number_sqrt(Number x);
Number number_acos(Number x);
Number number_asin(Number x);
Number number_atan(Number x);
Number number_cos(Number x);
Number number_sin(Number x);
Number number_tan(Number x);
Number number_acosh(Number x);
Number number_asinh(Number x);
Number number_atanh(Number x);
Number number_atan2(Number y, Number x);
Number number_cbrt(Number x);
Number number_cosh(Number x);
Number number_erf(Number x);
Number number_erfc(Number x);
Number number_exp2(Number x);
Number number_expm1(Number x);
Number number_frexp(Number x, int *exp);
Number number_hypot(Number x, Number y);
Number number_ldexp(Number x, int exp);
Number number_lgamma(Number x);
Number number_log10(Number x);
Number number_log1p(Number x);
Number number_log2(Number x);
Number number_nearbyint(Number x);
Number number_sinh(Number x);
Number number_tanh(Number x);
Number number_tgamma(Number x);
bool number_is_zero(Number x);
bool number_is_negative(Number x);
bool number_is_equal(Number x, Number y);
bool number_is_not_equal(Number x, Number y);
bool number_is_less(Number x, Number y);
bool number_is_greater(Number x, Number y);
bool number_is_less_equal(Number x, Number y);
bool number_is_greater_equal(Number x, Number y);
bool number_is_integer(Number x);
bool number_is_odd(Number x);
bool number_is_nan(Number x);
std::string number_to_string(Number x);
uint8_t number_to_uint8(Number x);
int8_t number_to_sint8(Number x);
uint16_t number_to_uint16(Number x);
int16_t number_to_sint16(Number x);
uint32_t number_to_uint32(Number x);
int32_t number_to_sint32(Number x);
uint64_t number_to_uint64(Number x);
int64_t number_to_sint64(Number x);
float number_to_float(Number x);
double number_to_double(Number x);
Number number_from_string(const std::string &s);
Number number_from_uint8(uint8_t x);
Number number_from_sint8(int8_t x);
Number number_from_uint16(uint16_t x);
Number number_from_sint16(int16_t x);
Number number_from_uint32(uint32_t x);
Number number_from_sint32(int32_t x);
Number number_from_uint64(uint64_t x);
Number number_from_sint64(int64_t x);
Number number_from_float(float x);
Number number_from_double(double x);

#endif
