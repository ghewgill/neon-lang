#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define timegm _mkgmtime
#else
#define _DEFAULT_SOURCE    // For timegm on linux
#endif
#include "datetime.h"

#include <time.h>

#include "array.h"
#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"



void datetime_gmtime(TExecutor *exec)
{
    Number t = top(exec->stack)->number;

    time_t x = number_to_sint64(t);
    struct tm *tm = gmtime(&x);

    Cell *r = cell_createArrayCell(0);
    Cell n; n.type = cNumber;
    n.number = number_from_uint32(tm->tm_sec);  cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_min);  cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_hour); cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_mday); cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_mon);  cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_year); cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_wday); cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_yday); cell_arrayAppendElement(r, n);
    n.number = number_from_uint32(tm->tm_isdst);cell_arrayAppendElement(r, n);

    pop(exec->stack);
    push(exec->stack, r);
}

void datetime_timegm(TExecutor *exec)
{
    Cell *t = top(exec->stack);

    struct tm tm;
    tm.tm_sec  = number_to_uint32(t->array->data[0].number);
    tm.tm_min  = number_to_uint32(t->array->data[1].number);
    tm.tm_hour = number_to_uint32(t->array->data[2].number);
    tm.tm_mday = number_to_uint32(t->array->data[3].number);
    tm.tm_mon  = number_to_uint32(t->array->data[4].number);
    tm.tm_year = number_to_uint32(t->array->data[5].number);

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_uint64(timegm(&tm))));
}
