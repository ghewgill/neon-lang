#ifndef NUMBER_H
#define NUMBER_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

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

#include "util.h"

#define BID_ZERO        bid128_from_uint32(0)

#define BID_MIN_INT32   bid128_from_int32(INT_MIN)
#define BID_MAX_INT32   bid128_from_int32(INT_MAX)
#define BID_MIN_UINT32  bid128_from_uint32(0)
#define BID_MAX_UINT32  bid128_from_uint32(UINT_MAX)

#define BID_MIN_INT64   bid128_from_int64(LLONG_MIN)
#define BID_MAX_INT64   bid128_from_int64(LLONG_MAX)
#define BID_MIN_UINT64  bid128_from_uint64(0)
#define BID_MAX_UINT64  bid128_from_uint64(ULLONG_MAX)


typedef BID_UINT128 Number;

void number_toString(Number x, char *buf, size_t len);
char *number_to_string(Number x);

Number number_from_string(char *s);

int32_t number_to_sint32(Number x);
uint32_t number_to_uint32(Number x);
int64_t number_to_sint64(Number x);
uint64_t number_to_uint64(Number x);

Number number_divide(Number x, Number y);
Number number_modulo(Number x, Number y);
Number number_multiply(Number x, Number y);
BOOL number_is_greater(Number x, Number y);
BOOL number_is_integer(Number x);
BOOL number_is_nan(Number x);
Number number_nearbyint(Number x);
Number number_trunc(Number x);

//Number number_from_uint8(uint8_t x);
//Number number_from_sint8(int8_t x);
//Number number_from_uint16(uint16_t x);
//Number number_from_sint16(int16_t x);
Number number_from_uint32(uint32_t x);
Number number_from_sint32(int32_t x);
Number number_from_uint64(uint64_t x);
Number number_from_sint64(int64_t x);
//Number number_from_float(float x);
//Number number_from_double(double x);

BOOL number_is_zero(Number x);
BOOL number_is_negative(Number x);
BOOL number_is_equal(Number x, Number y);
//BOOL number_is_not_equal(Number x, Number y);
BOOL number_is_less(Number x, Number y);
//BOOL number_is_greater(Number x, Number y);
//BOOL number_is_less_equal(Number x, Number y);
//BOOL number_is_greater_equal(Number x, Number y);
//BOOL number_is_integer(Number x);
BOOL number_is_odd(Number x);
//BOOL number_is_nan(Number x);

#endif
