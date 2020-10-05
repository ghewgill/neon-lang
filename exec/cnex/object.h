#ifndef OBJECT_H
#define OBJECT_H
#include <stdint.h>

#include "array.h"
#include "dictionary.h"
#include "number.h"
#include "nstring.h"
#include "util.h"

struct MmapObject;

typedef enum tagNObjectType {
    oNone,
    oArray,
    oBoolean,
    oBytes,
    oDictionary,
    oNumber,
    oString,
} ObjectType;

typedef struct tagTObject {
    ObjectType      type;
    int             refcount;
    void            *ptr;

    void (*release)(struct tagTObject*);
    Cell *(*toString)(struct tagTObject*);
} Object;

Object *object_createObject(void);

void object_releaseObject(Object *o);

Cell *object_toLiteralString(Object *s);
Cell *object_toString(Object *s);

Object *object_createArrayObject(Array *a);
Object *object_createBooleanObject(BOOL b);
Object *object_createBytesObject(TString *b);
Object *object_createDictionaryObject(Dictionary *d);
Object *object_createNumberObject(Number n);
Object *object_createStringObject(TString *s);

Object *object_createFileObject(FILE *f);
Object *object_createProcessObject(void *h);
Object *object_createMMapObject(struct MmapObject *m);

Object *object_fromCell(Cell *c);

#endif
