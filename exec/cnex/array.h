#ifndef ARRAY_H
#define ARRAY_H
#include <stdlib.h>

typedef struct tagTArray {
    size_t size;
    struct tagTCell *data;
} Array;

Array *array_createArray(void);
Array *array_createArrayFromSize(size_t iElements);

void array_freeArray(Array *self);

Array *array_copyArray(Array *a);

#endif
