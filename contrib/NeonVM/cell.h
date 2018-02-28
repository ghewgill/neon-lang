#ifndef __CELL_H
#define __CELL_H

//#define DECIMAL_GLOBAL_ROUNDING 1
//#define DECIMAL_GLOBAL_EXCEPTION_FLAGS 1
//
//#ifndef _WCHAR_T_DEFINED
//#define _WCHAR_T_DEFINED
//#endif
//#include "bid_conf.h"
//#ifdef _MSC_VER
//#pragma warning(push, 0)
//#endif
//#include "bid_functions.h"
//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif
#include "number.h"

typedef enum tagEtype {
    None,
    Address,
    Number,
    String,
} CellType;

typedef struct tagTCell {
    BID_UINT128 number;
    struct tagTCell *address;
    char *string;
    CellType type;
    //enum tagEtype {
    //    None,
    //    Address,
    //    Number,
    //    String,
    //} Type;
} Cell;

Cell *cell_newCell();
void cell_freeCell(Cell *c);

Cell *cell_fromNumber(BID_UINT128 n);
Cell *cell_fromString(const char *s);
Cell *cell_fromCell(const Cell *c);
Cell *cell_fromAddress(Cell *c);
//Cell *cell_copyCell(const Cell *c);
void cell_copyCell(Cell *dest, const Cell *source);
void  cell_resetCell(Cell *c);

#endif
