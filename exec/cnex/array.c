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
    a->refcount = 1;
    return a;
}

Array *array_createArrayFromSize(size_t iElements)
{
    Array *a = array_createArray();

    a->size = iElements;
    a->data = malloc(sizeof(Cell) * a->size);
    a->refcount = 1;
    if (a->data == NULL) {
        fatal_error("Unable to allocate memory for %d array elements.", a->size);
    }
    for (size_t i = 0; i < a->size; i++) {
        cell_initCell(&a->data[i]);
    }
    return a;
}

Array *array_resizeArray(Array *self, size_t newSize)
{
    if (self == NULL) {
        self = array_createArray();
    }

    // If we're shrinking the array, then we need to free the elements at the cutoff point, to prevent a memory leak.
    if (newSize < self->size) {
        for (size_t i = newSize; i < self->size; i++) {
            cell_clearCell(&self->data[i]);
        }
    }
    if (newSize > 0) {
        self->data = realloc(self->data, sizeof(Cell) * newSize);
        if (self->data == NULL) {
            fatal_error("Unable resize array from %zu to %zu elements.", self->size, newSize);
        }
    } else {
        // If the new size ends up being zero, fine, we won't realloc it, since we'd have already cleaned up
        // all of the array elements above, so there's no harm in leaving the array container allocated for more
        // than we're using.  It'll all come out in the wash.
        self->size = 0;
    }

    // Initialize any new elements we may have added, but skip any that may have been there before.
    for (size_t i = self->size; i < newSize; i++) {
        cell_initCell(&self->data[i]);
    }
    // Finally, assign the size, and return ourself.
    self->size = newSize;
    return self;
}

void array_freeArray(Array *self)
{
    if (self != NULL) {
        self->refcount--;
        if (self->refcount <= 0) {
            if (self->data != NULL) {
                for (size_t i = 0; i < self->size; i++) {
                    cell_clearCell(&self->data[i]);
                }
                free(self->data);
            }
            free(self);
        }
    }
}

void array_clearArray(Array *self)
{
    if (self != NULL && self->data != NULL) {
        for (size_t i = 0; i < self->size; i++) {
            cell_clearCell(&self->data[i]);
        }
        free(self->data);
        self->data = NULL;
        self->size = 0;
    }
}

void array_removeItem(Array *self, size_t index)
{
    if (self != NULL && self->data != NULL) {
        for(size_t i = index; i+1 < self->size; i++) {
            cell_copyCell(&self->data[i], &self->data[i+1]);
            cell_clearCell(&self->data[i+1]);
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
    // Obviously, if the two arrays point to the same array; then they're equal!
    if (l == r) {
        return TRUE;
    }

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

size_t array_appendElement(Array *self, const Cell *element)
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
    cell_initCell(&self->data[self->size-1]);
    cell_copyCell(&self->data[self->size-1], element);

    return self->size;
}
