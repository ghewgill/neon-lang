#ifndef _CELL_H
#define _CELL_H
#include <stdint.h>

#include "number.h"
#include "util.h"

typedef enum tagEType {
    cNothing,
    cAddress,
    cArray,
    cBoolean,
    cDictionary,
    cNumber,
    cPointer,
    cString,
} CellType;

typedef struct tagTCell {
    Number number;
    struct tagTCell *address;
    struct tagTCell *array;
    size_t array_size;
    struct tagTDictionary *dictionary;
    struct tagTString *string;
    enum tagEType type;
    BOOL boolean;
} Cell;

Cell *cell_createAddressCell(Cell *a);
Cell *cell_createArrayCell(size_t elements);
Cell *cell_createDictionaryCell(void);
Cell *cell_createStringCell(size_t size);

Cell *cell_newCell(void);
Cell *cell_newCellType(CellType t);

void cell_clearCell(Cell *c);
void cell_freeCell(Cell *c);
void cell_resetCell(Cell *c);

Cell *cell_fromAddress(Cell *c);
Cell *cell_fromArray(Cell *c);
Cell *cell_fromBoolean(BOOL b);
Cell *cell_fromPointer(void *p);
Cell *cell_fromNumber(Number n);
Cell *cell_fromCString(const char *s);
Cell *cell_fromStringLength(const char *s, int64_t length);
Cell *cell_fromCell(const Cell *c);

void cell_copyCell(Cell *dest, const Cell *source);

int32_t cell_compareCell(const Cell *s, const Cell *d);

void cell_setString(Cell *c, struct tagTString *s);
void cell_setNumber(Cell *c, Number n);
void cell_setBoolean(Cell *c, BOOL b);


Cell *cell_arrayIndexForRead(Cell* c, size_t i);
Cell *cell_arrayIndexForWrite(Cell *c, size_t i);
void cell_arrayAppendElement(Cell *c, const Cell e);
BOOL cell_arrayElementExists(const Cell *a, const Cell *e);
Cell *cell_arrayForWrite(Cell *c);


Cell *cell_dictionaryIndexForWrite(Cell *c, struct tagTString *key);
Cell *cell_dictionaryIndexForRead(Cell *c, struct tagTString *key);

#endif
