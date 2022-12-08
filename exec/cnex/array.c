#include "array.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "util.h"

// ToDo: make dictionary and array pre-malloc sizes adjustable with a command line parameter.
// ToDo: Profile cnex using various starting sizes to figure out what the best starting value would be.
static const size_t ARRAY_INITIAL_SIZE = 8;  // Reasonable starting value.  Will get us in the ballpark.

Array *array_createArray(void)
{
    Array *a = malloc(sizeof(Array));
    if (a == NULL) {
        fatal_error("Unable to allocate array.");
    }

    a->data = NULL;
    a->size = 0;
    a->max = ARRAY_INITIAL_SIZE;
    a->data = malloc(a->max * sizeof(Cell));
    a->refcount = 1;
    return a;
}

Array *array_createArrayFromSize(size_t iElements)
{
    Array *a = malloc(sizeof(Array));
    if (a == NULL) {
        fatal_error("Unable to allocate array for size.");
    }

    a->data = NULL;
    a->size = iElements;
    a->max = (a->size > 0 ? a->size : ARRAY_INITIAL_SIZE);
    a->data = malloc(a->max * sizeof(Cell));
    if (a->data == NULL) {
        fatal_error("Unable to allocate memory for an array of %zu elements.", a->max);
    }
    a->refcount = 1;

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
    // If we're expanding the array, but we don't have any preallocated space left, we need to make some.
    if (newSize > 0 && newSize > self->max) {
        self->max *= 2;
        // Just in case max*2 still isn't enough space, we'll cut our losses here and just allocate what we need.
        if (self->max < newSize) {
            self->max = newSize;
        }
        self->data = realloc(self->data, sizeof(Cell) * self->max);
        if (self->data == NULL) {
            fatal_error("Unable resize array from %zu to %zu elements.", self->size, self->max);
        }
    } else if (newSize == 0) {
        // If the new size ends up being zero, we'll basically reset the array to an initial starting point.
        self->size = 0;
        self->max = ARRAY_INITIAL_SIZE;
        self->data = realloc(self->data, sizeof(Cell) * self->max);
        if (self->data == NULL) {
            fatal_error("Unable resize array.");
        }
    } // Else we have enough elements to fit the new array data in, so no realloc is necessary, at this time.

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
    // Note:  This function only cleans up the elements of the array, not the array itself.  The reference counting
    //        will take care of that for us.  Because of that, we will not adjust the max member either, just the size.
    if (self != NULL && self->data != NULL) {
        for (size_t i = 0; i < self->size; i++) {
            cell_clearCell(&self->data[i]);
        }
        self->size = 0;
    }
}

void array_removeItem(Array *self, size_t index)
{
    if (self != NULL && self->data != NULL) {
        for(size_t i = index; i+1 < self->size; i++) {
            // It's safe to do a reference copy here, since we'll lower the reference count right after, and in fact
            // is faster than having to make a value copy.
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
        // Ensure that any arrays contained inside of the array is actually copied by value, and not by reference.
        cell_valueCopyCell(&r->data[i], &a->data[i]);
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
    assert(self);
    assert(self->data);

    array_resizeArray(self, self->size + 1);

    cell_copyCell(&self->data[self->size-1], element);
    return self->size;
}
