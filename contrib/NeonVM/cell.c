#include <assert.h>
#include <stdarg.h>
//#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "util.h"

Cell *cell_fromAddress(Cell *c)
{
    Cell *x = cell_newCell(cNothing);
    x->type = cAddress;
    x->address = c;
    return x;
}

Cell * cell_fromArray(Cell * c)
{
    uint64_t i;
    Cell *a = cell_newCell();

    a->type = cArray;
    a->array_size = c->array_size;
    a->array = malloc(sizeof(Cell) * c->array_size);
    for (i = 0; i < c->array_size; i++) {
        cell_copyCell(&a->array[i], &c->array[i]);
    }
    return a;
}

Cell *cell_fromBoolean(BOOL b)
{
    Cell *x = cell_newCell();
    x->type = cBoolean;
    x->boolean = b;
    return x;
}

Cell *cell_fromNumber(Number n)
{
    Cell *x = cell_newCell();
    x->type = cNumber;
    x->number = n;
    return x;
}

Cell *cell_fromString(const char *s, int64_t length)
{
    Cell *x = cell_newCell();
    x->type = cString;
    x->string = string_newString();

    if (length == -1) {
        x->string->length = strlen(s);
    } else {
        x->string->length = length;
    }
    x->string->data = malloc(x->string->length);
    if (!x->string->data) {
        fatal_error("Failed to allocate memory for new cell string.");
    }
    memcpy(x->string->data, s, x->string->length);
    return x;
}

Cell *cell_fromCell(const Cell *c)
{
    assert(c != NULL);

    uint32_t i = 0;
    Cell *x = cell_newCell();
    x->type = c->type;
    switch (c->type) {
        case cAddress:
            cell_copyCell(x, c);
            break;
        case cArray:
            x->array_size = c->array_size;
            x->array = malloc(sizeof(Cell) * c->array_size);
            for (i = 0; i < x->array_size; i++) {
                cell_copyCell(&x->array[i], &c->array[i]);
            }
            x->boolean = FALSE;
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            //x->string->length = 0;
            x->address = NULL;
            break;
        case cBoolean:
            x->boolean = c->boolean;
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            //x->string->length = 0;
            x->address = NULL;
            x->array = NULL;
            x->array_size = 0;
            break;
        case cString:
            x->string = string_copyString(c->string);
            //x->string->length = c->string->length;
            //x->string->data = malloc(x->string->length);
            //memcpy(x->string->data, c->string->data, x->string->length);
            //if (!x->string) {
            //    fatal_error("Failed to allocate memory for cell string.");
            //}
            x->address = NULL;
            x->number = bid128_from_uint32(0);
            x->boolean = FALSE;
            x->array = NULL;
            x->array_size = 0;
            break;
        case cNumber:
            x->number = c->number;
            x->string = NULL;
            x->address = NULL;
            x->boolean = FALSE;
            x->array = NULL;
            x->array_size = 0;
            break;
        case cNothing:
            assert(c->type == cNothing);
            break;
        default:
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
            x->boolean = FALSE;
            x->array = NULL;
            x->array_size = 0;
            break;
    }
    return x;
}

