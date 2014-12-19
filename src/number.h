#ifndef NUMBER_H
#define NUMBER_H

#include <stdint.h>
#include <string>

#define NUMBER_DECIMAL

// The Number type is defined as a struct below (containing a member
// of the actual numeric type) so that the rest of the code doesn't
// accidentally use regular arithmetic operations on these numeric
// values.

#if defined(NUMBER_DOUBLE)

struct Number {
    Number(): x(0) {}
    Number(double x): x(x) {}
    double x;
};

#elif defined(NUMBER_DECIMAL)

#define DECIMAL_GLOBAL_ROUNDING 1
#define DECIMAL_GLOBAL_EXCEPTION_FLAGS 1

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#endif
#include "bid_conf.h"
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "bid_functions.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

struct Number {
    Number(): x(bid64_from_uint32(0)) {}
    Number(BID_UINT64 x): x(x) {}
    BID_UINT64 x;
};

#else

#error Must define NUMBER_DOUBLE or NUMBER_DECIMAL

#endif

Number number_add(Number x, Number y);
Number number_subtract(Number x, Number y);
Number number_multiply(Number x, Number y);
Number number_divide(Number x, Number y);
Number number_modulo(Number x, Number y);
Number number_pow(Number x, Number y);
Number number_negate(Number x);
Number number_abs(Number x);
Number number_ceil(Number x);
Number number_floor(Number x);
Number number_exp(Number x);
Number number_log(Number x);
Number number_sqrt(Number x);
Number number_acos(Number x);
Number number_asin(Number x);
Number number_atan(Number x);
Number number_cos(Number x);
Number number_sin(Number x);
Number number_tan(Number x);
bool number_is_zero(Number x);
bool number_is_equal(Number x, Number y);
bool number_is_not_equal(Number x, Number y);
bool number_is_less(Number x, Number y);
bool number_is_greater(Number x, Number y);
bool number_is_less_equal(Number x, Number y);
bool number_is_greater_equal(Number x, Number y);
bool number_is_integer(Number x);
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
