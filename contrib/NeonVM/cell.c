#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "util.h"

#ifdef _MSC_VER
#define strdup(x) _strdup(x)
#endif

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

    Cell *x = cell_newCell();
    x->type = c->type;
    switch (c->type) {
        case Address:
            cell_copyCell(x, c);
            break;
        case Boolean:
            x->boolean = c->boolean;
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
            break;
        case String:
            x->string = strdup(c->string);
            x->address = NULL;
            x->number = bid128_from_uint32(0);
            x->boolean = false;
            break;
        case Number:
            x->number = c->number;
            x->string = NULL;
            x->address = NULL;
            x->boolean = false;
            break;
        case None:
            assert(c->type == None);
        default:
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
            x->boolean = false;
    }
    return x;
}

Cell *cell_fromAddress(Cell *c)
{
    Cell *x = cell_newCell();
    x->type = Address;
    x->address = c;
    return x;
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
    dest->address = source->address;
    dest->boolean = source->boolean;
    dest->type = source->type;
}

void cell_resetCell(Cell *c)
{
    c->number = bid128_from_uint32(0);
    c->string = NULL;
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
    c->type = None;
    return c;
}

void cell_freeCell(Cell *c)
{
    free(c->string);
    free(c);
}