Cell *cell_createArrayCell(uint64_t iElements)
{
    Cell *c = cell_newCell();

    c->type = cArray;
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

void cell_appendArrayElement(Cell *c, const Cell e)
{
    if (c->type == cNothing) {
        c->type = cArray;
    }
    assert(c->type == cArray);
    if (c->array) {
        c->array = realloc(c->array, sizeof(Cell) * (c->array_size + 1));
        if (c->array == NULL) {
            fatal_error("Unable to expand array.");
        }
        c->array_size++;
    }
    if (!c->array) {
        c->array = malloc(sizeof(Cell));
        c->array_size = 1;
        if (c->array == NULL) {
            fatal_error("Unable to allcoate memory for Read Array.");
        }
    }
    cell_copyCell(&c->array[c->array_size-1], &e);
}

BOOL cell_arrayElementExists(const Cell *a, const Cell *e)
{
    uint64_t i;

    assert(a->type == cArray);

    for (i = 0; i < a->array_size; i++) {
        if (cell_compareCell(&a->array[i], e) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

Cell *cell_arrayIndexForRead(Cell *c, size_t i)
{
    if (c->type == cNothing) {
        c->type = cArray;
    }
    assert(c->type == cArray);
    if (!c->array) {
        //c->array = malloc(sizeof(Cell) * i);
        //c->array_size = i;
        //if (c->array == NULL) {
        //    fatal_error("Unable to allcoate memory for Read Array.");
        //}
    }
    return &c->array[i];
}

Cell *cell_arrayIndexForWrite(Cell *c, size_t i)
{
    if (c->type == cNothing) {
        c->type = cArray;
    }
    assert(c->type == cArray);
    if (i >= c->array_size) {
        c->array = realloc(c->array, sizeof(Cell) * (i+1));
        if (c->array == NULL) {
            fatal_error("Unable to reallcoate memory for write array.");
        }
        c->array_size = i+1;
        cell_resetCell(&c->array[i]);
    }
    return &c->array[i];
}

Cell *cell_createDictionaryCell(uint64_t iEntries)
{
    Cell *c = cell_newCell();

    c->type = cDictionary;
    c->dictionary_size = iEntries;

    // ToDo: Implement dictionary Cell Types
    assert(c->type != cDictionary);

    return c;
}

Cell *cell_createStringCell(uint64_t length)
{
    Cell *c = cell_newCell();

    c->type = cString;
    c->string = string_newString();
    c->string->length = length;
    c->string->data = malloc(length);
    if (!c->string->data) {
        fatal_error("Unable to allocate requested string with length 0x%08X.", length);
    }
    return c;
}

void cell_copyCell(Cell *dest, const Cell *source)
{
    assert(source != NULL);
    assert(dest != NULL);

    cell_clearCell(dest);

    dest->number = source->number;
    if (source->type == cString && source->string != NULL) {
        dest->string = string_copyString(source->string);
        //dest->string->length = source->string->length;
        //dest->string = malloc(source->string->length);
        //if (!dest->string) {
        //    fatal_error("Failed to allocate memory for cell string copy.");
        //}
        //memcpy(dest->string, source->string, source->string->length);
    } else {
        dest->string = NULL;
    }
    dest->array_size = source->array_size;
    if (source->type == cArray && source->array != NULL) {
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

int32_t cell_compareCell(const Cell * s, const Cell * d)
{
    assert(s->type == d->type);
    // ToDo: Fix this, so that it actually COMPARES two cells, and returns 0 if equal, -1 if less than, and > 0 if greater than.
    if (s->type != d->type) {
        return -1;
    }

    switch (s->type) {
        case cString:   return string_compareString(s->string, d->string);
        case cAddress:  return s->address == d->address != 0;
        case cBoolean:  return s->boolean == d->boolean != 0;
        case cArray:    return s->array_size == d->array_size != 0;
        case cNumber:   return bid128_quiet_equal(s->number, d->number) != 0;
    }
    return 1;
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
    c->boolean = FALSE;
    c->array = NULL;
    c->array_size = 0;
    c->type = cNothing;
    return c;
}

Cell *cell_newCellType(CellType t)
{
    Cell *c = cell_newCell();
    c->type = t;

    return c;
}

void cell_resetCell(Cell *c)
{
    c->number = bid128_from_uint32(0);
    c->string = NULL;
    c->array = NULL;
    c->array_size = 0;
    c->address = NULL;
    c->boolean = FALSE;
    c->type = cNothing;
}

void cell_clearCell(Cell *c)
{
    assert(c != NULL);

    if (c->type == cString) {
        string_freeString(c->string);
        free(c->string);
    } else if (c->type == cArray) {
        uint64_t i;
        for (i = 0; i < c->array_size; i++) {
            cell_clearCell(&c->array[i]);
        }
        free(c->array);
    }
    cell_resetCell(c);
}

void cell_freeCell(Cell *c)
{
    cell_clearCell(c);
    free(c);
}

