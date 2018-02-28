#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "util.h"

Cell *cell_fromNumber(BID_UINT128 n)
{
    Cell *x = malloc(sizeof(Cell));
    if (x == NULL) {
        fatal_error("Could not allocate new number object.");
    }
    x->type = Number;
    x->number = n;
    return x;
}

Cell *cell_fromString(const char *s)
{
    Cell *x = malloc(sizeof(Cell));
    if (x == NULL) {
        fatal_error("Could not allocate new string object.");
    }
    x->type = String;
    x->string = _strdup(s);
    return x;
}

Cell *cell_fromCell(const Cell *c)
{
    Cell *x = malloc(sizeof(Cell));
    if (x == NULL) {
        fatal_error("Could not allocate new Cell object.");
    }
    assert(c != NULL);
    x->type = c->type;
    switch (c->type) {
        case Address:
            memcpy(x, c, sizeof(Cell));
            //x->address = c->address;
            break;
        case String:
            x->string = _strdup(c->string);
            x->number = bid128_from_uint32(0);
            x->address = NULL;
            break;
        case Number:
            x->number = c->number;
            x->string = NULL;
            x->address = NULL;
            break;
        case None:
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = c->address;
            break;
        default:
            x->number = bid128_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
    }
    return x;
}

Cell *cell_fromAddress(Cell *c)
{
    Cell *x = cell_fromCell(c);
    //x->address = c;
    //x->type = Address;
    return x;
}

void cell_resetCell(Cell *c)
{
    c->number = bid128_from_uint32(0);
    c->string = NULL;
    c->address = NULL;
    c->type = None;
}

Cell *cell_newCell()
{
    Cell *c = malloc(sizeof(Cell));
    // TODO: MemCheck
    c->number = bid128_from_uint32(0);
    c->string = NULL;
    c->address = NULL;
    c->type = None;
    return c;
}
