#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"

static const uint64_t NANOSECONDS_PER_SECOND = 1000000000LL;

void time_sleep(TExecutor *exec)
{
    // Note: The number on the top of the stack is in SECONDS!
    uint64_t ns = number_to_uint64(number_multiply(top(exec->stack)->number, number_from_uint64(NANOSECONDS_PER_SECOND))); pop(exec->stack);

    // So we must get the correct NANOSECOND value to sleep the correct amount of time.
    struct timespec ts;
    ts.tv_sec = (int)(ns / NANOSECONDS_PER_SECOND);
    ts.tv_nsec = (ns - ((long)ts.tv_sec * NANOSECONDS_PER_SECOND));

    nanosleep(&ts, NULL);
}

void time_now(TExecutor *exec)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        push(exec->stack, cell_fromNumber(number_from_uint32(0)));
        return;
    }
    push(exec->stack, cell_fromNumber(number_add(number_from_uint32(tv.tv_sec), number_divide(number_from_uint32(tv.tv_usec), number_from_uint32(1e6)))));
}
