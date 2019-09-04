#include <stdlib.h>
#include <mach/mach.h>
#include <mach/clock.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"


static Number NANOSECONDS_PER_SECOND;

void time_initModule()
{
    NANOSECONDS_PER_SECOND = number_from_uint64(NSEC_PER_SEC);
}

void time_tick(TExecutor *exec)
{
    clock_serv_t cclock;
    mach_timespec_t mts;

    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);

    push(exec->stack, cell_fromNumber(number_add(number_from_uint64(mts.tv_sec), number_divide(number_from_uint64(mts.tv_nsec), NANOSECONDS_PER_SECOND))));
}
