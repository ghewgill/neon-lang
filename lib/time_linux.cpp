#include <time.h>

#include "number.h"

namespace rtl {

namespace ne_time {

static const Number NANOSECONDS_PER_SECOND = number_from_uint64(1000000000LL);

Number tick()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return number_add(number_from_uint64(ts.tv_sec), number_divide(number_from_uint64(ts.tv_nsec), NANOSECONDS_PER_SECOND));
}

} // namespace ne_time

} // namespace rtl
