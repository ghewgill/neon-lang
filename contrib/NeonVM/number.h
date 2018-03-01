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

void number_toString(BID_UINT128 x, char *buf, size_t len);

BID_UINT128 number_modulo(BID_UINT128 x, BID_UINT128 y);

#endif
