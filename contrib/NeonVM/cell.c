#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "util.h"

Cell *cell_fromAddress(Cell *c)
{
    Cell *x = cell_newCell();
    x->type = Address;
    x->address = c;
    return x;
}

Cell * cell_fromArray(Cell * c)
{
    uint32_t i;
    Cell *a = cell_newCell();

    a->type = Array;
    a->array_size = c->array_size;
    a->array = malloc(sizeof(Cell) * c->array_size);
    for (i = 0; i < c->array_size; i++) {
        cell_copyCell(&a->array[i], &c->array[i]);
    }
    return a;
}

Cell *cell_fromBoolean(bool b)
{
    Cell *x = cell_newCell();
    x->type = Boolean;
    x->boolean = b;
    return x;
}

Cell *cell_fromNumber(BID_UINT128 n)
{
    Cell *x = cell_newCell();
    x->type = Number;
    x->number = n;
    return x;
}

Cell *cell_fromString(const char *s)
{
    Cell *x = cell_newCell();
    x->type = String;
    x->string = strdup(s);
    return x;
}

Cell *cell_fromCell(const Cell *c)
{
    assert(c != NULL);

    uint32_t i = 0;
    Cell *x = cell_newCell();
    x->type = c->type;
    switch (c->type) {
        case Address:
            cell_copyCell(x, c);
            break;
        case Array:
            x->array_size = c->array_size;
            x->array = malloc(sizeof(Cell) * c->array_size);
            for (i = 0; i < x->array_size; i++) {
                cell_copyCell(&x->array[i], &c->array[i]);
            }
            x->boolean = false;
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
            break;
        case Boolean:
            x->boolean = c->boolean;
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
            x->array = NULL;
            x->array_size = 0;
            break;
        case String:
            x->string = strdup(c->string);
            x->address = NULL;
            x->number = bid128_from_uint32(0);
            x->boolean = false;
            x->array = NULL;
            x->array_size = 0;
            break;
        case Number:
            x->number = c->number;
            x->string = NULL;
            x->address = NULL;
            x->boolean = false;
            x->array = NULL;
            x->array_size = 0;
            break;
        case None:
            assert(c->type == None);
        default:
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
            x->boolean = false;
            x->array = NULL;
            x->array_size = 0;
            break;
    }
    return x;
}

Cell *cell_createArrayCell(uint64_t iElements)
{
    Cell *c = cell_newCell();

    c->type = Array;
    c->array_size = iElements;
    c->array = malloc(sizeof(Cell) * c->array_size);
    if (c->array == NULL) {
        fatal_error("Unable to allocate memory for array.");
    }
    for (uint64_t i = 0; i < c->array_size; i++) {
        cell_resetCell(&c->array[i]);
    }
    return c;
}

Cell *cell_arrayIndexForRead(Cell *c, size_t i)
{
    if (c->type == None) {
        c->type = Array;
    }
    assert(c->type == Array);
    if (!c->array) {
        c->array = malloc(sizeof(Cell) * i);
        c->array_size = i;
        if (c->array == NULL) {
            fatal_error("Unable to allcoate memory for Read Array.");
        }
    }
    return &c->array[i];
}

Cell *cell_arrayIndexForWrite(Cell *c, size_t i)
{
    if (c->type == None) {
        c->type = Array;
    }
    assert(c->type == Array);
    if (!c->array) {
        c->array = malloc(sizeof(Cell) * i);
        if (c->array == NULL) {
            fatal_error("Unable to allcoate memory for Read Array.");
        }
    }

    if (i >= c->array_size) {
        c->array = realloc(c->array, i+1);
        c->array->array_size = i;
    }
    return &c->array[i];
}

void cell_copyCell(Cell *dest, const Cell *source)
{
    assert(source != NULL);
    assert(dest != NULL);

    dest->number = source->number;
    if (source->type == String && source->string != NULL) {
        dest->string = strdup(source->string);
    } else {
        dest->string = NULL;
    }
    dest->array_size = source->array_size;
    if (source->type == Array && source->array != NULL) {
        uint32_t i = 0;
        dest->array = malloc(sizeof(Cell) * dest->array_size);
        for (i = 0; i < dest->array_size; i++) {
            cell_copyCell(&dest->array[i], &source->array[i]);
        }
    } else {
        dest->array = NULL;
    }
    dest->address = source->address;
    dest->boolean = source->boolean;
    dest->type = source->type;
}

void cell_resetCell(Cell *c)
{
    c->number = bid128_from_uint32(0);
    c->string = NULL;
    c->array_size = 0;
    c->array = NULL;
    c->address = NULL;
    c->boolean = false;
    c->type = None;
}

Cell *cell_newCell()
{
    Cell *c = malloc(sizeof(Cell));
    if (c == NULL) {
        fatal_error("Could not allocate new cell object.");
    }

    c->number = bid128_from_uint32(0);
    c->string = NULL;
    c->address = NULL;
    c->boolean = false;
    c->array = NULL;
    c->array_size = 0;
    c->type = None;
    return c;
}

void cell_freeCell(Cell *c)
{
    free(c->string);
    free(c->array);
    free(c);
}
