#ifndef ARRAY_H
#define ARRAY_H
#include <stdlib.h>

typedef struct tagTArray {
    size_t size;
    size_t max;
    int refcount;
    struct tagTCell *data;
} Array;

Array *array_createArray(void);
Array *array_createArrayFromSize(size_t iElements);
Array *array_resizeArray(Array *self, size_t newSize);

void array_clearArray(Array *self);
void array_freeArray(Array *self);
void array_removeItem(Array *self, size_t index);

Array *array_copyArray(Array *a);

int array_compareArray(Array *l, Array *r);

size_t array_appendElement(Array *self, const struct tagTCell *element);

#endif
