#ifndef OBJECT_H
#define OBJECT_H
#include <stdint.h>

#include "array.h"
#include "dictionary.h"
#include "number.h"
#include "nstring.h"
#include "util.h"

typedef enum tagNObjectType {
    oNone,
    oArray,
    oBoolean,
    oBytes,
    oDictionary,
    oNumber,
    oOther,
    oString,
} ObjectType;

typedef struct tagTObject {
    ObjectType      type;
    int             refcount;
    struct tagTCell *cell;

    struct tagTCell *(*toString)(struct tagTObject*);
    void (*object_releaseObject)(struct tagTObject*);
} Object;

Object *object_createObject(void);

void object_releaseObject(Object *o);

Cell *object_toString(Object *s);

Object *object_createArrayObject(Array *a);
Object *object_createBooleanObject(BOOL b);
Object *object_createBytesObject(TString *b);
Object *object_createDictionaryObject(Dictionary *d);
Object *object_createNumberObject(Number n);
Object *object_createOtherObject(void *p);
Object *object_createStringObject(TString *s);

Object *object_fromCell(Cell *c);

#endif
