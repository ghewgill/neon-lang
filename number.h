#ifndef NUMBER_H
#define NUMBER_H

#include <string>
#include <stdint.h>

#define NUMBER_DOUBLE

// The Number type is defined as a struct below (containing a member
// of the actual numeric type) so that the rest of the code doesn't
// accidentally use regular arithmetic operations on these numeric
// values.

#if defined(NUMBER_DOUBLE)

typedef struct { double x; } Number;

#elif defined(NUMBER_DECIMAL)

#define DECIMAL_GLOBAL_ROUNDING 1
#define DECIMAL_GLOBAL_EXCEPTION_FLAGS 1

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#endif
#include "bid_conf.h"
#include "bid_functions.h"

typedef struct { BID_UINT64 x; } Number;

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
bool number_is_zero(Number x);
bool number_is_equal(Number x, Number y);
bool number_is_not_equal(Number x, Number y);
bool number_is_less(Number x, Number y);
bool number_is_greater(Number x, Number y);
bool number_is_less_equal(Number x, Number y);
bool number_is_greater_equal(Number x, Number y);
bool number_is_integer(Number x);
std::string number_to_string(Number x);
uint32_t number_to_uint32(Number x);
Number number_from_string(const std::string &s);
Number number_from_uint32(uint32_t x);

#endif
