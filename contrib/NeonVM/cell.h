#ifndef __CELL_H
#define __CELL_H
#include <stdbool.h>

#include "number.h"

#ifdef _MSC_VER
#define strdup(x) _strdup(x)
#endif

typedef unsigned char       uint8_t;
typedef signed   char       int8_t;
typedef unsigned short      uint16_t;
typedef signed   short      int16_t;
typedef unsigned int        uint32_t;
typedef signed   int        int32_t;

typedef enum tagEtype {
    None,
    Address,
    Array,
    Boolean,
    Number,
    String,
} CellType;

typedef struct tagTCell {
    BID_UINT128 number;
    struct tagTCell *address;
    struct tagTCell *array;
    uint32_t array_size;
    char *string;
    CellType type;
    bool boolean;
} Cell;

Cell *cell_newCell();
void cell_freeCell(Cell *c);
Cell *cell_createArrayCell(uint32_t iElements);

Cell *cell_fromBoolean(bool b);
Cell *cell_fromNumber(BID_UINT128 n);
Cell *cell_fromString(const char *s);
Cell *cell_fromCell(const Cell *c);
Cell *cell_fromAddress(Cell *c);
Cell *cell_fromArray(Cell *c);

void cell_copyCell(Cell *dest, const Cell *source);
void cell_resetCell(Cell *c);

#endif
