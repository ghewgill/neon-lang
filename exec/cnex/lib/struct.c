#include "struct.h"

#include <iso646.h>
#include <string.h>

#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "stack.h"



void struct_packIEEE32(struct tagTExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    float x = number_to_float(n);
    Cell *r = cell_createStringCell(sizeof(x));
    memcpy(r->string->data, &x, sizeof(x));
    r->type = cBytes;

    push(exec->stack, r);
}

void struct_packIEEE64(struct tagTExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    double x = number_to_double(n);
    Cell *r = cell_createStringCell(sizeof(x));
    memcpy(r->string->data, &x, sizeof(x));
    r->type = cBytes;

    push(exec->stack, r);
}

void struct_unpackIEEE32(struct tagTExecutor *exec)
{
    TString *b = top(exec->stack)->string;

    float x;
    memcpy(&x, b->data, sizeof(x));

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_float(x)));
}

void struct_unpackIEEE64(struct tagTExecutor *exec)
{
    TString *b = top(exec->stack)->string;

    double x;
    memcpy(&x, b->data, sizeof(x));

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_double(x)));
}
