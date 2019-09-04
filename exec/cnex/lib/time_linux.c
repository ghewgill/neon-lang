#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"


static Number NANOSECONDS_PER_SECOND;

void time_initModule()
{
    NANOSECONDS_PER_SECOND = number_from_uint64(1000000000LL);
}

void time_tick(TExecutor *exec)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    push(exec->stack, cell_fromNumber(number_add(number_from_uint64(ts.tv_sec), number_divide(number_from_uint64(ts.tv_nsec), NANOSECONDS_PER_SECOND))));
}
