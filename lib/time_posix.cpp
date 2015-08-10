#include <sys/time.h>
#include <time.h>

#include "number.h"

namespace rtl {

static const Number NANOSECONDS_PER_SECOND = number_from_uint64(1000000000LL);

Number time$now()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        return number_from_uint32(0);
    }
    return number_add(number_from_uint32(tv.tv_sec), number_divide(number_from_uint32(tv.tv_usec), number_from_uint32(1e6)));
}

Number time$tick()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return number_add(number_from_uint64(ts.tv_sec), number_divide(number_from_uint64(ts.tv_nsec), NANOSECONDS_PER_SECOND));
}

} // namespace rtl
