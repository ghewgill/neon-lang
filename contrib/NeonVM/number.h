#ifndef __NUMBER_H
#define __NUMBER_H

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

typedef BID_UINT128 Number;

void number_toString(Number x, char *buf, size_t len);
char *number_to_string(Number x);

Number number_modulo(Number x, Number y);
BOOL number_is_integer(Number x);

#endif
