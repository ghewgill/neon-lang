#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#ifdef HAVE_CLOCK_GETTIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"


static Number NANOSECONDS_PER_SECOND;
static Number MICROSECONDS_PER_SECOND;

void time_initModule()
{
    NANOSECONDS_PER_SECOND = number_from_uint64(1000000000LL);
    MICROSECONDS_PER_SECOND = number_from_uint64(1000000LL);
}

void time_tick(TExecutor *exec)
{
#ifdef HAVE_CLOCK_GETTIME
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    push(exec->stack, cell_fromNumber(number_add(number_from_uint64(ts.tv_sec), number_divide(number_from_uint64(ts.tv_nsec), NANOSECONDS_PER_SECOND))));
#else
    // Not all Linux distros will have clock_gettime(), so we'll resort to gettimeofday() in those cases.  Note the difference between
    // between timespec and timeval structures as well; where timeval has a tv_usec (Microseconds), vs. tv_nsec (Nanoseconds) member.
    // This means we have to divide by 1,000,000, not 1,000,000,000, when using timeval and gettimeofday().
    struct timeval tv;

    gettimeofday(&tv, NULL);

    push(exec->stack, cell_fromNumber(number_add(number_from_uint64(tv.tv_sec), number_divide(number_from_uint64(tv.tv_usec), MICROSECONDS_PER_SECOND))));
#endif
}
