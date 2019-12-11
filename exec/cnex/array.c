#include "array.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "util.h"

Array *array_createArray(void)
{
    Array *a = malloc(sizeof(Array));
    if (a == NULL) {
        fatal_error("Unable to allocate array.");
    }

    a->size = 0;
    a->data = NULL;
    return a;
}

Array *array_createArrayFromSize(size_t iElements)
{
    Array *a = array_createArray();

    a->size = iElements;
    a->data = malloc(sizeof(Cell) * a->size);
    if (a->data == NULL) {
        fatal_error("Unable to allocate memory for %d array elements.", a->size);
    }
    for (size_t i = 0; i < a->size; i++) {
        cell_resetCell(&a->data[i]);
    }
    return a;
}

Array *array_expandArray(Array *self, size_t iElements)
{
    if (self == NULL) {
        self = malloc(sizeof(Array));
        if (self == NULL) {
            fatal_error("Unable to allocate array for array expansion.");
        }
        self->size = 0;
        self->data = NULL;
    }

    self->data = realloc(self->data, sizeof(Cell) * (self->size + iElements));
    if (self->data == NULL) {
        fatal_error("Unable to allocate memory for %d expanded array elements.", self->size + iElements);
    }

    for (size_t i = self->size; i < self->size + iElements; i++) {
        cell_resetCell(&self->data[i]);
    }
    self->size += iElements;
    return self;
}

void array_freeArray(Array *self)
{
    if (self != NULL && self->data != NULL) {
        for (size_t i = 0; i < self->size; i++) {
            cell_clearCell(&self->data[i]);
        }
        free(self->data);
    }
    free(self);
}

void array_removeItem(Array *self, size_t index)
{
    if (self != NULL && self->data != NULL) {
        for(size_t i = index; i+1 < self->size; i++) {
            cell_copyCell(&self->data[i], &self->data[i+1]);
        }
        self->size--;
    }
}

Array *array_copyArray(Array *a)
{
    Array *r = array_createArrayFromSize(a->size);

    for (size_t i = 0; i < r->size; i++) {
        cell_copyCell(&r->data[i], &a->data[i]);
    }
    return r;
}

int array_compareArray(Array *l, Array *r)
{
    if (l->size != r->size) {
        return FALSE;
    }

    for (size_t i = 0; i < l->size; i++) {
        if (cell_compareCell(&l->data[i], &r->data[i]) != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

size_t array_appendElement(Array *self, Cell *element)
{
    if (self == NULL) {
        fatal_error("Uninitialized array passed to array_appendElement!");
    }
    if (self->data) {
        self->data = realloc(self->data, sizeof(Cell) * (self->size + 1));
        if (self->data == NULL) {
            fatal_error("Not enough memory to expand array.");
        }
        self->size++;
    }
    if (self->data == NULL) {
        self->data = malloc(sizeof(Cell));
        if (self->data == NULL) {
            fatal_error("Failed to allocate memory array element.");
        }
        self->size = 1;
    }
    cell_resetCell(&self->data[self->size-1]);
    cell_copyCell(&self->data[self->size-1], element);

    return self->size;
}
