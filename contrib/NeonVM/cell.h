#ifndef __CELL_H
#define __CELL_H
#include <stdint.h>

#include "dictionary.h"
#include "number.h"
#include "string.h"
#include "util.h"

typedef enum tagEType {
    cNothing,
    cAddress,
    cArray,
    cBoolean,
    cDictionary,
    cNumber,
    cString,
} CellType;

typedef struct tagTCell {
    Number number;
    struct tagTCell *address;
    struct tagTCell *array;
    uint64_t array_size;
    struct tagTDictionary *dictionary; // ToDo: Implement dictionary Cell Types
    uint64_t dictionary_size;
    struct tagTString *string;
    CellType type;
    BOOL boolean;
} Cell;

Cell *cell_createArrayCell(uint64_t elements);
Cell *cell_createDictionaryCell(uint64_t elements);
Cell *cell_createStringCell(uint64_t size);

Cell *cell_newCell();
Cell *cell_newCellType(CellType t);

void cell_clearCell(Cell *c);
void cell_freeCell(Cell *c);
void cell_resetCell(Cell *c);

Cell *cell_fromAddress(Cell *c);
Cell *cell_fromArray(Cell *c);
Cell *cell_fromBoolean(BOOL b);
Cell *cell_fromNumber(Number n);
Cell *cell_fromString(const char *s, int64_t length);

Cell *cell_fromCell(const Cell *c);

void cell_copyCell(Cell *dest, const Cell *source);

int32_t cell_compareCell(const Cell *s, const Cell *d);

Cell *cell_arrayIndexForRead(Cell* c, size_t i);
Cell *cell_arrayIndexForWrite(Cell *c, size_t i);
void cell_arrayAppendElement(Cell *c, const Cell e);
BOOL cell_arrayElementExists(const Cell *a, const Cell *e);
Cell *cell_arrayForWrite(Cell *c);

#endif
