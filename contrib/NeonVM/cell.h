#ifndef __CELL_H
#define __CELL_H
#include <stdbool.h>
#include <stdint.h>

#include "number.h"

#ifdef _MSC_VER
#define strdup(x) _strdup(x)
#endif

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
    uint64_t array_size;
    char *string;
    CellType type;
    bool boolean;
} Cell;

Cell *cell_newCell();
void cell_freeCell(Cell *c);

Cell *cell_fromBoolean(bool b);
Cell *cell_fromNumber(BID_UINT128 n);
Cell *cell_fromString(const char *s);
Cell *cell_fromCell(const Cell *c);
Cell *cell_fromAddress(Cell *c);
Cell *cell_fromArray(Cell *c);

void cell_copyCell(Cell *dest, const Cell *source);
void cell_resetCell(Cell *c);

Cell *cell_createArrayCell(uint64_t iElements);
Cell *cell_arrayIndexForRead(Cell* c, size_t i);
Cell *cell_arrayIndexForWrite(Cell *c, size_t i);

#endif
