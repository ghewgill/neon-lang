#ifndef __CELL_H
#define __CELL_H

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

typedef struct tagTCell {
    char *string;
    BID_UINT128 number;
    struct tagTCell *address;

    enum tagEtype {
        None,
        Address,
        Number,
        String,
    } Type;
} Cell;

Cell *cell_newCell();

Cell *cell_fromNumber(BID_UINT128 n);
Cell *cell_fromString(char *s);
Cell *cell_fromCell(Cell *c);
Cell *cell_fromAddress(void *c);
#endif
