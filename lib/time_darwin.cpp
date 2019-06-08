#include <mach/clock.h>
#include <mach/mach.h>

#include "number.h"

namespace rtl {

namespace ne_time {

static const Number NANOSECONDS_PER_SECOND = number_from_uint64(NSEC_PER_SEC);

Number tick()
{
    clock_serv_t cclock;
    mach_timespec_t mts;

    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);

    return number_add(number_from_uint64(mts.tv_sec), number_divide(number_from_uint64(mts.tv_nsec), NANOSECONDS_PER_SECOND));
}

} // namespace ne_time

} // namespace rtl
