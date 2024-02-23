#include "struct.h"

#include <iso646.h>
#include <string.h>

#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "stack.h"



void struct_packIEEE32BE(struct tagTExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    float x = number_to_float(n);
    Cell *r = cell_createStringCell(sizeof(x));
    uint32_t i = *(uint32_t *)&x;
    r->string->data[0] = (i >> 24) & 0xff;
    r->string->data[1] = (i >> 16) & 0xff;
    r->string->data[2] = (i >>  8) & 0xff;
    r->string->data[3] = (i      ) & 0xff;
    r->type = cBytes;

    push(exec->stack, r);
}

void struct_packIEEE32LE(struct tagTExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    float x = number_to_float(n);
    Cell *r = cell_createStringCell(sizeof(x));
    uint32_t i = *(uint32_t *)&x;
    r->string->data[3] = (i >> 24) & 0xff;
    r->string->data[2] = (i >> 16) & 0xff;
    r->string->data[1] = (i >>  8) & 0xff;
    r->string->data[0] = (i      ) & 0xff;
    r->type = cBytes;

    push(exec->stack, r);
}

void struct_packIEEE64BE(struct tagTExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    double x = number_to_double(n);
    Cell *r = cell_createStringCell(sizeof(x));
    uint64_t i = *(uint64_t *)&x;
    r->string->data[0] = (i >> 56) & 0xff;
    r->string->data[1] = (i >> 48) & 0xff;
    r->string->data[2] = (i >> 40) & 0xff;
    r->string->data[3] = (i >> 32) & 0xff;
    r->string->data[4] = (i >> 24) & 0xff;
    r->string->data[5] = (i >> 16) & 0xff;
    r->string->data[6] = (i >>  8) & 0xff;
    r->string->data[7] = (i      ) & 0xff;
    r->type = cBytes;

    push(exec->stack, r);
}

void struct_packIEEE64LE(struct tagTExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    double x = number_to_double(n);
    Cell *r = cell_createStringCell(sizeof(x));
    uint64_t i = *(uint64_t *)&x;
    r->string->data[7] = (i >> 56) & 0xff;
    r->string->data[6] = (i >> 48) & 0xff;
    r->string->data[5] = (i >> 40) & 0xff;
    r->string->data[4] = (i >> 32) & 0xff;
    r->string->data[3] = (i >> 24) & 0xff;
    r->string->data[2] = (i >> 16) & 0xff;
    r->string->data[1] = (i >>  8) & 0xff;
    r->string->data[0] = (i      ) & 0xff;
    r->type = cBytes;

    push(exec->stack, r);
}

void struct_unpackIEEE32BE(struct tagTExecutor *exec)
{
    TString *b = top(exec->stack)->string;

    uint32_t i = (b->data[0] << 24) |
                 (b->data[1] << 16) |
                 (b->data[2] <<  8) |
                 (b->data[3]      );
    float x = *(float *)&i;

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_float(x)));
}

void struct_unpackIEEE32LE(struct tagTExecutor *exec)
{
    TString *b = top(exec->stack)->string;

    uint32_t i = (b->data[3] << 24) |
                 (b->data[2] << 16) |
                 (b->data[1] <<  8) |
                 (b->data[0]      );
    float x = *(float *)&i;

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_float(x)));
}

void struct_unpackIEEE64BE(struct tagTExecutor *exec)
{
    TString *b = top(exec->stack)->string;

    uint64_t i = ((uint64_t)b->data[0] << 56) |
                 ((uint64_t)b->data[1] << 48) |
                 ((uint64_t)b->data[2] << 40) |
                 ((uint64_t)b->data[3] << 32) |
                 ((uint64_t)b->data[4] << 24) |
                 ((uint64_t)b->data[5] << 16) |
                 ((uint64_t)b->data[6] <<  8) |
                 ((uint64_t)b->data[7]      );

    double x = *(double *)&i;

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_double(x)));
}

void struct_unpackIEEE64LE(struct tagTExecutor *exec)
{
    TString *b = top(exec->stack)->string;

    uint64_t i = ((uint64_t)b->data[7] << 56) |
                 ((uint64_t)b->data[6] << 48) |
                 ((uint64_t)b->data[5] << 40) |
                 ((uint64_t)b->data[4] << 32) |
                 ((uint64_t)b->data[3] << 24) |
                 ((uint64_t)b->data[2] << 16) |
                 ((uint64_t)b->data[1] <<  8) |
                 ((uint64_t)b->data[0]      );

    double x = *(double *)&i;

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_double(x)));
}
