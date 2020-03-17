#ifndef CELL_H
#define CELL_H
#include <stdint.h>

#include "number.h"
#include "util.h"

typedef enum tagEType {
    cNothing,
    cAddress,
    cArray,
    cBoolean,
    cBytes,
    cDictionary,
    cNumber,
    cObject,
    cString,
    cOther,
} CellType;

typedef struct tagTCell {
    Number number;
    struct tagTCell *address;
    struct tagTArray *array;
    struct tagTDictionary *dictionary;
    struct tagTObject *object;
    struct tagTString *string;
    enum tagEType type;
    BOOL boolean;
    void *other;
} Cell;

Cell *cell_createAddressCell(Cell *a);
Cell *cell_createArrayCell(size_t elements);
Cell *cell_createDictionaryCell(void);
Cell *cell_createStringCell(size_t size);
Cell *cell_createOtherCell(void *ptr);

Cell *cell_newCell(void);
Cell *cell_newCellType(CellType t);

void cell_ensureAddress(Cell *a);
void cell_ensureArray(Cell *a);
void cell_ensureBoolean(Cell *b);
void cell_ensureBytes(Cell *b);
void cell_ensureDictionary(Cell *d);
void cell_ensureNumber(Cell *n);
void cell_ensureObject(Cell *o);
void cell_ensureOther(Cell *o);
void cell_ensureString(Cell *s);

void cell_clearCell(Cell *c);
void cell_freeCell(Cell *c);
void cell_initCell(Cell *c);

Cell *cell_fromAddress(Cell *c);
Cell *cell_fromBoolean(BOOL b);
Cell *cell_fromBytes(struct tagTString *b);
Cell *cell_fromDictionary(struct tagTDictionary *d);
Cell *cell_fromPointer(void *p);
Cell *cell_fromNumber(Number n);
Cell *cell_fromObject(struct tagTObject *o);
Cell *cell_fromCString(const char *s);
Cell *cell_fromString(struct tagTString *s);
Cell *cell_fromStringLength(const char *s, int64_t length);
Cell *cell_fromOther(void *p);
Cell *cell_fromCell(const Cell *c);

void cell_copyCell(Cell *dest, const Cell *source);

int32_t cell_compareCell(const Cell *s, const Cell *d);

void cell_setString(Cell *c, struct tagTString *s);
void cell_setNumber(Cell *c, Number n);
void cell_setBoolean(Cell *c, BOOL b);


struct tagTString *cell_toString(Cell *c);

Cell *cell_arrayIndexForRead(Cell* c, size_t i);
Cell *cell_arrayIndexForWrite(Cell *c, size_t i);
void cell_arrayAppendElement(Cell *c, const Cell e);
void cell_arrayAppendElementPointer(Cell *c, Cell *e);
BOOL cell_arrayElementExists(const Cell *a, const Cell *e);


Cell *cell_dictionaryIndexForWrite(Cell *c, struct tagTString *key);
Cell *cell_dictionaryIndexForRead(Cell *c, struct tagTString *key);

#endif
